#pragma once

#include <Core/Type/TypeAlias.h>

#include <coreclr_delegates.h>

#include <string_view>

namespace URay
{

class VirtualPath;

class DotNetRuntime
{
public:
    bool Initialize(const VirtualPath& enginePath);
    void Finalize();

    template <typename Fn>
    Fn GetFunction(const VirtualPath& assemblyPath, std::wstring_view typeName, std::wstring_view methodName)
    {
        static_assert(
            std::is_pointer_v<Fn> &&
                std::is_function_v<std::remove_pointer_t<Fn>>,
            "Fn must be a function pointer type.");

        return reinterpret_cast<Fn>(
            GetFunctionAddress(assemblyPath, typeName, methodName));
    }

private:
    void* GetFunctionAddress(const VirtualPath& assemblyPath, std::wstring_view typeName, std::wstring_view methodName);

private:
    void* library = nullptr;
    load_assembly_and_get_function_pointer_fn loadAssembly = nullptr;
};

} // namespace URay
