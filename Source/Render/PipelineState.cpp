#include "PipelineState.h"

#include "Render/Shader/Shader.h"

namespace URay
{

uint64_t PipelineState::GetKey() const
{
    uint64_t shaderId = shader ? shader->GetId() : 0;

    uint64_t key = 0;
    key |= (shaderId << 32);

    return key;
}

} // namespace URay
