#include "PipelineState.h"

#include "Render/Shader/Shader.h"

namespace URay
{

uint64_t PipelineState::GetKey() const
{
    const uint64_t shaderId = shader ? shader->GetId() : 0;
    const uint64_t topologyId = static_cast<uint64_t>(topology);

    uint64_t key = 0;
    key |= (shaderId << 32);
    key |= topologyId;

    return key;
}

} // namespace URay
