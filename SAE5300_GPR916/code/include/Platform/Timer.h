#ifndef __SAE5300_GPR916_TIMER_H__
#define __SAE5300_GPR916_TIMER_H__

#include <Windows.h>

namespace SAE {
  namespace Timing {

    class Timer {
    public:
      struct State {
        double
          elapsed,
          totalElapsed;

        State(
          double inElapsed,
          double inTotal);
      };

      Timer();

      bool initialize();
      bool update();
      bool deinitialize();

      inline State state() const { return State(elapsed(), totalElapsed()); }

      inline double elapsed()      const { return m_elapsed;      }
      inline double totalElapsed() const { return m_totalElapsed; }

    private:
      LARGE_INTEGER
        m_frequency,
        m_firstTimestamp,
        m_previousTimestamp,
        m_currentTimestamp;
      double
        m_elapsed,
        m_totalElapsed;
    };

  }
}

#endif