#pragma once

namespace URay
{

class Object;
class VirtualPath;

class Importer
{
public:
    virtual ~Importer() = default;

public:
    virtual Object* Import(const VirtualPath& filePath) = 0;
};

} // namespace URay
