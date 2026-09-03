#include "DecalPass.h"

namespace URay::Render
{

DecalPass::DecalPass() = default;

DecalPass::~DecalPass() = default;

void DecalPass::Begin(const RenderPassContext& context)
{
}

void DecalPass::End(const RenderPassContext& context)
{
}

void DecalPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
}

} // namespace URay::Render
