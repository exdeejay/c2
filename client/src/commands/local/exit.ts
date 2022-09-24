import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

export = function (commands: CommandList) {
    commands['exit'] = exit;
};

function exit(server: ControlServer, args: string[]) {
    process.exit(0);
}
