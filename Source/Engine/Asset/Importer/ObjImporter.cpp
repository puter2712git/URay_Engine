#include "ObjImporter.h"

#include "Core/File/FileIO.h"

#include <fstream>
#include <sstream>

namespace URay
{

MeshAsset* ObjImporter::Import(const std::string& filePath)
{
    if (!FileIO::Exists(filePath))
        return nullptr;

    Reset();

    std::fstream file(filePath);
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v")
        {
            Vector3 p;
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (type == "vt")
        {
            Vector2 uv;
            ss >> uv.x >> uv.y;
            uvs.push_back(uv);
        }
        else if (type == "vn")
        {
            Vector3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
    }
}

void ObjImporter::Reset()
{
    positions.clear();
    uvs.clear();
    normals.clear();
}

} // namespace URay
