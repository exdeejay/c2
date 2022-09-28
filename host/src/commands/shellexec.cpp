#include <string>
#include <optional>
#include "controller.h"
#include <Windows.h>
#include <shellapi.h>
using namespace std;

int shellExecute(Controller& ctrl, const optional<string> maybeVerb, const optional<string> maybeFile, const optional<string> maybeParams, const optional<string> maybeDir, const optional<int> maybeNShowCmd) {
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
    if (!ShellExecuteA(NULL, verb, file, params, dir, maybeNShowCmd.value_or(1))) {
        return 1;
    }
    return 0;
}
