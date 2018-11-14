// VnmFile.h

#pragma once

#include <cstdint>

namespace Vnm
{
    class FileResource
    {
    public:
        static FileResource* LoadFileResource(const char* mpFilename);
        static void DestroyFileResource(FileResource* fileResource);

        size_t GetSize() const              { return mSize; }
        const std::uint8_t* GetData() const { return mData; }

    private:
        FileResource() = default;
        ~FileResource() = default;

        size_t        mSize = 0;
        std::uint8_t* mData = nullptr;
    };
}
