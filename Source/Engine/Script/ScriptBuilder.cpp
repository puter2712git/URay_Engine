#include "ScriptBuilder.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>
#include <Engine/Asset/Script/Script.h>

#include <Windows.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>

namespace URay
{
namespace
{
// Escape MSBuild metacharacters first, then XML attribute characters.
String EscapeCompilePath(const String& value)
{
    constexpr char hex[] = "0123456789ABCDEF";
    String result;
    for (unsigned char c : value)
    {
        if (String("%$@'();?*").find(static_cast<char>(c)) != String::npos)
        {
            result += '%';
            result += hex[c >> 4];
            result += hex[c & 15];
        }
        else if (c == '&') result += "&amp;";
        else if (c == '<') result += "&lt;";
        else if (c == '>') result += "&gt;";
        else if (c == '"') result += "&quot;";
        else result += static_cast<char>(c);
    }
    return result;
}
}

bool ScriptBuilder::CollectInputs(Vector<VirtualPath>& paths, Vector<String>& typeNames) const
{
    auto& fileSystem = assetSystem.GetFileSystem();
    auto& serializer = assetSystem.GetMetadataSerializer();
    for (const Script* script : assetSystem.GetDatabase().GetAssets<Script>())
    {
        const VirtualPath& path = script->GetFilePath();
        if (path.ToString().rfind("Project://Asset/", 0) != 0 ||
            path.GetExtension() != ".cs" || !fileSystem.Exists(path))
            continue;

        AssetMetadata metadata;
        if (!serializer.Load(metadata, assetSystem.GetMetadataPath(path)) ||
            metadata.className != Script::StaticType().name ||
            metadata.handle != script->GetHandle() ||
            metadata.assetPath.ToString() != path.ToString())
            continue;

        if (std::find(typeNames.begin(), typeNames.end(), script->GetTypeName()) != typeNames.end())
        {
            URAY_LOG("[ScriptBuilder] Duplicate script name: %s", script->GetTypeName().c_str());
            return false;
        }
        paths.push_back(path);
        typeNames.push_back(script->GetTypeName());
    }
    return true;
}

bool ScriptBuilder::Build(const VirtualPath& projectPath, Vector<String>& typeNames) const
{
    Vector<VirtualPath> paths;
    typeNames.clear();
    if (!CollectInputs(paths, typeNames))
        return false;
    if (paths.empty())
    {
        URAY_LOG("[ScriptBuilder] No registered project scripts; skipping build.");
        return false;
    }

    auto& fileSystem = assetSystem.GetFileSystem();
    String props = "<Project>\n  <ItemGroup>\n";
    for (const VirtualPath& path : paths)
        props += "    <Compile Include=\"" + EscapeCompilePath(path.GetRelativePath()) + "\" />\n";
    props += "  </ItemGroup>\n</Project>\n";
    if (!fileSystem.WriteText("Project://ScriptInputs.props", props))
    {
        URAY_LOG("[ScriptBuilder] Failed to write script inputs.");
        return false;
    }

    // Build a generated copy so projects created before ScriptInputs.props was
    // introduced also get the import without rewriting the user's project.
    const VirtualPath projectFile("Project://Project.Scripts.csproj");
    String projectText = fileSystem.ReadText(projectFile);
    if (projectText.empty())
    {
        URAY_LOG("[ScriptBuilder] Project.Scripts.csproj is missing or unreadable.");
        return false;
    }
    if (projectText.find("ScriptInputs.props") == String::npos)
    {
        const size_t end = projectText.rfind("</Project>");
        if (end == String::npos)
        {
            URAY_LOG("[ScriptBuilder] Invalid Project.Scripts.csproj.");
            return false;
        }
        projectText.insert(end, "  <Import Project=\"ScriptInputs.props\" />\n");
    }
    if (!fileSystem.WriteText("Project://.URay.ScriptBuild.csproj", projectText))
    {
        URAY_LOG("[ScriptBuilder] Failed to write generated script project.");
        return false;
    }

    const std::filesystem::path project = std::filesystem::absolute(std::filesystem::u8path(projectPath.ToString()));
    const std::filesystem::path logPath = project / L"Log" / L"ScriptBuild.log";
    std::error_code error;
    std::filesystem::create_directories(logPath.parent_path(), error);
    if (error)
        return false;

    SECURITY_ATTRIBUTES security{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    HANDLE log = CreateFileW(logPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &security, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (log == INVALID_HANDLE_VALUE)
        return false;

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    startup.hStdOutput = log;
    startup.hStdError = log;
    HANDLE input = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               &security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (input == INVALID_HANDLE_VALUE)
    {
        CloseHandle(log);
        return false;
    }
    startup.hStdInput = input;
    PROCESS_INFORMATION process{};
    std::wstring command = L"dotnet build \".URay.ScriptBuild.csproj\" --nologo";
    const BOOL started = CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE,
                                        CREATE_NO_WINDOW, nullptr, project.c_str(), &startup, &process);
    const DWORD startError = started ? 0 : GetLastError();
    CloseHandle(log);
    CloseHandle(input);
    if (!started)
    {
        URAY_LOG("[ScriptBuilder] Could not start dotnet build (error %lu).", startError);
        return false;
    }
    WaitForSingleObject(process.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(process.hProcess, &exitCode);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    if (exitCode != 0)
    {
        std::ifstream input(logPath, std::ios::binary);
        String output((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        URAY_LOG("[ScriptBuilder] Build failed (%lu). See Log/ScriptBuild.log.\n%s", exitCode, output.c_str());
        return false;
    }
    return true;
}
} // namespace URay
