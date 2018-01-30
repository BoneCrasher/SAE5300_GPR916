#ifndef __SAE5300_GPR917_DX11ENV_H__
#define __SAE5300_GPR917_DX11ENV_H__

#include <vector>
#include "Platform/Environment.h"
#include "DirectX11Common.h"

namespace SAE {
  namespace DirectX11 {
    using SAE::Environment::Environment;

    struct GAPIMode {
      DXGI_FORMAT 
        format;
      uint32_t
        width,
        height;
      double
        numerator,
        denominator;
    };

    struct GAPIOutput {
      std::string 
        name;
      uint32_t
        width,
        height;
      HMONITOR    
        monitorHandle;
      std::vector<GAPIMode> modes;
    };

    struct GAPIAdapter {
      std::string name;
      std::string desc;
      uint32_t    deviceMemory;
      std::vector<GAPIOutput> outputs;
    };

    struct GAPICaps {
      std::vector<GAPIAdapter> adapters;
    };

    struct SelectedModePath {
      uint32_t
        adapterIndex,
        outputIndex,
        modeIndex;
    };

    class DirectX {
    public:
      static GAPICaps findAdapters();

      DirectX();

      bool initialize(std::shared_ptr<Environment> const& environment);
      bool deinitialize();

      SelectedModePath const& getSelectedModePath() const;
      GAPIMode const& getSelectedMode() const;

      IDXGISwapChainPtr         getSwapChain();
      ID3D11DevicePtr           getDevice();
      ID3D11DeviceContextPtr    getImmediateContext();
      ID3D11RenderTargetViewPtr getMainRenderTarget();
      
    private:
      GAPICaps         m_caps;
      SelectedModePath m_selectedModePath;

      IDXGISwapChainPtr      m_swapChain;
      ID3D11DevicePtr        m_device;
      ID3D11DeviceContextPtr m_immediateContext;

      ID3D11RenderTargetViewPtr m_mainRenderTarget;
    };

  }
}

#endif