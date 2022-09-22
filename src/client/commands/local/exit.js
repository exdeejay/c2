module.exports = function (commands) {
    commands['exit'] = exit;
};

function exit(server, args) {
    process.exit(0);
}
