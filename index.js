const addon = require("node-gyp-build")(__dirname);

module.exports = {
  addon,
  ready: (pid = process.pid) => addon.ready(pid),
  stopping: (pid = process.pid) => addon.stopping(pid),
};
