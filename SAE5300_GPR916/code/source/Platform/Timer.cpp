#include "Logging/Logging.h"

#include "Platform/Timer.h"

namespace SAE {
  namespace Timing {
    using namespace SAE::Log;

    Timer::State
      ::State(
        double inElapsed,
        double inTotal)
      : elapsed(inElapsed)
      , totalElapsed(inTotal)
    {}

    Timer
      ::Timer()
      : m_frequency({})
      , m_firstTimestamp({})
      , m_previousTimestamp({})
      , m_currentTimestamp({})
      , m_elapsed(0.0)
      , m_totalElapsed(0.0)
    {}

    bool Timer
      ::initialize()
    {
      BOOL frequencyFetchOk = QueryPerformanceFrequency(&m_frequency);
      if(!frequencyFetchOk) {
        Log("Cannot fetch performance frequency.");
        return false;
      }

      BOOL fetchCountOk = QueryPerformanceCounter(&m_firstTimestamp);
      if(!fetchCountOk) {
        Log("Cannot fetch performance counter value for first timestamp.");
        return false;
      }

      return true;
    }

    bool Timer
      ::update()
    {
      m_previousTimestamp = m_currentTimestamp;

      BOOL fetchCountOk = QueryPerformanceCounter(&m_currentTimestamp);
      if(!fetchCountOk) {
        Log("Cannot fetch performance counter value for current timestamp.");
        return false;
      }

      m_elapsed
        = ((m_previousTimestamp.QuadPart - m_currentTimestamp.QuadPart)
           / ((double)m_frequency.QuadPart));
      m_totalElapsed 
        = ((m_firstTimestamp.QuadPart - m_currentTimestamp.QuadPart)
           / ((double)m_frequency.QuadPart));

      return true;
    }

    bool Timer
      ::deinitialize()
    {
      return true;
    }

  }
}