// VnmTgaImage.cpp

#include "VnmTgaImage.h"
#include <cassert>

namespace Vnm
{
    class TgaFileHeader
    {
    public:
        #pragma pack(push, 1)
        uint8_t	 mIdLength;
        uint8_t	 mColorMapType;
        uint8_t	 mImageType;
        uint16_t mColorMapStart;
        uint16_t mColorMapLength;
        uint8_t	 mColorMapEntrySize;
        uint16_t mXOrigin;
        uint16_t mYOrigin;
        uint16_t mImageWidth;
        uint16_t mImageHeight;
        uint8_t	 mBpp;
        uint8_t	 mImageDesc;
        #pragma pack(pop)
    };

    enum ImageType
    {
        kNone,
        kIndexed,
        kTrueColor,
        kGreyScale,
        kRle
    };

    static bool VerifyTgaHeader(const TgaFileHeader* header)
    {
        if (header->mImageType == kTrueColor)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void TgaImage::ParseData(const uint8_t* pTgaData)
    {
        const TgaFileHeader* pHeader = (const TgaFileHeader*)pTgaData;

        if (!VerifyTgaHeader(pHeader))
        {
            assert(!"Invalid tga header");
            return;
        }

        mWidth = pHeader->mImageWidth;
        mHeight = pHeader->mImageHeight;
        mBytesPerPixel = pHeader->mBpp / 8;
        mImageSize = mWidth * mHeight * mBytesPerPixel;
        mpImageData = pTgaData + sizeof(TgaFileHeader);
    }
}
