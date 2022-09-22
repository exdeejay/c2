module.exports = function (commands) {
    commands['echo'] = echo;
};

function echo(server, args) {
    console.log(args.slice(1).join(' '));
}
