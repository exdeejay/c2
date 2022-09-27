#include <string>
#include <optional>
#include "controller.h"
#include <Windows.h>
using namespace std;

int shellExecute(Controller& ctrl, const optional<string> maybeVerb, const optional<string> maybeFile, const optional<string> maybeDir, const optional<int> maybeNShowCmd) {
    char* verb = nullptr;
    if (maybeVerb) {
        verb = maybeVerb.value().c_str();
    }
    char* file = nullptr;
    if (maybeFile) {
        file = maybeFile.value().c_str();
    }
    char* dir = nullptr;
    if (maybeDir) {
        dir = maybeDir.value().c_str();
    }
    if (!ShellExecuteA(nullptr, verb, file, dir, maybeNShowCmd.value_or(1))) {
        return 1;
    }
    return 0;
}
