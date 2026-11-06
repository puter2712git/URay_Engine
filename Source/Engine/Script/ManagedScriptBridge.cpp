#include "ManagedScriptBridge.h"

#include <Core/File/VirtualPath.h>
#include <Core/Log/LogSystem.h>
#include <Core/Math/Vector3.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Object/ObjectId.h>
#include <Engine/Script/DotNetRuntime.h>
#include <Engine/Script/Native/NativeAPI.h>

#include <cstring>
#include <filesystem>
#include <limits>

namespace URay
{

namespace
{

int32 __cdecl CollectScriptType(void* context, const char* typeName) noexcept
{
    if (!context || !typeName)
        return -1;

    try
    {
        auto& types = *static_cast<Vector<String>*>(context);
        types.emplace_back(typeName);
        return 0;
    }
    catch (...)
    {
        return -1;
    }
}

int32 __cdecl CollectScriptField(
    void* context,
    const char* typeName,
    const char* fieldName,
    int32 type,
    const uint8* data,
    int32 size) noexcept
{
    if (!context || !typeName || !fieldName)
        return -1;

    static_assert(sizeof(int32) == 4);
    static_assert(sizeof(float) == 4);

    try
    {
        ScriptFieldInfo fieldInfo = {};
        fieldInfo.declaringTypeName = typeName;
        fieldInfo.name = fieldName;
        fieldInfo.type = static_cast<ScriptFieldType>(type);

        switch (fieldInfo.type)
        {
        case ScriptFieldType::Bool:
        {
            if (!data || size != sizeof(int32))
                return -1;

            int32 value;
            std::memcpy(&value, data, sizeof(value));

            if (value != 0 && value != 1)
                return -1;

            fieldInfo.defaultValue.emplace(value == 1);
            break;
        }

        case ScriptFieldType::Int:
        {
            if (!data || size != sizeof(int32))
                return -1;

            int32 value;
            std::memcpy(&value, data, sizeof(int32));
            fieldInfo.defaultValue.emplace(value);
            break;
        }

        case ScriptFieldType::Float:
        {
            if (!data || size != sizeof(float))
                return -1;

            float value;
            std::memcpy(&value, data, sizeof(value));
            fieldInfo.defaultValue.emplace(value);
            break;
        }

        case ScriptFieldType::Vector3:
        {
            if (!data || size != sizeof(float) * 3)
                return -1;

            float values[3];
            std::memcpy(values, data, sizeof(values));

            fieldInfo.defaultValue.emplace(Vector3(values[0], values[1], values[2]));
            break;
        }

        case ScriptFieldType::String:
        {
            if (size == -1)
            {
                if (data)
                    return -1;

                break;
            }

            if (size < 0 || (size > 0 && !data))
                return -1;

            String value;

            if (size > 0)
            {
                value.assign(
                    reinterpret_cast<const char*>(data),
                    static_cast<size_t>(size));
            }

            fieldInfo.defaultValue.emplace(value);
            break;
        }

        default:
            return -1;
        }

        auto& fields = *static_cast<Vector<ScriptFieldInfo>*>(context);
        fields.push_back(std::move(fieldInfo));
        return 0;
    }
    catch (...)
    {
        return -1;
    }
}

} // namespace

ManagedScriptBridge::ManagedScriptBridge() = default;

ManagedScriptBridge::~ManagedScriptBridge() = default;

bool ManagedScriptBridge::Initialize(const VirtualPath& enginePath, const VirtualPath& projectPath, bool shouldLoadProjectAssembly)
{
    runtime = MakeUnique<DotNetRuntime>();
    if (!runtime->Initialize(enginePath))
    {
        Finalize();
        return false;
    }

    using ManagedInitializeFn = int32(__cdecl*)(const NativeAPI*);

    const auto init = runtime->GetFunction<ManagedInitializeFn>(
        enginePath.Join("URay.Managed.dll"),
        L"URay.EntryPoint, URay.Managed",
        L"Initialize");

    if (!init)
    {
        URAY_LOG("[ManagedScriptBridge] Managed Initialize was not found.");
        Finalize();
        return false;
    }

    const NativeAPI api{
        .logAPI = CreateLogAPI(),
        .inputAPI = CreateInputAPI(),
        .objectAPI = CreateObjectAPI(),
        .unitAPI = CreateUnitAPI(),
        .transformAPI = CreateTransformAPI()
    };

    const int32 result = init(&api);

    if (result != 0)
    {
        URAY_LOG("[ManagedScriptBridge] Managed initialization failed: %d", result);
        Finalize();
        return false;
    }

    const VirtualPath assemblyPath = enginePath.Join("URay.Managed.dll");
    constexpr auto typeName = L"URay.EntryPoint, URay.Managed";

    createScript = runtime->GetFunction<CreateScriptFn>(assemblyPath, typeName, L"CreateScript");
    updateScript = runtime->GetFunction<UpdateScriptFn>(assemblyPath, typeName, L"UpdateScript");
    unloadScript = runtime->GetFunction<UnloadScriptFn>(assemblyPath, typeName, L"UnloadScript");
    enumerateScriptTypes = runtime->GetFunction<EnumerateScriptTypesFn>(assemblyPath, typeName, L"EnumerateScriptTypes");
    enumerateScriptFields = runtime->GetFunction<EnumerateScriptFieldsFn>(assemblyPath, typeName, L"EnumerateScriptFields");

    if (!createScript || !updateScript || !unloadScript || !enumerateScriptTypes || !enumerateScriptFields)
    {
        URAY_LOG("[ManagedScriptBridge] A managed script entry point were not found.");
        Finalize();
        return false;
    }

    if (!shouldLoadProjectAssembly)
        return true;

    using LoadProjectAssemblyFn = int32(__cdecl*)(const char*);

    const auto loadProjectAssembly = runtime->GetFunction<LoadProjectAssemblyFn>(
        assemblyPath,
        typeName,
        L"LoadProjectAssembly");

    if (!loadProjectAssembly)
    {
        URAY_LOG("[ManagedScriptBridge] LoadProjectAssembly was not found.");
        Finalize();
        return false;
    }

    const VirtualPath projectAssemblyPath = projectPath.Join("Bin/Project.Scripts.dll");

    const std::filesystem::path physicalPath = std::filesystem::absolute(
        std::filesystem::u8path(projectAssemblyPath.ToString()));

    const std::u8string utf8Path = physicalPath.u8string();
    const String pathString(utf8Path.begin(), utf8Path.end());

    const int32 loadResult = loadProjectAssembly(pathString.c_str());

    if (loadResult == 1)
    {
        URAY_LOG(
            "[ManagedScriptBridge] Project script assembly was not found: %s",
            pathString.c_str());
    }
    else if (loadResult != 0)
    {
        URAY_LOG(
            "[ManagedScriptBridge] Failed to load project script assembly: %s",
            pathString.c_str());
        return true;
    }

    return true;
}

void ManagedScriptBridge::Finalize()
{
    createScript = nullptr;
    updateScript = nullptr;
    unloadScript = nullptr;
    enumerateScriptTypes = nullptr;
    enumerateScriptFields = nullptr;

    if (runtime)
    {
        runtime->Finalize();
        runtime.reset();
    }
}

int32 ManagedScriptBridge::CreateScript(
    const char* typeName,
    const ObjectId& ownerId,
    const Vector<ScriptFieldOverride>& overrides)
{
    if (!createScript || !typeName)
        return 0;

    if (overrides.size() > static_cast<size_t>(std::numeric_limits<int32>::max()))
        return 0;

    Vector<Array<uint8, 12>> buffers(overrides.size());

    Vector<ScriptFieldTransfer> transfers;
    transfers.reserve(overrides.size());

    for (uint32 index = 0; index < overrides.size(); ++index)
    {
        const ScriptFieldOverride& field = overrides[index];

        ScriptFieldTransfer transfer{};
        transfer.declaringTypeName = field.declaringTypeName.c_str();
        transfer.name = field.name.c_str();
        transfer.type = static_cast<int32>(field.type);

        switch (field.type)
        {
        case ScriptFieldType::Bool:
        {
            const bool* value = field.value ? field.value->TryGet<bool>() : nullptr;
            if (!value)
                return 0;

            const int32 data = *value ? 1 : 0;
            std::memcpy(buffers[index].data(), &data, sizeof(data));

            transfer.data = buffers[index].data();
            transfer.size = static_cast<int32>(sizeof(data));
            break;
        }

        case ScriptFieldType::Int:
        {
            const int32* value = field.value ? field.value->TryGet<int32>() : nullptr;
            if (!value)
                return 0;

            std::memcpy(buffers[index].data(), value, sizeof(*value));

            transfer.data = buffers[index].data();
            transfer.size = static_cast<int32>(sizeof(*value));
            break;
        }

        case ScriptFieldType::Float:
        {
            const float* value = field.value ? field.value->TryGet<float>() : nullptr;
            if (!value)
                return 0;

            std::memcpy(buffers[index].data(), value, sizeof(*value));

            transfer.data = buffers[index].data();
            transfer.size = static_cast<int32>(sizeof(*value));
            break;
        }

        case ScriptFieldType::Vector3:
        {
            const Vector3* value = field.value ? field.value->TryGet<Vector3>() : nullptr;
            if (!value)
                return 0;

            const float data[3] = { value->x, value->y, value->z };
            std::memcpy(buffers[index].data(), data, sizeof(data));

            transfer.data = buffers[index].data();
            transfer.size = static_cast<int32>(sizeof(data));
            break;
        }

        case ScriptFieldType::String:
        {
            if (!field.value)
            {
                transfer.data = nullptr;
                transfer.size = -1;
                break;
            }

            const String* value = field.value->TryGet<String>();

            if (!value || value->size() > static_cast<size_t>(std::numeric_limits<int32>::max()))
                return 0;

            transfer.data = reinterpret_cast<const uint8*>(value->data());
            transfer.size = static_cast<int32>(value->size());
            break;
        }

        default:
            return 0;
        }

        transfers.push_back(transfer);
    }

    return createScript(
        typeName,
        ownerId.index,
        ownerId.generation,
        transfers.empty() ? nullptr : transfers.data(),
        static_cast<int32>(transfers.size()));
}

int32 ManagedScriptBridge::UpdateScript(int32 handle, float deltaTime)
{
    if (!updateScript || handle <= 0)
        return -1;

    return updateScript(handle, deltaTime);
}

int32 ManagedScriptBridge::UnloadScript(int32 handle)
{
    if (!unloadScript || handle <= 0)
        return -1;

    return unloadScript(handle);
}

bool ManagedScriptBridge::GetScriptTypes(Vector<String>& outTypes)
{
    if (!enumerateScriptTypes)
        return false;

    Vector<String> discoveredTypes;

    const int32 result = enumerateScriptTypes(&CollectScriptType, &discoveredTypes);

    if (result != 0)
    {
        URAY_LOG("[ManagedScriptBridge] Script type discovery failed: %d", result);
        return false;
    }

    outTypes = std::move(discoveredTypes);
    return true;
}

bool ManagedScriptBridge::GetScriptFields(const char* typeName, Vector<ScriptFieldInfo>& outFields)
{
    if (!enumerateScriptFields || !typeName)
        return false;

    Vector<ScriptFieldInfo> discoveredFields;

    const int32 result = enumerateScriptFields(
        typeName,
        &CollectScriptField,
        &discoveredFields);

    if (result != 0)
    {
        URAY_LOG(
            "[ManagedScriptBridge] Script field discovery failed: %s (%d)",
            typeName,
            result);
        return false;
    }

    outFields = std::move(discoveredFields);
    return true;
}

} // namespace URay
