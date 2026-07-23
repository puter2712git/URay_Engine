#include "PipelineState.h"

#include "Render/Shader/Shader.h"

namespace URay
{

uint64_t PipelineState::GetKey() const
{
    const uint64_t shaderId = shader ? shader->GetId() : 0;

    uint64_t key = 0;
    key |= (shaderId << 32);
    key |= static_cast<uint64_t>(topology);
    key |= static_cast<uint64_t>(depthStencil.depthTestEnable) << 8;
    key |= static_cast<uint64_t>(depthStencil.depthWriteEnable) << 9;
    key |= static_cast<uint64_t>(depthStencil.depthCompareOp) << 10;
    key |= static_cast<uint64_t>(depthStencil.stencilTestEnable) << 12;

    return key;
}

} // namespace URay
