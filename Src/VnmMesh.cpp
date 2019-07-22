// VnmMesh.cpp

#include "VnmMesh.h"
#include <cassert>
#include <fstream>

namespace Vnm
{
    void VnmMesh::CreateFromFile(const char* filename)
    {
        std::fstream meshFile(filename, std::ios::in | std::ios::binary);
        if (!meshFile.is_open())
        {
            assert(!"Unable to open file");
            return;
        }

        MeshHeader header;
        meshFile.read(reinterpret_cast<char*>(&header), sizeof(header));
        const uint32_t vnmMesh4cc = ('m' << 24) | ('m' << 16) | ('n' << 8) | ('v');
        assert(header.mMagic == vnmMesh4cc);

        mSubmeshes.resize(header.mNumSubmeshes);
        meshFile.read(reinterpret_cast<char*>(mSubmeshes.data()), mSubmeshes.size() * sizeof(mSubmeshes[0]));

        mVertices.resize(header.mNumTotalVertices);
        meshFile.read(reinterpret_cast<char*>(mVertices.data()), mVertices.size() * sizeof(mVertices[0]));

        mIndices.resize(header.mNumTotalIndices);
        meshFile.read(reinterpret_cast<char*>(mIndices.data()), mIndices.size() * sizeof(mIndices[0]));
        meshFile.close();
    }

    void VnmMesh::Destroy()
    {
        mIndices.clear();
        mVertices.clear();
        mSubmeshes.clear();
    }
}
