#include "Render/Shader/Reflection/ShaderReflector.h"

#include "Core/Type/Types.h"

#include <utility>

namespace URay::Render
{

namespace
{

ShaderStageFlags ToShaderStageFlags(SpvReflectShaderStageFlagBits stage)
{
    switch (stage)
    {
    case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
        return ShaderStageFlags::Vertex;
    case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
        return ShaderStageFlags::Fragment;
    default:
        return ShaderStageFlags{};
    }
}

bool ToShaderResourceType(
    SpvReflectDescriptorType source,
    ShaderResourceType& outType)
{
    switch (source)
    {
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
        outType = ShaderResourceType::Sampler;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        outType = ShaderResourceType::CombinedImageSampler;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        outType = ShaderResourceType::SampledImage;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        outType = ShaderResourceType::StorageImage;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        outType = ShaderResourceType::UniformBuffer;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        outType = ShaderResourceType::StorageBuffer;
        return true;
    default:
        return false;
    }
}

ShaderValueType ToShaderValueType(const SpvReflectBlockVariable& variable)
{
    if (variable.type_description == nullptr)
        return ShaderValueType::Scalar;

    const SpvReflectTypeFlags flags = variable.type_description->type_flags;
    if ((flags & SPV_REFLECT_TYPE_FLAG_STRUCT) != 0)
        return ShaderValueType::Struct;
    if ((flags & SPV_REFLECT_TYPE_FLAG_MATRIX) != 0)
        return ShaderValueType::Matrix;
    if ((flags & SPV_REFLECT_TYPE_FLAG_VECTOR) != 0)
        return ShaderValueType::Vector;

    return ShaderValueType::Scalar;
}

ShaderScalarType ToShaderScalarType(const SpvReflectBlockVariable& variable)
{
    if (variable.type_description == nullptr)
        return ShaderScalarType::Unknown;

    const SpvReflectTypeFlags flags = variable.type_description->type_flags;
    if ((flags & SPV_REFLECT_TYPE_FLAG_BOOL) != 0)
        return ShaderScalarType::Bool;
    if ((flags & SPV_REFLECT_TYPE_FLAG_FLOAT) != 0)
    {
        return variable.numeric.scalar.width == 64
            ? ShaderScalarType::Double
            : ShaderScalarType::Float;
    }
    if ((flags & SPV_REFLECT_TYPE_FLAG_INT) != 0)
    {
        return variable.numeric.scalar.signedness != 0
            ? ShaderScalarType::Int
            : ShaderScalarType::UInt;
    }

    return ShaderScalarType::Unknown;
}

} // namespace

bool ShaderReflector::ReflectSPIRV(
    const std::vector<uint8>& code,
    const std::string& entryPoint,
    ShaderReflection& outReflection)
{
    outReflection = {};

    SpvReflectShaderModule module = {};
    if (spvReflectCreateShaderModule(
            code.size(),
            code.data(),
            &module) != SPV_REFLECT_RESULT_SUCCESS)
    {
        return false;
    }

    const SpvReflectEntryPoint* reflectedEntryPoint =
        spvReflectGetEntryPoint(&module, entryPoint.c_str());

    if (!reflectedEntryPoint)
    {
        spvReflectDestroyShaderModule(&module);
        return false;
    }

    outReflection.entryPoint = reflectedEntryPoint->name;
    outReflection.stage = ToShaderStageFlags(reflectedEntryPoint->shader_stage);

    const bool succeeded =
        ReflectDescriptorBindings(module, reflectedEntryPoint->name, outReflection) &&
        ReflectPushConstantBlocks(module, reflectedEntryPoint->name, outReflection);

    spvReflectDestroyShaderModule(&module);

    return succeeded;
}

bool ShaderReflector::ReflectDescriptorBindings(
    const SpvReflectShaderModule& module,
    const char* entryPoint,
    ShaderReflection& outReflection)
{
    uint32 count = 0;
    if (spvReflectEnumerateEntryPointDescriptorBindings(
            &module, entryPoint, &count, nullptr) != SPV_REFLECT_RESULT_SUCCESS)
    {
        return false;
    }

    std::vector<SpvReflectDescriptorBinding*> sourceBindings(count);
    if (spvReflectEnumerateEntryPointDescriptorBindings(
            &module, entryPoint, &count, sourceBindings.data()) != SPV_REFLECT_RESULT_SUCCESS)
    {
        return false;
    }

    outReflection.descriptorBindings.clear();
    outReflection.descriptorBindings.reserve(count);

    for (const SpvReflectDescriptorBinding* source : sourceBindings)
    {
        ReflectedDescriptorBinding result = {};
        result.name = source->name ? source->name : "";
        result.set = source->set;
        result.binding = source->binding;
        result.stages = outReflection.stage;
        result.arrayCount = source->count;

        for (uint32 i = 0; i < source->array.dims_count; ++i)
        {
            if (source->array.dims[i] == SPV_REFLECT_ARRAY_DIM_RUNTIME)
            {
                result.isRuntimeArray = true;
                break;
            }
        }

        if (!ToShaderResourceType(source->descriptor_type, result.resourceType))
            return false;

        const bool nonWritable =
            (source->decoration_flags & SPV_REFLECT_DECORATION_NON_WRITABLE) != 0;
        const bool nonReadable =
            (source->decoration_flags & SPV_REFLECT_DECORATION_NON_READABLE) != 0;

        result.readOnly = nonWritable ||
            source->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER ||
            source->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
            source->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
            source->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        result.writeOnly = nonReadable;

        outReflection.descriptorBindings.push_back(std::move(result));
    }

    return true;
}

bool ShaderReflector::ReflectPushConstantBlocks(
    const SpvReflectShaderModule& module,
    const char* entryPoint,
    ShaderReflection& outReflection)
{
    uint32 count = 0;
    if (spvReflectEnumerateEntryPointPushConstantBlocks(
            &module, entryPoint, &count, nullptr) != SPV_REFLECT_RESULT_SUCCESS)
    {
        return false;
    }

    std::vector<SpvReflectBlockVariable*> sourceBlocks(count);
    if (spvReflectEnumerateEntryPointPushConstantBlocks(
            &module, entryPoint, &count, sourceBlocks.data()) != SPV_REFLECT_RESULT_SUCCESS)
    {
        return false;
    }

    outReflection.pushConstantBlocks.clear();
    outReflection.pushConstantBlocks.reserve(count);

    for (const SpvReflectBlockVariable* source : sourceBlocks)
    {
        ReflectedPushConstantBlock result = {};
        result.name = source->name ? source->name : "";
        result.offset = source->offset;
        result.size = source->size;
        result.members.reserve(source->member_count);

        for (uint32 i = 0; i < source->member_count; ++i)
            result.members.push_back(ReflectBlockMember(source->members[i]));

        outReflection.pushConstantBlocks.push_back(std::move(result));
    }

    return true;
}

ReflectedBlockMember ShaderReflector::ReflectBlockMember(
    const SpvReflectBlockVariable& variable)
{
    ReflectedBlockMember result = {};

    result.name = variable.name ? variable.name : "";
    result.offset = variable.offset;
    result.absoluteOffset = variable.absolute_offset;
    result.size = variable.size;
    result.paddedSize = variable.padded_size;
    result.valueType = ToShaderValueType(variable);
    result.scalarType = ToShaderScalarType(variable);

    if (variable.numeric.vector.component_count > 0)
        result.vectorComponentCount = variable.numeric.vector.component_count;
    if (variable.numeric.matrix.column_count > 0)
        result.matrixColumnCount = variable.numeric.matrix.column_count;
    if (variable.numeric.matrix.row_count > 0)
        result.matrixRowCount = variable.numeric.matrix.row_count;
    result.arrayStride = variable.array.stride;
    result.matrixStride = variable.numeric.matrix.stride;

    result.isRowMajor =
        (variable.decoration_flags & SPV_REFLECT_DECORATION_ROW_MAJOR) != 0;

    if (variable.array.dims_count > 0)
    {
        result.arrayCount = 1;
        for (uint32 i = 0; i < variable.array.dims_count; ++i)
        {
            const uint32 dimension = variable.array.dims[i];
            if (dimension == SPV_REFLECT_ARRAY_DIM_RUNTIME)
            {
                result.arrayCount = 0;
                result.isRuntimeArray = true;
                break;
            }

            result.arrayCount *= dimension;
        }
    }

    result.members.reserve(variable.member_count);
    for (uint32 i = 0; i < variable.member_count; ++i)
    {
        result.members.push_back(ReflectBlockMember(variable.members[i]));
    }

    return result;
}

} // namespace URay::Render
