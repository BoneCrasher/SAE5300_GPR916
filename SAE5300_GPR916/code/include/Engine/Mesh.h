#ifndef __SAE5300_GPR916_MESH_H__
#define __SAE5300_GPR916_MESH_H__

#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/cimport.h>

namespace SAE {
  namespace Engine {

    template <typename TVector>
    static TVector aiVector3DToTVector(const aiVector3D& in, const float w = 0.0f, const float factor = 1.0f)
    {
      return TVector({ in.x * factor, in.y * factor, in.z * factor, w });
    }

    template <typename TVector>
    struct Vertex {
      TVector
        position,
        normal,
        tangent,
        uv,
        color;
    };
    
    template <typename TVector>
    class Mesh {
    public:
      using Vertex_t       = Vertex<TVector>;
      using VertexBuffer_t = std::vector<Vertex_t>;
      using Index_t        = uint32_t;
      using IndexBuffer_t  = std::vector<Index_t>;
      
      VertexBuffer_t const& vertexBuffer() const { return m_vertexBuffer; }
      IndexBuffer_t  const& indexBuffer()  const { return m_indexBuffer;  }

      static bool LoadMeshAssimp(
        const char     *filename,
        VertexBuffer_t &outVB,
        uint64_t       &outVCount,
        IndexBuffer_t  &outIB,
        uint64_t       &outICount);

    protected:
      VertexBuffer_t& vertexBuffer() { return m_vertexBuffer; }
      IndexBuffer_t&  indexBuffer()  { return m_indexBuffer;  }
      
    private:
      VertexBuffer_t m_vertexBuffer;
      IndexBuffer_t  m_indexBuffer;
    };

    template <typename TVector>
    bool 
      Mesh<TVector>::LoadMeshAssimp(
      const char     *filename,
      VertexBuffer_t &outVB,
      uint64_t       &outVCount,
      IndexBuffer_t  &outIB,
      uint64_t       &outICount)
    {
      Assimp::Importer importer;

      unsigned int flags
        = aiProcess_GenUVCoords
        | aiProcess_GenNormals
        | aiProcess_CalcTangentSpace
        | aiProcess_Triangulate
        // | aiProcess_FixInfacingNormals
        | aiProcess_FlipWindingOrder;

      const aiScene*pScene = importer.ReadFile(filename, flags);

      if( !pScene )
      {
        const std::string err = importer.GetErrorString();
        std::cout << "Mimimimi" << std::endl;
        return false;
      }

      //pScene = aiApplyPostProcessing(pScene, flags);

      if( !pScene->HasMeshes() )
        return false;

      // Prescan all meshes to get the total size!
      unsigned int totalVertexCount = 0;
      unsigned int totalFaceCount   = 0;
      unsigned int totalIndexCount  = 0;
      for( unsigned int k=0; k < pScene->mNumMeshes; ++k )
      {
        aiMesh *pMesh = pScene->mMeshes[k];

        totalVertexCount += pMesh->mNumVertices;
        totalFaceCount   += pMesh->mNumFaces;
        totalIndexCount  += pMesh->mNumFaces * 3;

        for( unsigned int j=0; j < pMesh->mNumFaces; ++j )
        {
          aiFace *pFace = pMesh->mFaces + j;
        }

        // break;
      }

      VertexBuffer_t vertices{};
      uint64_t       vertexCount = 0;
      IndexBuffer_t  indices{};
      uint64_t       indexCount = 0;

      for( unsigned int k=0; k < pScene->mNumMeshes; ++k )
      {
        aiMesh *pMesh = pScene->mMeshes[k];
        vertexCount  = pMesh->mNumVertices;

        vertices.resize(vertexCount);
        for( unsigned int v=0; v < pMesh->mNumVertices; ++v )
        {
          vertices[v].position = aiVector3DToTVector<TVector>(pMesh->mVertices[v], 1.0f);
          vertices[v].normal   = aiVector3DToTVector<TVector>(pMesh->mNormals[v]);
          vertices[v].tangent  = aiVector3DToTVector<TVector>(pMesh->mTangents[v]);
          vertices[v].uv       = aiVector3DToTVector<TVector>((pMesh->mTextureCoords[0][v]));
        }

        indexCount = pMesh->mNumFaces * 3;
        indices.resize(indexCount);
        for( unsigned int j=0; j < pMesh->mNumFaces; ++j )
        {
          aiFace *pFace = pMesh->mFaces + j;
          for( unsigned int i=0; i < pFace->mNumIndices; ++i )
          {
            indices[3*j + i] = *(pFace->mIndices + i);
          }
        }			
      }

      outVB     = vertices;
      outIB     = indices;
      outVCount = vertexCount;
      outICount = indexCount;

      // ai-Resources destroyed by the importer destructor.
      return true;
    }

  }
}

#endif