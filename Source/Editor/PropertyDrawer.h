#pragma once

namespace URay
{

struct Property;

class PropertyDrawer
{
public:
    static void Draw(Property& prop, void* addr);

private:
    static void DrawBool(Property& prop, void* addr);
    static void DrawVector3(Property& prop, void* addr);
    static void DrawString(Property& prop, void* addr);
    static void DrawMesh(Property& prop, void* addr);
};

} // namespace URay
