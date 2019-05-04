// VnmObjMesh.h

#pragma once

#include <stdint.h>
#include <vector>

namespace Vnm
{
    class ObjMesh
    {
    public:
        class Vec3
        {
        public:
            Vec3() = default;
            Vec3(float x, float y, float z) : mVec{ x, y, z } {}

            bool operator ==(const Vec3& rhs) const
            {
                return (mVec[0] == rhs.mVec[0]) && (mVec[1] == rhs.mVec[1]) && (mVec[2] == rhs.mVec[2]);
            }

            float mVec[3];
        };

        class Vertex
        {
        public:
            bool operator ==(const Vertex& rhs) const
            {
                return (mPosition == rhs.mPosition) && (mTexcoord == rhs.mTexcoord) && (mNormal == rhs.mNormal);
            }

            Vec3 mPosition;
            Vec3 mTexcoord;
            Vec3 mNormal;
        };

        void CreateFromFile(const char* filename);
        void Destroy();

        size_t GetNumVertices() const { return mVertices.size(); }
        size_t GetVertexDataSize() const { return GetNumVertices() * sizeof(mVertices[0]); }
        const Vertex* GetVertexData() const { return mVertices.data(); }

        size_t GetNumIndices() const { return mIndices.size(); }
        size_t GetIndexDataSize() const { return GetNumIndices() * sizeof(mIndices[0]); }
        const uint32_t* GetIndexData() const { return mIndices.data(); }

    protected:
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
    };
}
