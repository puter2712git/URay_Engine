#pragma once

#include <imgui/imgui.h>
#include <vulkan/vulkan.h>

#include <memory>

namespace URay::RHI
{

class Texture;
class TextureView;

class RenderTarget
{
public:
    TextureView* GetColorView() const { return colorTextureView.get(); }
    VkFramebuffer GetFramebuffer() const { return framebuffer; }
    VkExtent2D GetExtent() const { return extent; }

private:
    std::unique_ptr<Texture> colorTexture = nullptr;
    std::unique_ptr<TextureView> colorTextureView = nullptr;

    std::unique_ptr<Texture> depthTexture = nullptr;
    std::unique_ptr<TextureView> depthTextureView = nullptr;

    VkFramebuffer framebuffer = VK_NULL_HANDLE;

    VkExtent2D extent = {};

    ImTextureID imguiTexture = nullptr;
};

} // namespace URay::RHI
