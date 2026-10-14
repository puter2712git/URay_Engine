#pragma once

#include <Core/UUID.h>
#include <Engine/Object/Object.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace URay
{

class Component;
class TransformComponent;
class Scene;

enum class ReparentMode
{
    KeepLocal,
    KeepWorld
};

class Unit : public Object
{
    URAY_CLASS(Unit, Object)

public:
    Unit();
    ~Unit() override;

public:
    virtual void Update(float deltaTime);

    virtual YAML::Node Serialize() const override;
    virtual void Deserialize(const YAML::Node& node) override;

    Component* AddComponent(std::unique_ptr<Component> component);

    template <typename T>
    T* GetComponent() const
    {
        for (const auto& comp : components)
        {
            if (T* target = Cast<T>(comp.get()))
                return target;
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Component>>& GetComponents() const { return components; }

    TransformComponent* GetTransform() const { return transform; }

    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }

    const UUID& GetPersistentId() const { return persistentId; }
    void SetPersistentId(const UUID& id) { persistentId = id; }

    Unit* GetParent() const { return parent; }
    bool SetParent(Unit* unit, ReparentMode mode = ReparentMode::KeepLocal);

    const std::vector<Unit*>& GetChildren() const { return children; }

    Scene* GetOwner() const { return scene; }
    void SetOwner(Scene* scene) { this->scene = scene; }

private:
    static TransformComponent* FindClosestTransform(Unit* unit);

    void CollectTransformRoots(std::vector<TransformComponent*>& results, bool hasTransformAncestor) const;

    void MarkTransformHierarchyDirty();

private:
    std::string name;

    UUID persistentId = UUID::Generate();

    Unit* parent = nullptr;
    std::vector<Unit*> children;

    std::vector<std::unique_ptr<Component>> components;
    TransformComponent* transform = nullptr;

    Scene* scene = nullptr;
};

} // namespace URay
