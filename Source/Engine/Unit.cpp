#include "Unit.h"

#include "Engine/Component/Component.h"
#include "Engine/Component/RenderComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Scene.h"

#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/RenderScene.h"

namespace URay
{

URAY_REGISTER_CLASS(Unit)

Unit::~Unit()
{
    for (Component* comp : components)
    {
        if (comp)
        {
            delete comp;
            comp = nullptr;
        }
    }

    components.clear();
}

void Unit::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::String,
                                 .name = "Name",
                                 .offset = offsetof(Unit, name),
                                 .size = sizeof(std::string) });
}

void Unit::Update(float deltaTime)
{
    for (auto comp : components)
        comp->Update(deltaTime);
}

YAML::Node Unit::Serialize()
{
    YAML::Node node;

    for (auto comp : components)
        node[comp->GetClass()->GetName()] = comp->Serialize();

    return node;
}

void Unit::Deserialize(const YAML::Node& node)
{
}

void Unit::RegisterTransformUpdateCallback(const std::function<void()>& callback)
{
    transformUpdateCallbacks.push_back(callback);
}

void Unit::InvokeCallbacks()
{
    for (const auto& callback : transformUpdateCallbacks)
    {
        callback();
    }
}

Component* Unit::AddComponent(Component* comp)
{
    if (!comp)
        return nullptr;

    if (TransformComponent* transformComp = dynamic_cast<TransformComponent*>(comp))
    {
        transform = transformComp;
    }

    comp->SetOwner(this);
    components.push_back(comp);
    comp->OnAttached();

    if (scene)
    {
        if (RenderComponent* renderComp = dynamic_cast<RenderComponent*>(comp))
        {
            std::unique_ptr<Render::RenderObject> robj(renderComp->CreateRenderObject());
            if (robj && scene->GetRenderScene())
            {
                scene->GetRenderScene()->Add(std::move(robj));
            }
        }
    }

    return comp;
}

} // namespace URay
