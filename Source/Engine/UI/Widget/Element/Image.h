#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Engine/Asset/AssetHandle.h>

#include <Core/Math/Color.h>

namespace URay
{

class Image final : public Widget
{
public:
    explicit Image(AssetHandle textureHandle);

public:
    bool CanHit() const override { return false; }

    void SetTexture(AssetHandle textureHandle) { this->textureHandle = textureHandle; }
    void SetColor(const Color& color) { this->color = color; }

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    AssetHandle textureHandle;
    Color color = Color::White;
};

} // namespace URay
