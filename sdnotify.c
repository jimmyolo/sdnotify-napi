#include <node_api.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>
#include <string.h>

#define READY "READY=1"
#define STOPPING "STOPPING=1"

napi_value ready(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[1];
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  int32_t pid;
  if (argc < 1) {
    pid = getpid();
  } else {
    napi_get_value_int32(env, argv[0], &pid);
  }

  sd_pid_notify(pid, 0, READY);
  return NULL;
}

napi_value stopping(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[1];
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  int32_t pid;
  if (argc < 1) {
    pid = getpid();
  } else {
    napi_get_value_int32(env, argv[0], &pid);
  }

  sd_pid_notify(pid, 0, STOPPING);
  return NULL;
}

napi_value is_watchdog_enabled(napi_env env, napi_callback_info info) {
  uint64_t usec = 0;
  int enabled = sd_watchdog_enabled(0, &usec);
  if (enabled > 0) {
    napi_value js_timeout;
    napi_create_int64(env, (int64_t)usec/1000, &js_timeout);
    return js_timeout;
  } else if (enabled == 0) {
    // Watchdog is not enabled
    return NULL;
  } else {
    const char *error_msg = strerror(-enabled);
    napi_value js_error_msg;
    napi_create_string_utf8(env, error_msg, NAPI_AUTO_LENGTH, &js_error_msg);
    napi_value js_error;
    napi_create_error(env, NULL, js_error_msg, &js_error);
    return js_error;
  }
}

napi_value init_all(napi_env env, napi_value exports) {
  napi_value fn;

  napi_create_function(env, NULL, 0, ready, NULL, &fn);
  napi_set_named_property(env, exports, "ready", fn);

  napi_create_function(env, NULL, 0, stopping, NULL, &fn);
  napi_set_named_property(env, exports, "stopping", fn);

  napi_create_function(env, NULL, 0, is_watchdog_enabled, NULL, &fn);
  napi_set_named_property(env, exports, "isWatchdogEnabled", fn);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
