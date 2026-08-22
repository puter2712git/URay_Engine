#pragma once

#include <memory>
#include <vector>

namespace URay::RHI
{

class RenderInstance;

class RenderScene
{
public:
    void Add(std::unique_ptr<RenderInstance> instance);

private:
    std::vector<std::unique_ptr<RenderInstance>> instances;
};

} // namespace URay::RHI
