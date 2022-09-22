
const server = require('../../server');

module.exports = function (args) {
    server.connectedServer.close();
    process.exit(0);
}