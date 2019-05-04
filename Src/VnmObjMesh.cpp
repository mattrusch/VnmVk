// ObjMesh.cpp

#include "VnmObjMesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <vector>

namespace Vnm
{
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

    static ObjMesh::Vec3 ParseVector(std::istringstream& srcStream)
    {
        float x, y, z;

        if (!(srcStream >> x >> y >> z))
        {
            assert(!"Error parsing vector");
        }

        return ObjMesh::Vec3(x, y, z);
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
        std::vector<ObjMesh::Vec3> mPositions;
        std::vector<ObjMesh::Vec3> mNormals;
        std::vector<ObjMesh::Vec3> mTexcoords;
        std::vector<Face> mFaces;
    };

    void ObjMesh::CreateFromFile(const char* filename)
    {
        std::ifstream objFile(filename);
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
        for (auto& face : scratchMesh.mFaces)
        {
            for (uint32_t i = 0; i < face.mNumIndices; ++i)
            {
                Vertex vertex;

                int positionIndex = face.mFaceIndices[i].mAttribIndices[VertexAttrib::Position];
                vertex.mPosition = scratchMesh.mPositions[positionIndex];

                int texcoordIndex = face.mFaceIndices[i].mAttribIndices[VertexAttrib::Texcoord];
                const Vec3 defaultTexcoord(0.0f, 0.0f, 0.0f);
                vertex.mTexcoord = texcoordIndex >= 0 ? scratchMesh.mTexcoords[texcoordIndex] : defaultTexcoord;

                int normalIndex = face.mFaceIndices[i].mAttribIndices[VertexAttrib::Normal];
                const Vec3 defaultNormal(0.0f, 1.0f, 0.0f);
                vertex.mNormal = normalIndex >= 0 ? scratchMesh.mNormals[normalIndex] : defaultNormal;

                auto it = std::find(mVertices.begin(), mVertices.end(), vertex);
                if(it != mVertices.end())
                {
                    mIndices.emplace_back(static_cast<uint32_t>(it - mVertices.begin()));
                }
                else
                {
                    mIndices.emplace_back(static_cast<uint32_t>(mVertices.size()));
                    mVertices.emplace_back(vertex);
                }
            }
        }

        // Convert face indices to triangle indices. Assumes file is either all triangles or all faces
        if (scratchMesh.mFaces[0].mNumIndices != 3)
        {
            std::vector<uint32_t> scratchIndices;
            size_t numFaces = mIndices.size();
            for (int i = 0; i < numFaces; i += 4)
            {
                scratchIndices.emplace_back(mIndices[i+0]);
                scratchIndices.emplace_back(mIndices[i+1]);
                scratchIndices.emplace_back(mIndices[i+2]);

                scratchIndices.emplace_back(mIndices[i+0]);
                scratchIndices.emplace_back(mIndices[i+2]);
                scratchIndices.emplace_back(mIndices[i+3]);
            }

            mIndices.swap(scratchIndices);
        }
    }

    void ObjMesh::Destroy()
    {
        mVertices.clear();
        mIndices.clear();
    }
}
