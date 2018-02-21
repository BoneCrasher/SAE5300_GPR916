#ifndef __SAE5300_GPR916_TEXTURE_H__
#define __SAE5300_GPR916_TEXTURE_H__

#include <string>
#include <vector>

namespace SAE {
  namespace Texture {

    typedef char Byte;

    struct Texture2DDescriptor
    {
      std::vector<std::vector<Byte>> inData;
      unsigned int  inByteSize;
      unsigned int  width;
      unsigned int  height;
      unsigned int  depth;
      unsigned int  channels;
      unsigned int  mipLevels;

      inline void freeData()
      {
      }
    };

    bool SAELoadTextureArrayFromFiles(
      const std::vector<std::string> &filenames, 
      Texture2DDescriptor            &outImage);

    bool SAELoadTextureFromFile(
      const char          *filename,
      Texture2DDescriptor &outImage);

  }
}

#endif