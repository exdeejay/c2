module.exports = function (commands) {
    commands['exit'] = exit;
};

function exit(server, args) {
    server.close();
    process.exit(0);
}
