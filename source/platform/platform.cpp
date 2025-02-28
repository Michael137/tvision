#include <internal/platform.h>
#include <internal/unixcon.h>
#include <internal/linuxcon.h>
#include <internal/win32con.h>
#include <internal/sighandl.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>

// This is used by TText. It is a global function pointer (instead of a
// Platform instance method) so that it can be still used after
// Platform::instance has been destroyed.
int (*Platform::charWidth)(uint32_t) noexcept = &Platform::errorCharWidth;

int Platform::errorCharWidth(uint32_t) noexcept
{
    fputs( "Cannot measure character widths before the platform module is "
           "loaded.\nAvoid invoking TText methods during static initialization.\n",
           stderr );
    abort();
}

Platform Platform::instance;

Platform::Platform() noexcept
{
#ifdef _WIN32
    setlocale(LC_ALL, ".utf8");
    charWidth = &Win32ConsoleStrategy::charWidth;
#else
    setlocale(LC_ALL, "");
    charWidth =
#ifdef __linux__
        io.isLinuxConsole() ? &LinuxConsoleStrategy::charWidth :
#endif // __linux__
        &UnixConsoleStrategy::charWidth;
#endif // _WIN32
}

Platform::~Platform()
{
    restoreConsole();
}

void Platform::restoreConsole() noexcept
{
    auto doRemove = [] (void *self, EventSource &source) {
        ((Platform *) self)->waiter.removeSource(source);
    };
    console.lock([&] (ConsoleStrategy *&c) {
        if (c != &dummyConsole)
        {
            flushScreen();
            c->forEachSource(this, *(void (*)(void *, EventSource &)) doRemove);
            SignalHandler::disable();
            delete c;
            c = &dummyConsole;
        }
    });
}

// The remaining methods are in platfcon.cpp.
