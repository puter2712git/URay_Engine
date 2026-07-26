#include "PropertyDrawer.h"

#include "Engine/Engine.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Property/Property.h"

#include "Core/Math/Vector3.h"

#include <imgui/imgui.h>

namespace URay
{

void PropertyDrawer::Draw(Property& prop)
{
    switch (prop.type)
    {
    case PropertyType::Bool:
        DrawBool(prop);
        break;
    case PropertyType::Vector3:
        DrawVector3(prop);
        break;
    case PropertyType::Mesh:
        DrawMesh(prop);
        break;
    }
}

void PropertyDrawer::DrawBool(Property& prop)
{
    bool* data = reinterpret_cast<bool*>(prop.dataPtr);
    ImGui::Checkbox(prop.name.c_str(), data);
}

void PropertyDrawer::DrawVector3(Property& prop)
{
    Vector3* data = reinterpret_cast<Vector3*>(prop.dataPtr);
    ImGui::DragFloat3(prop.name.c_str(), &data->x, 0.1f);
}

void PropertyDrawer::DrawMesh(Property& prop)
{
    Mesh** currMesh = reinterpret_cast<Mesh**>(prop.dataPtr);

    MeshManager* meshManager = gEngine->GetMeshManager();
    const auto& meshes = meshManager->GetMeshes();

    size_t meshCount = meshes.size();
    std::vector<std::string> meshNames(meshCount);
    std::vector<Mesh*> meshArr(meshCount);

    size_t selectedIndex = -1;
    int i = 0;
    for (auto [meshName, mesh] : meshes)
    {
        meshNames[i] = meshName;
        meshArr[i] = mesh;

        if (mesh == *currMesh)
        {
            selectedIndex = i;
        }

        ++i;
    }

    if (ImGui::BeginCombo("Mesh", meshNames[selectedIndex].c_str()))
    {
        for (int index = 0; index < meshCount; ++index)
        {
            const bool isSelected = selectedIndex == index;

            if (ImGui::Selectable(meshNames[index].c_str(), isSelected))
            {
                selectedIndex = index;
                *currMesh = meshArr[index];
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
}

} // namespace URay
