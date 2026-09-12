#pragma once

#include "Engine/Object/Object.h"
#include "Engine/Ray/EventRay.h"

#include <functional>
#include <string>
#include <vector>

namespace URay
{

class Component;
class TransformComponent;
class Scene;

class Unit : public Object
{
    URAY_CLASS(Unit, Object)

public:
    Unit() = default;
    virtual ~Unit();

public:
    virtual void Update(float deltaTime);

    virtual YAML::Node Serialize() const override;
    virtual void Deserialize(const YAML::Node& node) override;

    Component* AddComponent(Component* comp);

    template <typename T>
    T* GetComponent() const
    {
        for (Component* comp : components)
        {
            if (T* target = Cast<T>(comp))
                return target;
        }
        return nullptr;
    }

    const std::vector<Component*>& GetComponents() const { return components; }

    TransformComponent* GetTransform() const { return transform; }

    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }

    Unit* GetParent() const { return parent; }
    bool SetParent(Unit* unit);

    const std::vector<Unit*>& GetChildren() const { return children; }

    Scene* GetOwner() const { return scene; }
    void SetOwner(Scene* scene) { this->scene = scene; }

    const EventRay<Component*>& GetTransformUpdateRay() const { return transformUpdateRay; }

private:
    std::string name;

    Unit* parent = nullptr;
    std::vector<Unit*> children;

    std::vector<Component*> components;
    TransformComponent* transform = nullptr;

    Scene* scene = nullptr;

    EventRay<Component*> transformUpdateRay;
};

} // namespace URay
