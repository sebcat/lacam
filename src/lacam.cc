#include "lacam.h"

#define LACAM_SERVICE "lacam_service"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))
#endif

struct lacam_service {
  int val;
};

struct nameival {
  const char *name;
  int value;
};

static inline struct lacam_service *tolacam_service(lua_State *L, int arg)
{
  return (struct lacam_service*)luaL_checkudata(L, arg, LACAM_SERVICE);
}

static int lacam_service_thing(lua_State *L)
{
  struct lacam_service *svc = tolacam_service(L, 1);
  lua_pushinteger(L, svc->val);
  return 1;
}

static int lacam_get_service(lua_State *L)
{
  int ret;
  int nargs;
  struct lacam_service *svc;
  
  svc = (struct lacam_service*)lua_newuserdata(L,
      sizeof(struct lacam_service));
  luaL_setmetatable(L, LACAM_SERVICE);

  /* TODO: populate 'svc' */
  svc->val = 42;

  return 1;
}

static int lacam_service_get_number_of_cameras(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_get_camera_info(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_connect(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_connect_device(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_add_listener(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_get_concurrent_camera_ids(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_is_concurrent_session_combination_supported(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_remove_listener(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_get_camera_characteristics(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_get_camera_vendor_tag_descriptor(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_get_camera_vendor_tag_cache(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_get_legacy_parameters(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_supports_camera_api(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_is_hidden_physical_camera(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_inject_camera(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_set_torch_mode(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_notify_system_event(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_notify_display_configuration_change(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static int lacam_service_notify_device_state_change(lua_State *L)
{
  return luaL_error(L, "NYI");
}

static const luaL_Reg lacamservice[] = {
  {"thing", lacam_service_thing},
  {"get_number_of_cameras", lacam_service_get_number_of_cameras},
  {"get_camera_info", lacam_service_get_camera_info},
  {"connect", lacam_service_connect},
  {"connect_device", lacam_service_connect_device},
  {"add_listener", lacam_service_add_listener},
  {"get_concurrent_camera_ids", lacam_service_get_concurrent_camera_ids},
  {"is_concurrent_session_combination_supported",
      lacam_service_is_concurrent_session_combination_supported},
  {"remove_listener", lacam_service_remove_listener},
  {"get_camera_characteristics", lacam_service_get_camera_characteristics},
  {"get_camera_vendor_tag_descriptor",
      lacam_service_get_camera_vendor_tag_descriptor},
  {"get_camera_vendor_tag_cache",
      lacam_service_get_camera_vendor_tag_cache},
  {"get_legacy_parameters", lacam_service_get_legacy_parameters},
  {"supports_camera_api", lacam_service_supports_camera_api},
  {"is_hidden_physical_camera", lacam_service_is_hidden_physical_camera},
  {"inject_camera", lacam_service_inject_camera},
  {"set_torch_mode", lacam_service_set_torch_mode},
  {"notify_system_event", lacam_service_notify_system_event},
  {"notify_display_configuration_change",
      lacam_service_notify_display_configuration_change},
  {"notify_device_state_change", lacam_service_notify_device_state_change},

  {},
};

static const luaL_Reg lacamlib[] = {
  {"get_service", lacam_get_service},
  {},
};

static struct nameival intvals[] = {
  {"ERROR_PERMISSION_DENIED", 1},
  {"ERROR_ALREADY_EXISTS", 2},
  {"ERROR_ILLEGAL_ARGUMENT", 3},
  {"ERROR_DISCONNECTED", 4},
  {"ERROR_TIMED_OUT", 5},
  {"ERROR_DISABLED", 6},
  {"ERROR_CAMERA_IN_USE", 7},
  {"ERROR_MAX_CAMERAS_IN_USE", 8},
  {"ERROR_DEPRECATED_HAL", 9},
  {"ERROR_INVALID_OPERATION", 10},
  {"CAMERA_TYPE_BACKWARD_COMPATIBLE", 0},
  {"CAMERA_TYPE_ALL", 1},
  {"USE_CALLING_UID", -1},
  {"USE_CALLING_PID", -1},
  {"API_VERSION_1", 1},
  {"API_VERSION_2", 2},
  {"EVENT_NONE", 0},
  {"EVENT_USER_SWITCHED", 1}, // The argument is the set of new foreground user IDs.
  {"DEVICE_STATE_NORMAL", 0},
  {"DEVICE_STATE_BACK_COVERED", 1},
  {"DEVICE_STATE_FRONT_COVERED", 2},
  {"DEVICE_STATE_FOLDED", 4},
  {"DEVICE_STATE_LAST_FRAMEWORK_BIT", (1<<31)},
};

LUALIB_API int luaopen_lacam(lua_State *L)
{
  int i;

  luaL_newmetatable(L, LACAM_SERVICE);
  luaL_setfuncs(L, lacamservice, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -1, "__index");

  lua_newtable(L);
  luaL_setfuncs(L, lacamlib, 0);

  for (i = 0; i < ARRAY_SIZE(intvals); i++) {
    lua_pushinteger(L, intvals[i].value);
    lua_setfield(L, -2, intvals[i].name);
  }

  return 1;
}
