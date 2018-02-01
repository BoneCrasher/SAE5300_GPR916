#include "Platform/DirectX11/DirectX11Mesh.h"

#include <functional>
#include <fstream>
#include <iterator>

namespace SAE {
  namespace DirectX11 {
    using namespace SAE::Engine;

    std::shared_ptr<DirectX11Mesh>
      DirectX11Mesh::loadFromFile(
        std::shared_ptr<DirectX11ResourceManager>      &resourceManager,
        std::string                               const&filename)
    {
      // RAII
      std::shared_ptr<DirectX11Mesh> pMesh= std::shared_ptr<DirectX11Mesh>(new DirectX11Mesh());

      VertexBuffer_t& underlyingVertexBuffer = pMesh->vertexBuffer();
      underlyingVertexBuffer =
      {
        {{-0.5f, -0.5f, +1.0f, 1.0f}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{+0.5f, -0.5f, +1.0f, 1.0f}, {0.0f, 0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{+0.0f,  0.5f, +1.0f, 1.0f}, {0.0f, 0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
      };

      IndexBuffer_t&  underlyingIndexBuffer = pMesh->indexBuffer();
      underlyingIndexBuffer =
      {
        0, 1, 2
      };

      D3D11_BUFFER_DESC vertexBufferDescription ={};
      vertexBufferDescription.ByteWidth           = underlyingVertexBuffer.size() * sizeof(Vertex_t);
      vertexBufferDescription.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
      vertexBufferDescription.Usage               = D3D11_USAGE_DEFAULT;
      vertexBufferDescription.MiscFlags           = 0;
      vertexBufferDescription.CPUAccessFlags      = 0;
      vertexBufferDescription.StructureByteStride = 0;

      D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData={};
      vertexBufferSubresourceData.pSysMem          = underlyingVertexBuffer.data();
      vertexBufferSubresourceData.SysMemPitch      = 0; // Only for textures
      vertexBufferSubresourceData.SysMemSlicePitch = 0; // Only for texture lists

      uint64_t vertexBufferHandle
        = resourceManager->create<ID3D11Buffer>(vertexBufferDescription, vertexBufferSubresourceData);

      D3D11_BUFFER_DESC indexBufferDescription ={};
      indexBufferDescription.ByteWidth           = underlyingIndexBuffer.size() * sizeof(Index_t);
      indexBufferDescription.BindFlags           = D3D11_BIND_INDEX_BUFFER;
      indexBufferDescription.Usage               = D3D11_USAGE_DEFAULT;
      indexBufferDescription.MiscFlags           = 0;
      indexBufferDescription.CPUAccessFlags      = 0;
      indexBufferDescription.StructureByteStride = 0;

      D3D11_SUBRESOURCE_DATA indexBufferSubresourceData={};
      indexBufferSubresourceData.pSysMem          = underlyingIndexBuffer.data();
      indexBufferSubresourceData.SysMemPitch      = 0; // Only for textures
      indexBufferSubresourceData.SysMemSlicePitch = 0; // Only for texture lists

      uint64_t indexBufferHandle
        = resourceManager->create<ID3D11Buffer>(indexBufferDescription, indexBufferSubresourceData);

      underlyingVertexBuffer.clear();
      underlyingVertexBuffer.resize(0);
      underlyingIndexBuffer.clear();
      underlyingIndexBuffer.resize(0);

      // To be moved into shader class!
      std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements={};
      inputElements.resize(3);

      inputElements[0].SemanticName         = "POSITION";
      inputElements[0].SemanticIndex        = 0;
      inputElements[0].Format               = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
      inputElements[0].AlignedByteOffset    = 0;
      inputElements[0].InputSlot            = 0;
      inputElements[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
      inputElements[0].InstanceDataStepRate = 0;

      inputElements[1].SemanticName         = "NORMAL";
      inputElements[1].SemanticIndex        = 0;
      inputElements[1].Format               = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
      inputElements[1].AlignedByteOffset    = sizeof(XMVECTOR);
      inputElements[1].InputSlot            = 0;
      inputElements[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
      inputElements[1].InstanceDataStepRate = 0;

      inputElements[2].SemanticName         = "COLOR";
      inputElements[2].SemanticIndex        = 0;
      inputElements[2].Format               = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
      inputElements[2].AlignedByteOffset    = 2 * sizeof(XMVECTOR);
      inputElements[2].InputSlot            = 0;
      inputElements[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
      inputElements[2].InstanceDataStepRate = 0;

      std::function<bool(std::string const&, std::vector<uint8_t>&)>
        loadShaderFn = nullptr;

      loadShaderFn
        = [] (std::string const&filename, std::vector<uint8_t>&pInOutData) -> bool
      {
        std::ifstream in;
        in.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

        if((in.bad() || in.fail()))
          return false;

        // Get size
        std::streampos size = in.tellg();
        in.seekg(0, std::ios::beg);

        pInOutData.resize(size_t(size));
        in.read(reinterpret_cast<char*>(&pInOutData.front()), size);

        return true;
      };

      std::vector<uint8_t> 
        vertexShaderByteCode,
        pixelShaderByteCode;

      if(!loadShaderFn("standard_vertex_shader.cso", vertexShaderByteCode)) {
        // TODO: ERROR
      }

      if(!loadShaderFn("standard_fragment_shader.cso", pixelShaderByteCode)) {
        // TODO: ERROR
      }

      DirectX11ShaderBuffer
        vertexShaderBuffer ={ nullptr, 0 };
      vertexShaderBuffer.pData = vertexShaderByteCode.data();
      vertexShaderBuffer.size  = vertexShaderByteCode.size();

      DirectX11ShaderBuffer
        pixelShaderBuffer ={ nullptr, 0 };
      pixelShaderBuffer.pData = pixelShaderByteCode.data();
      pixelShaderBuffer.size  = pixelShaderByteCode.size();

      uint64_t inputLayoutHandle
        = resourceManager->create<ID3D11InputLayout>(inputElements, vertexShaderBuffer);

      uint64_t vertexShaderHandle
        = resourceManager->create<ID3D11VertexShader>(vertexShaderBuffer);

      uint64_t pixelShaderHandle
        = resourceManager->create<ID3D11PixelShader>(pixelShaderBuffer);
      
      pMesh->setVertexBuffer(vertexBufferHandle);
      pMesh->setIndexBuffer(indexBufferHandle);
      pMesh->setVertexShader(vertexShaderHandle);
      pMesh->setPixelShader(pixelShaderHandle);
      pMesh->setInputLayout(inputLayoutHandle);

      return pMesh;
    }

  }
}