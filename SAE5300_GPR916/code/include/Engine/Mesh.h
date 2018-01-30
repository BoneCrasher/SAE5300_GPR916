#ifndef __SAE5300_GPR916_MESH_H__
#define __SAE5300_GPR916_MESH_H__

#include <vector>
#include <memory>
#include <string>

namespace SAE {
  namespace Engine {

    template <typename TVector>
    struct Vertex {
      TVector
        position,
        normal,
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
      
    protected:
      VertexBuffer_t& vertexBuffer() { return m_vertexBuffer; }
      IndexBuffer_t&  indexBuffer()  { return m_indexBuffer;  }

    private:
      VertexBuffer_t m_vertexBuffer;
      IndexBuffer_t  m_indexBuffer;
    };

  }
}

#endif