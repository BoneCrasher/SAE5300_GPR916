#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Engine/Texture.h"

namespace SAE {
  namespace Texture {

    bool SAELoadTextureArrayFromFiles(
      const std::vector<std::string> &filenames, 
      Texture2DDescriptor            &outImage)
    {
      try
      {
        unsigned int previousW = 0;
        unsigned int previousH = 0;

        outImage.depth = filenames.size();
        outImage.inData.resize(outImage.depth);

        for(unsigned int k=0; k < outImage.depth; ++k)
        {
          int w = 0, h = 0, c = 0;
          unsigned char* stbuc = stbi_load(filenames[k].c_str(), &w, &h, &c, 4);

          outImage.inByteSize = w * h * 4 * sizeof(Byte);
          outImage.inData[k].resize(outImage.inByteSize);
          outImage.width      = w;
          outImage.height     = h;
          outImage.channels   = c;

          bool throwDimensionMismatch = false;
          if(k > 0
             && !(outImage.width == previousW
                  && outImage.height == previousH))
          {
            throwDimensionMismatch = true;
          }
          else
          {
            previousW = w;
            previousH = h;
            memcpy(&outImage.inData[k][0], stbuc, outImage.inByteSize);
          }

          stbi_image_free(stbuc);

          if(throwDimensionMismatch)
            throw std::exception("Dimension mismatch loading texture array");

        }

        return true;
      } catch(...)
      {
        return false;
      }
    }

    bool SAELoadTextureFromFile(const char* filename, Texture2DDescriptor& outImage)
    {
      try
      {
        int w = 0, h = 0, c = 0;
        unsigned char* stbuc = stbi_load(filename, &w, &h, &c, 4);

        outImage.inData.resize(1);

        outImage.inByteSize = w * h * 4 * sizeof(Byte);
        outImage.inData[0].resize(outImage.inByteSize);
        outImage.width      = w;
        outImage.height     = h;
        outImage.depth      = 1;
        outImage.channels   = c;
        memcpy(&outImage.inData[0][0], stbuc, outImage.inByteSize);

        stbi_image_free(stbuc);

        return true;
      } catch(...)
      {
        return false;
      }
    }


  }
}