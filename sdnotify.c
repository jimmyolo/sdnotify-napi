#include <node_api.h>
#include <systemd/sd-daemon.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define READY "READY=1"
#define STOPPING "STOPPING=1"
#define WATCHDOG "WATCHDOG=1"

napi_value ready(napi_env env, napi_callback_info info)
{
  size_t argc = 1;
  napi_value argv[argc];
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

napi_value stopping(napi_env env, napi_callback_info info)
{
  size_t argc = 1;
  napi_value argv[argc];
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

napi_value watchdog(napi_env env, napi_callback_info info)
{
  sd_notify(0, WATCHDOG);
  return NULL;
}

napi_value watchdog_interval(napi_env env, napi_callback_info info)
{
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

char *read_js_string(napi_env env, napi_value js_value)
{
  size_t len;
  napi_get_value_string_utf8(env, js_value, NULL, 0, &len);

  napi_status status;

  char *value = (char *)malloc(len + 1);
  status = napi_get_value_string_utf8(env, js_value, value, len + 1, &len);
  if (status != napi_ok) {
    free(value);
    napi_throw_error(env, NULL, "Unable to convert js string to char *");
    return NULL;
  }
  // printf("%s", value);

  return value;
}

napi_value status(napi_env env, napi_callback_info info)
{
  size_t argc = 1;
  napi_value argv[argc];
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  char *js_string = read_js_string(env, argv[0]);

  sd_notify(0, js_string);

  free(js_string);
  return NULL;
}

napi_value Init(napi_env env, napi_value exports)
{
  napi_value fn;

  napi_create_function(env, NULL, 0, ready, NULL, &fn);
  napi_set_named_property(env, exports, "ready", fn);

  napi_create_function(env, NULL, 0, stopping, NULL, &fn);
  napi_set_named_property(env, exports, "stopping", fn);

  napi_create_function(env, NULL, 0, watchdog, NULL, &fn);
  napi_set_named_property(env, exports, "watchdog", fn);

  napi_create_function(env, NULL, 0, watchdog_interval, NULL, &fn);
  napi_set_named_property(env, exports, "watchdogInterval", fn);

  napi_create_function(env, NULL, 0, status, NULL, &fn);
  napi_set_named_property(env, exports, "status", fn);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
