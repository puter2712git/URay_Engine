#pragma once

#include "Render/Shader/ShaderStageFlags.h"

#include "Core/Type/Types.h"

#include <string>
#include <vector>

namespace URay::Render
{

enum class ShaderValueType
{
    Scalar,
    Vector,
    Matrix,
    Struct
};

enum class ShaderScalarType
{
    Unknown,
    Bool,
    Int,
    UInt,
    Float,
    Double
};

enum class ShaderResourceType
{
    Sampler,
    CombinedImageSampler,
    SampledImage,
    StorageImage,
    UniformBuffer,
    StorageBuffer,
};

struct ReflectedBlockMember
{
    std::string name;

    uint32 offset = 0;
    uint32 absoluteOffset = 0;
    uint32 size = 0;
    uint32 paddedSize = 0;

    ShaderValueType valueType = ShaderValueType::Scalar;
    ShaderScalarType scalarType = ShaderScalarType::Unknown;
    uint32 vectorComponentCount = 1;
    uint32 matrixColumnCount = 1;
    uint32 matrixRowCount = 1;

    uint32 arrayCount = 0;
    uint32 arrayStride = 0;
    uint32 matrixStride = 0;
    bool isRuntimeArray = false;
    bool isRowMajor = false;

    std::vector<ReflectedBlockMember> members;
};

struct ReflectedDescriptorBinding
{
    std::string name;
    uint32 set = 0;
    uint32 binding = 0;

    ShaderResourceType resourceType = ShaderResourceType::Sampler;
    ShaderStageFlags stages = ShaderStageFlags::None;
    uint32 arrayCount = 1;
    bool isRuntimeArray = false;

    ShaderScalarType sampledScalarType = ShaderScalarType::Unknown;
    bool readOnly = true;
    bool writeOnly = false;
};

struct ReflectedPushConstantBlock
{
    std::string name;

    uint32 offset = 0;
    uint32 size = 0;

    std::vector<ReflectedBlockMember> members;
};

struct ShaderReflection
{
    std::string entryPoint;
    ShaderStageFlags stage = ShaderStageFlags::None;

    std::vector<ReflectedDescriptorBinding> descriptorBindings;
    std::vector<ReflectedPushConstantBlock> pushConstantBlocks;
};

struct MergedPushConstantBlock
{
    ReflectedPushConstantBlock block;
    ShaderStageFlags stages = ShaderStageFlags::None;
};

struct ShaderPipelineReflection
{
    std::vector<ReflectedDescriptorBinding> descriptorBindings;
    std::vector<MergedPushConstantBlock> pushConstantBlocks;
};

} // namespace URay::Render
