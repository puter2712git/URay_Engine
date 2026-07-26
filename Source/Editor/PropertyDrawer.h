#pragma once

namespace URay
{

struct Property;

class PropertyDrawer
{
public:
    static void Draw(Property& prop);

private:
    static void DrawBool(Property& prop);
    static void DrawVector3(Property& prop);
    static void DrawMesh(Property& prop);
};

} // namespace URay
