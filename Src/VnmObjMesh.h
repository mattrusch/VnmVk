// VnmObjMesh.h

#pragma once

#include <stdint.h>

namespace Vnm
{
    class ObjMesh
    {
    public:
        void CreateFromFile(const char* filename);
        void Destroy();

    protected:
        size_t    mNumVertices = 0;
        uint8_t*  mVertexData = nullptr;
        size_t    mNumIndices  = 0;
        uint16_t* mIndexData = nullptr;
    };
}
