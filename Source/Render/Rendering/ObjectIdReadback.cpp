#include "ObjectIdReadback.h"

#include <Render/RHI/Buffer/Buffer.h>
#include <Render/RHI/Buffer/BufferDesc.h>
#include <Render/RHI/CommandBuffer/CommandBuffer.h>
#include <Render/RHI/Device.h>

#include <Core/Log/LogSystem.h>

namespace URay::Render
{

ObjectIdReadback::ObjectIdReadback(Device& device) : device(device) {}

ObjectIdReadback::~ObjectIdReadback() = default;

bool ObjectIdReadback::Initialize()
{
    BufferDesc desc = {};
    desc.size = sizeof(uint32);
    desc.bindFlags = BufferBindFlags::CopyDst;
    desc.memoryUsage = MemoryUsage::GpuToCpu;

    buffer.reset(device.CreateBuffer(desc));
    if (!buffer)
    {
        URAY_LOG("[ObjectIdReadback] Failed to create object ID readback buffer.");
        Finalize();
        return false;
    }

    return true;
}

void ObjectIdReadback::Finalize()
{
    if (buffer)
    {
        buffer.reset();
    }
}

std::optional<uint64> ObjectIdReadback::Request(uint32 x, uint32 y)
{
    if (pendingRequest || inFlightRequest)
        return std::nullopt;

    const uint64 requestId = nextRequestId++;

    TextureRegion region = {
        .x = x,
        .y = y,
        .width = 1,
        .height = 1
    };

    pendingRequest = PendingRequest{
        .requestId = requestId,
        .region = region
    };

    return requestId;
}

void ObjectIdReadback::Record(
    CommandBuffer& commandBuffer,
    Texture& idTexture,
    uint32 frameIndex)
{
    if (!pendingRequest || inFlightRequest)
        return;

    idTexture.Transition(commandBuffer, ImageLayout::TransferSrc);

    commandBuffer.CopyTextureToBuffer(idTexture, *buffer, pendingRequest->region);

    inFlightRequest = InFlightRequest{
        .requestId = pendingRequest->requestId,
        .frameIndex = frameIndex
    };

    pendingRequest.reset();
}

std::optional<ObjectIdReadbackResult> ObjectIdReadback::ConsumeCompleted(uint32 completedFrameIndex)
{
    if (!inFlightRequest || inFlightRequest->frameIndex != completedFrameIndex)
        return std::nullopt;

    const uint32* objectId = static_cast<const uint32*>(buffer->Map());
    if (!objectId)
    {
        inFlightRequest.reset();
        return std::nullopt;
    }

    const ObjectIdReadbackResult result = {
        .requestId = inFlightRequest->requestId,
        .objectId = *objectId
    };

    buffer->Unmap();
    inFlightRequest.reset();
    return result;
}

} // namespace URay::Render
