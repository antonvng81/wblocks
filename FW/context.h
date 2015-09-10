#ifndef CONTEXT_H
#define CONTEXT_H

#include "FW/tools.h"

class TypeStruct;
class TypeScene;

class TypeContext : public TypeVariant
{
public:

    TypeContext( TypeStruct& records, TypeScene& scene, TypeStruct* root = 0, TypeVariant* Parent = 0 );
    ~TypeContext() override;

    M_REFERENCE         ( Struct,  TypeStruct )
    M_REFERENCE         ( Scene,   TypeScene )
    M_REFERENCE         ( Root,    TypeStruct )

};

#endif // CONTEXT_H
