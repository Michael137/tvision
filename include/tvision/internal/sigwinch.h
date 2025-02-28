#ifndef TVISION_SIGWINCH_H
#define TVISION_SIGWINCH_H

#include <tvision/tv.h>

#ifdef _TV_UNIX
#include <internal/events.h>
#include <signal.h>

struct TEvent;

class SigwinchHandler
{
    WakeUpEventSource eventSource;
    struct sigaction oldSa;

    static SigwinchHandler *instance;
    static void handleSignal(int) noexcept;
    static bool getEvent(void *, TEvent &) noexcept;

    SigwinchHandler( SysManualEvent::Handle handle,
                     const struct sigaction &aOldSa ) noexcept;

public:

    static SigwinchHandler *create() noexcept;
    ~SigwinchHandler();

    EventSource &getEventSource() noexcept;
};

inline EventSource &SigwinchHandler::getEventSource() noexcept
{
    return eventSource;
}

#endif // _TV_UNIX

#endif // TVISION_SIGWINCH_H
