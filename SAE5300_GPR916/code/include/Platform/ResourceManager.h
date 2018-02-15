#ifndef __SAE5300_GPR916_RESOURCEMANAGER_H__
#define __SAE5300_GPR916_RESOURCEMANAGER_H__

#include <stdint.h>
#include <string>
#include <map>
#include <memory>

#include "Errors/ErrorHandling.h"

namespace SAE {
  namespace Resources {
    using namespace SAE::Error;

    template <typename T>
    class ResourceHolder {
    public:
      bool set(
        uint64_t           const&id, 
        std::shared_ptr<T> const&handle)
      {
        m_resources[id] = handle;

        return true;
      }
    protected:
      bool get(uint64_t const&id, std::shared_ptr<T>&out) {
        return (out = m_resources[id]) != nullptr;
      }

    private:
      std::map<uint64_t, std::shared_ptr<T>> m_resources;
    };

    template <typename... Types>
    class ResourceManager 
      : public ResourceHolder<Types>...
    {
    public:
      ResourceManager()
      {}

      template <typename T>
      std::shared_ptr<T> get(uint64_t const&id) {
        std::shared_ptr<T> ptr = nullptr;
        this->ResourceHolder<T>::get(id, ptr);

        return ptr;
      }
    };

  }
}

#endif 