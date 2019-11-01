// VnmTgaImage.h

#pragma once

#include <cstdint>

namespace Vnm
{
    class TgaImage
    {
    public:
        TgaImage()
            : mWidth(0)
            , mHeight(0)
            , mBytesPerPixel(0)
            , mImageSize(0)
            , mpImageData(0)
        {}
        ~TgaImage() {}

        void ParseData(const uint8_t* tgaData);

        uint32_t GetWidth() const { return mWidth; }
        uint32_t GetHeight() const { return mHeight; }
        uint32_t GetBytesPerPixel() const { return mBytesPerPixel; }
        uint32_t GetSize() const { return mImageSize; }
        const uint8_t* GetImageData() const { return mpImageData; }

    protected:
        uint32_t       mWidth;
        uint32_t       mHeight;
        uint32_t       mBytesPerPixel;
        uint32_t       mImageSize;
        const uint8_t* mpImageData;
    };

}
