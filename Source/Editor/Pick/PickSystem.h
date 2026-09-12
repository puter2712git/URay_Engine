#pragma once

#include "Core/Type/Types.h"

#include <memory>
#include <unordered_map>

namespace URay
{

class Component;
class CameraComponent;
class PickObject;
class Unit;
class GizmoController;
struct Ray;

struct PickResult
{
    bool isHit = false;
    Unit* pickedUnit = nullptr;
    int32 gizmoAxis = -1;
};

class PickSystem
{
public:
    explicit PickSystem(GizmoController& gizmo);
    ~PickSystem();

public:
    bool Initialize();
    void Finalize();

    PickResult Pick(CameraComponent* camera, float screenX, float screenY) const;

private:
    bool PickGizmo(const Ray& worldRay, int32& outAxis) const;

private:
    GizmoController& gizmo;

    std::unordered_map<Component*, std::unique_ptr<PickObject>> pickObjects;
};

} // namespace URay
