// VnmFile.cpp

#include "VnmFile.h"
#include <cstdio>
#include <cassert>

namespace Vnm
{
    FileResource* FileResource::LoadFileResource(const char* filename)
    {
        FILE* file;
        errno_t error = fopen_s(&file, filename, "rb");
        assert(!error);

        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        FileResource* pFileResource = new FileResource();
        pFileResource->mSize = fileSize;
        pFileResource->mData = new std::uint8_t[pFileResource->mSize];
        fread(pFileResource->mData, 1, fileSize, file);
        fclose(file);

        return pFileResource;
    }

    void FileResource::DestroyFileResource(FileResource* pFileResource)
    {
        delete[] pFileResource->mData;
        delete pFileResource;
    }
}
