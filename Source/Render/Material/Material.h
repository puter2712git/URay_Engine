#pragma once

namespace URay
{

class Shader;

class Material
{
public:
    Shader* GetShader() const
    {
        return shader;
    }
    void SetShader(Shader* inShader)
    {
        shader = inShader;
    }

private:
    Shader* shader = nullptr;
};

} // namespace URay
