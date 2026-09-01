#pragma once

namespace URay
{

struct Property;

class PropertyDrawer
{
public:
    static void Draw(Property& prop, void* addr);

private:
    static bool DrawBool(Property& prop, void* addr);
    static bool DrawFloat(Property& prop, void* addr);
    static bool DrawVector3(Property& prop, void* addr);
    static bool DrawVector4(Property& prop, void* addr);
    static bool DrawString(Property& prop, void* addr);
    static bool DrawMesh(Property& prop, void* addr);
    static bool DrawTexture(Property& prop, void* addr);
    static bool DrawMaterial(Property& prop, void* addr);
};

} // namespace URay
