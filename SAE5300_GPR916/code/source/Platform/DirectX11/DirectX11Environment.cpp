#include "Platform/DirectX11/DirectX11Environment.h"

#include <vector>
#include <functional>

#include <locale>
#include <codecvt>

#include "Logging/Logging.h"
#include "Errors/ErrorHandling.h"

namespace SAE {
  namespace DirectX11 {
    using namespace SAE::Log;
    using namespace SAE::Error;

    static std::string toString(const std::wstring &wideString) {
      // the UTF-8 / UTF-16 standard conversion facet
      std::string utf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(wideString.data());
      return utf8;
    }

    static std::string toString(const uint32_t &number) {
      return std::to_string(number);
    }

    DirectX
      ::DirectX()
    {

    }

    GAPICaps DirectX
      ::findAdapters()
    {
      try {
        HRESULT result = S_OK;

        GAPICaps caps ={};

        IDXGIFactoryPtr pFactory = nullptr;

        IDXGIFactory *pFactoryUnmanaged = nullptr;
        result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&pFactoryUnmanaged);
        HandleWINAPIError(result, "Failed to created DXGIFactory");

        pFactory = MakeDirectX11ResourceSharedPointer(pFactoryUnmanaged);

        uint32_t      k                 = 0;
        IDXGIAdapter *pAdapterUnmanaged = nullptr;
        while((result = pFactory->EnumAdapters(k, &pAdapterUnmanaged)) != DXGI_ERROR_NOT_FOUND)
        {
          DXGI_ADAPTER_DESC adapterDescription ={};
          pAdapterUnmanaged->GetDesc(&adapterDescription);

          GAPIAdapter gapiAdapter={};
          gapiAdapter.name         = toString(adapterDescription.DeviceId);
          gapiAdapter.desc         = toString(adapterDescription.Description);
          gapiAdapter.deviceMemory = adapterDescription.DedicatedVideoMemory;

          uint32_t     j                = 0;
          IDXGIOutput *pOutputUnmanaged = nullptr;
          while((result = pAdapterUnmanaged->EnumOutputs(j, &pOutputUnmanaged)) != DXGI_ERROR_NOT_FOUND)
          {
            DXGI_OUTPUT_DESC outputDescription ={};
            pOutputUnmanaged->GetDesc(&outputDescription);

            GAPIOutput gapiOutput={};
            gapiOutput.name          = toString(outputDescription.DeviceName);
            gapiOutput.width         = (outputDescription.DesktopCoordinates.right - outputDescription.DesktopCoordinates.left);
            gapiOutput.height        = (outputDescription.DesktopCoordinates.bottom - outputDescription.DesktopCoordinates.top);
            gapiOutput.monitorHandle = outputDescription.Monitor;

            uint32_t    outputModeCount = 0;
            DXGI_FORMAT format          = DXGI_FORMAT_R8G8B8A8_UNORM;
            uint32_t    flags           = DXGI_ENUM_MODES_INTERLACED;

            result = pOutputUnmanaged->GetDisplayModeList(format, flags, &outputModeCount, nullptr);
            HandleWINAPIError(result, "Failed to get display mode count.");

            DXGI_MODE_DESC *outputModes = new DXGI_MODE_DESC[outputModeCount];

            result = pOutputUnmanaged->GetDisplayModeList(format, flags, &outputModeCount, outputModes);
            HandleWINAPIError(result, "Failed to get display mode list.");

            for(uint32_t m=0; m < outputModeCount; ++m) {
              DXGI_MODE_DESC mode = outputModes[m];

              GAPIMode gapiMode ={};
              gapiMode.format      = mode.Format;
              gapiMode.width       = mode.Width;
              gapiMode.height      = mode.Height;
              gapiMode.numerator   = mode.RefreshRate.Numerator;
              gapiMode.denominator = mode.RefreshRate.Denominator;

              gapiOutput.modes.push_back(gapiMode);
            }

            delete[] outputModes;

            gapiAdapter.outputs.push_back(gapiOutput);
            ++j;
          }

          // if(j == 0)
          //   Log("No outputs found for adapter @ index " << k);

          caps.adapters.push_back(gapiAdapter);
          ++k;
        }

        if(k == 0)
          HandleWINAPIError(result, "No adapters found.");

        pFactory = nullptr;

        return caps;
      } catch(WINAPIError const&err) {
        Log("WINAPI error occurred(" << err.errorCode << "): " << err.message);
        return GAPICaps();
      }
    }

    bool DirectX
      ::initialize(std::shared_ptr<Environment> const& environment) 
    {

      std::function<bool()> getCapabilities
        = [this] () -> bool 
      {
        GAPICaps caps = DirectX::findAdapters();

        if(caps.adapters.empty()) {
          Log("No adapters available.");
          return false;
        }

        int32_t
          adapterIndex = -1,
          outputIndex  = -1,
          modeIndex    = -1;

        for(uint32_t k=0; k < caps.adapters.size(); ++k) {

          GAPIAdapter const& adapter = caps.adapters[k];

          if(adapter.outputs.empty())
            continue;

          for(uint32_t j=0; j < adapter.outputs.size(); ++j) {

            GAPIOutput const&output = adapter.outputs[j];

            if(output.modes.empty())
              continue;

            uint32_t 
              maxWidth  = 0,
              maxHeight = 0;
            for(uint32_t m=0; m < output.modes.size(); ++m) {
              GAPIMode const&mode = output.modes[m];
              if(maxWidth < mode.width) {
                maxWidth  = mode.width;
                maxHeight = mode.height;
                modeIndex = m;
              }
            }

            outputIndex = j;
          }

          adapterIndex = k;
        }

        if(!(adapterIndex > -1
             && outputIndex > -1 
             && modeIndex   > -1)) {
          Log("No valid selected mode found ("
              << "Adapter: "  << adapterIndex 
              << ", Output: " << outputIndex 
              << ", Mode: "   << modeIndex
              << ")");
          return false;
        }

        SelectedModePath path ={};
        path.adapterIndex = adapterIndex;
        path.outputIndex  = outputIndex;
        path.modeIndex    = modeIndex;

        m_caps             = caps;
        m_selectedModePath = path;

        return true;
      };

      std::function<bool()> initDeviceAndSwapChain
        = [this, &environment] () -> bool
      {
        HRESULT hres = S_OK;

        GAPIMode const& selectedMode = getSelectedMode();

        DXGI_SWAP_CHAIN_DESC swapChainDesc={};
        swapChainDesc.BufferCount = 1; // One single backbuffer

        swapChainDesc.BufferDesc.Width                   = selectedMode.width;
        swapChainDesc.BufferDesc.Height                  = selectedMode.height;
        swapChainDesc.BufferDesc.Format                  = selectedMode.format;
        swapChainDesc.BufferDesc.RefreshRate.Numerator   = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferUsage                        = DXGI_USAGE_BACK_BUFFER 
                                                         | DXGI_USAGE_RENDER_TARGET_OUTPUT;

        swapChainDesc.SampleDesc.Count   = 1;
        swapChainDesc.SampleDesc.Quality = 0;

        swapChainDesc.Windowed     = true;
        swapChainDesc.OutputWindow = environment->mainWindowHandle();
        swapChainDesc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

        IDXGISwapChain      *pSwapChainUnmanaged        = nullptr;
        ID3D11Device        *pDeviceUnmagaged           = nullptr;
        ID3D11DeviceContext *pImmediateContextUnmanaged = nullptr;

        hres = D3D11CreateDeviceAndSwapChain(
          nullptr, // IDXGIAdapter-instance
          D3D_DRIVER_TYPE_HARDWARE,
          0,                          // SW-Module handle
          D3D11_CREATE_DEVICE_DEBUG,  // CreationFlags
          &featureLevel,              // Supported engine feature levels
          1,
          D3D11_SDK_VERSION,
          &swapChainDesc,          
          &pSwapChainUnmanaged,       // Target pointer for swapchain
          &pDeviceUnmagaged,          // Target pointer for device
          nullptr,                    // Supported hardware feature levels
          &pImmediateContextUnmanaged // Target pointer for immediate context
        );
        HandleWINAPIError(hres, "Failed to creat device and/or swapchain.");

        m_swapChain        = MakeDirectX11ResourceSharedPointer(pSwapChainUnmanaged);
        m_device           = MakeDirectX11ResourceSharedPointer(pDeviceUnmagaged);
        m_immediateContext = MakeDirectX11ResourceSharedPointer(pImmediateContextUnmanaged);

        return true;
      };

      std::function<bool()> bindSwapChain
        = [this, &environment] () -> bool
      {
        HRESULT hres = S_OK;

        ID3D11Texture2D *pTextureUnmanaged = nullptr;
        hres = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), ((void**)&pTextureUnmanaged));
        HandleWINAPIError(hres, "Failed to get underlying swapchain backbuffer texture.");

        DXGI_SWAP_CHAIN_DESC swapChainDesc ={};
        hres = m_swapChain->GetDesc(&swapChainDesc);
        HandleWINAPIError(hres, "Failed to access swap chain descriptor.");

        D3D11_RENDER_TARGET_VIEW_DESC desc ={};
        desc.Format             = swapChainDesc.BufferDesc.Format;
        desc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;

        ID3D11RenderTargetView *pRenderTargetViewUnmanaged = nullptr;
        hres = m_device->CreateRenderTargetView(pTextureUnmanaged, &desc, &pRenderTargetViewUnmanaged);
        HandleWINAPIError(hres, "Failed to create render target view.");

        m_mainRenderTarget = MakeDirectX11ResourceSharedPointer(pRenderTargetViewUnmanaged);

        // WE don't need the texture anymore, free it!
        pTextureUnmanaged->Release();

        return true;
      };

      try {
        bool result = true;

        result &= getCapabilities();
        result &= initDeviceAndSwapChain();
        result &= bindSwapChain();

        return true;
      } catch(WINAPIError &err) {
        Log("An error occurred trying to initialize DirectX (" << err.errorCode << "): " << err.message);
        return false;
      }
    }

    bool DirectX
      ::deinitialize() {

      return true;
    }

    SelectedModePath const& 
      DirectX
      ::getSelectedModePath() const 
    {
      return m_selectedModePath;
    }

    GAPIMode const& DirectX
      ::getSelectedMode() const 
    {
      return m_caps
        .adapters[m_selectedModePath.adapterIndex]
        .outputs[m_selectedModePath.outputIndex]
        .modes[m_selectedModePath.modeIndex];
    }

    IDXGISwapChainPtr         
      DirectX::getSwapChain()
    {
      return m_swapChain;
    }

    ID3D11DevicePtr           
      DirectX::getDevice()
    {
      return m_device;
    }

    ID3D11DeviceContextPtr    
      DirectX::getImmediateContext()
    {
      return m_immediateContext;
    }

    ID3D11RenderTargetViewPtr 
      DirectX::getMainRenderTarget()
    {
      return m_mainRenderTarget;
    }
  }
}