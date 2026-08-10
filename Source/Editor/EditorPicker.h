#pragma once

namespace URay
{

class Engine;
class Unit;
class CameraComponent;
class MeshComponent;

struct Ray;

struct PickResult
{
    bool hit = false;
    Unit* pickedUnit = nullptr;

    int gizmoAxis = -1;
};

class EditorPicker
{
public:
    EditorPicker(Engine& engine);

public:
    PickResult Pick(CameraComponent* camera, float screenX, float screenY) const;

private:
    bool PickGizmo(const Ray& ray, int& outAxis) const;
    bool PickMesh(const Ray& ray, const MeshComponent* meshComp, float& minDist) const;

private:
    Engine& engine;
};

} // namespace URay
