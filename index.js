const addon = require("node-gyp-build")(__dirname);

let watchdogTimeout = null;

module.exports = {
  addon,
  ready: (pid = process.pid) => addon.ready(pid),
  stopping: (pid = process.pid) => addon.stopping(pid),
  status: (msg = '') => addon.status(`STATUS=${msg}\n`),
  watchdog: () => addon.watchdog(),
  watchdogInterval: () => addon.watchdogInterval(),
  startWatchdogMode: (interval) => {
    interval = +interval
    if (interval <= 0) throw new RangeError('interval must be a positive number')

    watchdogTimeout && clearInterval(watchdogTimeout)
    watchdogTimeout = setInterval(addon.watchdog, interval)
  },
  stopWatchdogMode: () => {
    watchdogTimeout && clearInterval(watchdogTimeout)
    watchdogTimeout = null
  },
};
