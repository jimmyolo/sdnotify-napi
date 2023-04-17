#include <node_api.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>

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

napi_value init_all(napi_env env, napi_value exports) {
  napi_value fn;

  napi_create_function(env, NULL, 0, ready, NULL, &fn);
  napi_set_named_property(env, exports, "ready", fn);

  napi_create_function(env, NULL, 0, stopping, NULL, &fn);
  napi_set_named_property(env, exports, "stopping", fn);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
