// ObjMesh.cpp

#include "VnmObjMesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <vector>

namespace Vnm
{
    class Vec3
    {
    public:
        Vec3() = default;
        Vec3(float x, float y, float z)
            : mVec{x, y, z}
        {}

        bool operator ==(const Vec3& rhs) const
        {
            return (mVec[0] == rhs.mVec[0]) &&
                (mVec[1] == rhs.mVec[1]) &&
                (mVec[2] == rhs.mVec[2]);
        }

        float mVec[3];
    };

    class Vertex
    {
    public:
        bool operator ==(const Vertex& rhs) const
        {
            return (mPosition == rhs.mPosition) &&
                (mTexcoord == rhs.mTexcoord) &&
                (mNormal == rhs.mNormal);
        }

        Vec3 mPosition;
        Vec3 mTexcoord;
        Vec3 mNormal;
    };

    enum VertexAttrib
    {
        Position,
        Texcoord,
        Normal,
        NumAttribs
    };

    class VertexIndex
    {
    public:
        int mAttribIndices[VertexAttrib::NumAttribs] = { -1, -1, -1 };
    };

    class Face
    {
    public:
        const static size_t maxIndices = 4;

        uint32_t    mNumIndices = 0;
        VertexIndex mFaceIndices[maxIndices];
    };

    static Vec3 ParseVector(std::istringstream& srcStream)
    {
        float x, y, z;

        if (!(srcStream >> x >> y >> z))
        {
            assert(!"Error parsing vector");
        }

        return Vec3(x, y, z);
    }

    static Face ParseFace(std::istringstream& srcStream)
    {
        Face result;

        // Obj face format is as follows: f v/vt/vn v/vt/vn v/vt/vn v/vt/vn, vt and vn are optional
        while (!srcStream.eof())
        {
            VertexIndex& vertexIndex = result.mFaceIndices[result.mNumIndices++];
            int index;
            srcStream >> index;
            vertexIndex.mAttribIndices[VertexAttrib::Position] = index - 1;
            if (srcStream.peek() == '/')
            {
                srcStream.get();
                if (srcStream.peek() == '/')
                {
                    srcStream.get();
                    srcStream >> index;
                    vertexIndex.mAttribIndices[VertexAttrib::Normal] = index - 1;
                }
                else
                {
                    srcStream >> index;
                    vertexIndex.mAttribIndices[VertexAttrib::Texcoord] = index - 1;
                    if (srcStream.peek() == '/')
                    {
                        srcStream.get();
                        srcStream >> index;
                        vertexIndex.mAttribIndices[VertexAttrib::Normal] = index - 1;
                    }
                }

            }
            srcStream >> std::ws;
        }

        return result;
    }

    class ScratchMesh
    {
    public:
        std::vector<Vec3> mPositions;
        std::vector<Vec3> mNormals;
        std::vector<Vec3> mTexcoords;
        std::vector<Face> mFaces;
    };

    void ObjMesh::CreateFromFile(const char* filename)
    {
        std::ifstream objFile("filename");
        assert(objFile.good());

        ScratchMesh scratchMesh;

        // Extract geometric data from file
        std::string line;
        while (std::getline(objFile, line))
        {
            std::istringstream stream(line);
            std::string id;
            stream >> id >> std::ws;

            if (id == "v")
            {
                scratchMesh.mPositions.emplace_back(ParseVector(stream));
            }
            else if (id == "vn")
            {
                scratchMesh.mNormals.emplace_back(ParseVector(stream));
            }
            else if (id == "vt")
            {
                scratchMesh.mTexcoords.emplace_back(ParseVector(stream));
            }
            else if (id == "f")
            {
                scratchMesh.mFaces.emplace_back(ParseFace(stream));
            }
        }

        // Build interleaved vertex buffer and matching index buffer
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (auto& face : scratchMesh.mFaces)
        {
            for (uint32_t i = 0; i < face.mNumIndices; ++i)
            {
                Vertex vertex;
                vertex.mPosition = scratchMesh.mPositions[face.mFaceIndices[i].mAttribIndices[VertexAttrib::Position]];
                vertex.mTexcoord = scratchMesh.mTexcoords[face.mFaceIndices[i].mAttribIndices[VertexAttrib::Texcoord]];
                vertex.mNormal = scratchMesh.mNormals[face.mFaceIndices[i].mAttribIndices[VertexAttrib::Normal]];

                auto it = std::find(vertices.begin(), vertices.end(), vertex);
                if(it != vertices.end())
                {
                    indices.emplace_back(static_cast<uint32_t>(it - vertices.begin()));
                }
                else
                {
                    indices.emplace_back(static_cast<uint32_t>(vertices.size()));
                    vertices.emplace_back(vertex);
                }
            }
        }

        // Convert face indices to triangle indices
        if (scratchMesh.mFaces[0].mNumIndices != 3)
        {
            std::vector<uint32_t> scratchIndices;
            size_t numFaces = scratchMesh.mFaces.size();
            for (int i = 0; i < numFaces; i += 4)
            {
                scratchIndices.emplace_back(indices[i+0]);
                scratchIndices.emplace_back(indices[i+1]);
                scratchIndices.emplace_back(indices[i+2]);

                scratchIndices.emplace_back(indices[i+0]);
                scratchIndices.emplace_back(indices[i+2]);
                scratchIndices.emplace_back(indices[i+3]);
            }

            indices.swap(scratchIndices);
        }
    }

    void ObjMesh::Destroy()
    {

    }
}
