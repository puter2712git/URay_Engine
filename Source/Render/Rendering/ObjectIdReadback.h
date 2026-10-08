#pragma once

#include <Render/RHI/Texture/Texture.h>

#include <Core/Type/Types.h>

#include <memory>
#include <optional>

namespace URay::Render
{

class Buffer;
class CommandBuffer;
class Device;
class Texture;

struct ObjectIdReadbackResult
{
    uint64 requestId = 0;
    uint32 objectId = 0;
};

class ObjectIdReadback
{
public:
    ObjectIdReadback(Device& device);
    ~ObjectIdReadback();

public:
    bool Initialize();
    void Finalize();

    std::optional<uint64> Request(uint32 x, uint32 y);

    void Record(CommandBuffer& commandBuffer, Texture& idTexture, uint32 frameIndex);

    std::optional<ObjectIdReadbackResult> ConsumeCompleted(uint32 completedFrameIndex);

private:
    struct PendingRequest
    {
        uint64 requestId = 0;
        TextureRegion region = {};
    };

    struct InFlightRequest
    {
        uint64 requestId = 0;
        uint32 frameIndex = 0;
    };

    Device& device;
    std::unique_ptr<Buffer> buffer = nullptr;

    std::optional<PendingRequest> pendingRequest = std::nullopt;
    std::optional<InFlightRequest> inFlightRequest = std::nullopt;

    uint64 nextRequestId = 1;
};

} // namespace URay::Render
