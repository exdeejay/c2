#include <string>
#include <optional>
#include "controller.h"
#include "commands.h"
#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h>
#endif
using namespace std;

int ShellExecuteCommand::execute(Controller& ctrl, const optional<string> maybeVerb, optional<string> maybeFile, optional<string> maybeParams, optional<string> maybeDir, optional<int> maybeNShowCmd) {
    const char* verb = nullptr;
    if (maybeVerb) {
        verb = maybeVerb.value().c_str();
    }
    const char* file = nullptr;
    if (maybeFile) {
        file = maybeFile.value().c_str();
    }
    const char* params = nullptr;
    if (maybeParams) {
        params = maybeParams.value().c_str();
    }
    const char* dir = nullptr;
    if (maybeDir) {
        dir = maybeDir.value().c_str();
    }
    #ifdef _WIN32
    if (!ShellExecuteA(NULL, verb, file, params, dir, maybeNShowCmd.value_or(1))) {
        return 1;
    }
    #endif
    return 0;
}
