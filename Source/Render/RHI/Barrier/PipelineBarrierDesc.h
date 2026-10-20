#pragma once

#include <Render/RHI/Barrier/BufferBarrier.h>
#include <Render/RHI/Barrier/ImageBarrier.h>

#include <span>

namespace URay::Render
{

struct PipelineBarrierDesc
{
    std::span<const BufferBarrierDesc> bufferBarriers = {};
    std::span<const ImageBarrierDesc> imageBarriers = {};
};

} // namespace URay::Render
