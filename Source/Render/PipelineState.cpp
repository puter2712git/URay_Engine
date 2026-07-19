#include "PipelineState.h"

#include "Render/Shader/Shader.h"

namespace URay
{

uint64_t PipelineState::GetKey() const
{
    uint64_t vsId = vertexShader ? vertexShader->GetId() : 0;
    uint64_t fsId = fragmentShader ? fragmentShader->GetId() : 0;

    uint64_t key = 0;
    key |= (vsId << 32);
    key |= (fsId << 8);

    return key;
}

} // namespace URay
