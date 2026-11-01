#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/RHI/Barrier/BufferBarrier.h>
#include <Render/RHI/Barrier/ImageBarrier.h>

#include <span>

namespace URay::Render
{

struct PipelineBarrierDesc
{
    Span<const BufferBarrierDesc> bufferBarriers = {};
    Span<const ImageBarrierDesc> imageBarriers = {};
};

} // namespace URay::Render
