import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

export = function (commands: CommandList) {
    commands['echo'] = echo;
};

function echo(server: ControlServer, args: string[]) {
    console.log(args.slice(1).join(' '));
}
