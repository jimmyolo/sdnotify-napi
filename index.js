const addon = require("./build/Release/sdnotify.node");

module.exports = {
  addon,
  ready: (pid = process.pid) => addon.ready(pid),
  stopping: (pid = process.pid) => addon.stopping(pid),
};
