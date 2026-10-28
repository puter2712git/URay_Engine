#include "FunctionParameter.h"

#include <Core/Type/Variant/Variant.h>

namespace URay
{

bool FunctionParameter::Accepts(const Variant& argument) const
{
    const Type* argumentType = argument.GetType();

    if (!type || !argumentType)
        return false;

    if (type->kind == TypeKind::Class && type->isObjectType)
    {
        if (!argumentType->isObjectType)
            return false;

        if (!IsTypeOf(*argumentType, *type))
            return false;

        if (isConst)
            return false;

        if (argument.IsNullObject())
            return nullable;

        return argument.IsObjectAlive();
    }

    if (type != argumentType)
        return false;

    if (type->kind == TypeKind::Value)
        return !isConst && !nullable;

    if (type->kind == TypeKind::Class && !type->isObjectType)
    {
        if (argument.IsConstPointer() != isConst)
            return false;

        if (argument.IsNullPointer() && !nullable)
            return false;

        return true;
    }

    return false;
}

bool FunctionParameter::Accepts(const FunctionParameter& source) const
{
    if (!type || !source.type)
        return false;

    if (source.nullable && !nullable)
        return false;

    if (isConst != source.isConst)
        return false;

    if (type->isObjectType && source.type->isObjectType)
        return IsTypeOf(*source.type, *type);

    return type == source.type;
}

} // namespace URay
