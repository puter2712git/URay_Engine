#pragma once

namespace URay::Render
{

class RenderScene;
class FogObject;
class AmbientLightObject;
class DirectionalLightObject;

class RenderEnvironment
{
    friend class RenderScene;

public:
    FogObject* GetFog() const { return fog; }
    AmbientLightObject* GetAmbientLight() const { return ambientLight; }
    DirectionalLightObject* GetDirectionalLight() const { return directionalLight; }

private:
    void SetFog(FogObject* fog) { this->fog = fog; }
    void SetAmbientLight(AmbientLightObject* ambientLight) { this->ambientLight = ambientLight; }
    void SetDirectionalLight(DirectionalLightObject* directionalLight) { this->directionalLight = directionalLight; }

private:
    FogObject* fog = nullptr;
    AmbientLightObject* ambientLight = nullptr;
    DirectionalLightObject* directionalLight = nullptr;
};

} // namespace URay::Render
