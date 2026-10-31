#pragma once

#include <Editor/Widget/Panel/Panel.h>

#include <memory>

namespace URay
{

class MeshPreviewSession;
struct InputSnapshot;

class MeshPreviewPanel final : public Panel
{
    URAY_CLASS(MeshPreviewPanel, Panel)

public:
    explicit MeshPreviewPanel(std::unique_ptr<MeshPreviewSession> session);
    ~MeshPreviewPanel() override;

public:
    MeshPreviewSession& GetSession() const { return *session; }

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;
    void OnPointerCancel() override;

protected:
    void OnUpdate(float deltaTime) override;
    void OnPaint(Render::UIDrawContext& context) override;

private:
    const InputSnapshot* FindWindowInputSnapshot() const;

private:
    std::unique_ptr<MeshPreviewSession> session;
    bool isCameraRotating = false;
    Vector2 cameraMouseDelta = Vector2::Zero;
};

} // namespace URay
