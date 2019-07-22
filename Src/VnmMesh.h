// VnmMesh.h

#pragma once

#include <vector>

namespace Vnm
{
    class MeshHeader
    {
    public:
        uint32_t mMagic;
        uint32_t mNumSubmeshes;
        uint32_t mNumTotalVertices;
        uint32_t mNumTotalIndices;
    };

    const uint32_t meshNameSize = 64;
    class SubmeshDesc
    {
    public:
        char	 mName[meshNameSize];
        uint32_t mNumVertices;
        uint32_t mNumIndices;
    };

    class VnmMesh
    {
    public:

        class Vector4
        {
        public:
            float v[4];
        };

        class Vector3
        {
        public:
            float v[3];
        };

        class Vector2
        {
        public:
            float v[2];
        };

        class Vertex
        {
        public:
            Vector3 mPosition;
            Vector3 mNormal;
            Vector4 mTangent;	// w component stores handedness
            Vector2 mUv;
        };

        void CreateFromFile(const char* filename);
        void Destroy();

        size_t GetNumVertices() const { return mVertices.size(); }
        size_t GetVertexDataSize() const { return GetNumVertices() * sizeof(mVertices[0]); }
        const Vertex* GetVertexData() const { return mVertices.data(); }

        size_t GetNumIndices() const { return mIndices.size(); }
        size_t GetIndexDataSize() const { return GetNumIndices() * sizeof(mIndices[0]); }
        const uint32_t* GetIndexData() const { return mIndices.data(); }

        size_t GetNumSubmeshes() const { return mSubmeshes.size(); }
        const SubmeshDesc* GetSubmeshData() const { return mSubmeshes.data(); }

    private:
        std::vector<SubmeshDesc> mSubmeshes;
        std::vector<Vertex>      mVertices;
        std::vector<uint32_t>    mIndices;
    };
}
