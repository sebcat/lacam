#include <binder/IServiceManager.h>
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <android/hardware/ICameraService.h>
#include <camera/CameraMetadata.h>
#include <camera/VendorTagDescriptor.h>
#include <utils/String16.h>

#include <cstring>
#include <sstream>

#include "lacam.h"
#include "camera_device_callbacks.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))
#endif

#define LACAM_SERVICE "lacam_service"
#define LACAM_VENDOR_TAG_DESCRIPTOR "lacam_vendor_tag_descriptor"
#define LACAM_VENDOR_TAG_DESCRIPTOR_CACHE "lacam_vendor_tag_descriptor_cache"
#define LACAM_CAMERA_METADATA "lacam_camera_metadata"

using android::sp;
using android::status_t;
using android::interface_cast;
using android::String16;
using android::IServiceManager;
using android::IBinder;
using android::hardware::ICameraService;
using android::hardware::camera2::ICameraDeviceCallbacks;
using android::hardware::camera2::ICameraDeviceUser;
using android::CameraMetadata;
using android::VendorTagDescriptor;
using android::VendorTagDescriptorCache;


/* NB: lua_newuserdata does not zero memory and
 * StrongPointer:operator= will try to dereference
 * it on assignment. At the same time we don't want to
 * unbox sp's all the time. Always allocate with zero
 * initialization (lua_newuserdataz) */
static inline void *lua_newuserdataz(lua_State *L, size_t size)
{
  void *data = lua_newuserdata(L, size);
  memset(data, 0, size);
  return data;
}
#undef lua_newuserdata
#define lua_newuserdata lua_newuserdataz

struct lacam_service {
  sp<ICameraService> service;
};

struct lacam_vendor_tag_descriptor {
  sp<VendorTagDescriptor> desc;
};

struct lacam_vendor_tag_descriptor_cache {
  sp<VendorTagDescriptorCache> cache;
};

struct lacam_camera_metadata {
  CameraMetadata *metadata;
};

struct nameival {
  const char *name;
  int value;
};

struct metatbl {
  const char *name;
  const luaL_Reg *reg;
};

static inline struct lacam_service *tolacam_service(lua_State *L, int arg)
{
  return (struct lacam_service*)luaL_checkudata(L, arg, LACAM_SERVICE);
}

static inline int _checkstatus(lua_State *L, android::binder::Status status,
    const char *prefix)
{
  if (!status.isOk()) {
    std::stringstream ss;
    ss << status;
    return luaL_error(L, "%s: %s", prefix, ss.str().c_str());
  }

  return 0;
}

#define checkstatus(l, s) _checkstatus((l), (s), __func__)

static inline struct lacam_vendor_tag_descriptor *
    tolacam_vendor_tag_descriptor(lua_State *L, int arg)
{
  return (struct lacam_vendor_tag_descriptor*)
      luaL_checkudata(L, arg, LACAM_VENDOR_TAG_DESCRIPTOR);
}

static inline struct lacam_vendor_tag_descriptor_cache *
    tolacam_vendor_tag_descriptor_cache(lua_State *L, int arg)
{
  return (struct lacam_vendor_tag_descriptor_cache*)
      luaL_checkudata(L, arg, LACAM_VENDOR_TAG_DESCRIPTOR_CACHE);
}

static inline struct lacam_camera_metadata *
    tolacam_camera_metadata(lua_State *L, int arg)
{
  return (struct lacam_camera_metadata*)
      luaL_checkudata(L, arg, LACAM_CAMERA_METADATA);
}

static int lacam_vtd_set_as_global(lua_State *L)
{
  struct lacam_vendor_tag_descriptor *vtd =
      tolacam_vendor_tag_descriptor(L, 1);
  status_t err = VendorTagDescriptor::setAsGlobalVendorTagDescriptor(vtd->desc);
  if (err != android::OK) {
      return luaL_error(L, "failed to set global vendor tag descriptor: %s",
          strerror(-err));
  }

  return 0;
}

static int lacam_vtd_dump(lua_State *L)
{
  lua_Integer fd;
  lua_Integer verbose;
  lua_Integer indent;

  struct lacam_vendor_tag_descriptor *vtd =
      tolacam_vendor_tag_descriptor(L, 1);
  fd = luaL_checkinteger(L, 2);
  verbose = luaL_checkinteger(L, 3);
  indent = luaL_checkinteger(L, 4);
  vtd->desc->dump(fd, verbose, indent);
  return 0;
}

static int lacam_vtd_gc(lua_State *L)
{
  struct lacam_vendor_tag_descriptor *vtd =
      tolacam_vendor_tag_descriptor(L, 1);
  vtd->desc = nullptr;
  return 0;
}

static int lacam_vtc_set_as_global(lua_State *L)
{
  struct lacam_vendor_tag_descriptor_cache *vtc =
      tolacam_vendor_tag_descriptor_cache(L, 1);
  status_t err = VendorTagDescriptorCache::setAsGlobalVendorTagCache(vtc->cache);
  if (err != android::OK) {
      return luaL_error(L, "failed to set global vendor tag cache: %s",
          strerror(-err));
  }

  return 0;
}

static int lacam_vtc_dump(lua_State *L)
{
  lua_Integer fd;
  lua_Integer verbose;
  lua_Integer indent;

  struct lacam_vendor_tag_descriptor_cache *vtc =
      tolacam_vendor_tag_descriptor_cache(L, 1);
  fd = luaL_checkinteger(L, 2);
  verbose = luaL_checkinteger(L, 3);
  indent = luaL_checkinteger(L, 4);
  vtc->cache->dump(fd, verbose, indent);
  return 0;
}

static int lacam_vtc_gc(lua_State *L)
{
  struct lacam_vendor_tag_descriptor_cache *vtc =
      tolacam_vendor_tag_descriptor_cache(L, 1);
  vtc->cache = nullptr;
  return 0;
}

static int lacam_camera_metadata_dump(lua_State *L)
{
  struct lacam_camera_metadata *metadata;
  lua_Integer fd;
  lua_Integer verbose;
  lua_Integer indent;

  metadata = tolacam_camera_metadata(L, 1);
  fd = luaL_checkinteger(L, 2);
  verbose = luaL_checkinteger(L, 3);
  indent = luaL_checkinteger(L, 4);
  metadata->metadata->dump(fd, verbose, indent);
  return 0;
}

static int lacam_camera_metadata_gc(lua_State *L)
{
  struct lacam_camera_metadata *metadata =
      tolacam_camera_metadata(L, 1);
  delete metadata->metadata;
  metadata->metadata = nullptr;
  return 0;
}

static int lacam_get_service(lua_State *L)
{
  struct lacam_service *svc;
  
  svc = (struct lacam_service*)lua_newuserdata(L,
      sizeof(struct lacam_service));
  luaL_setmetatable(L, LACAM_SERVICE);
  sp<IServiceManager> sm = android::defaultServiceManager();
  sp<IBinder> binder = sm->getService(String16("media.camera"));
  if (binder == nullptr) {
    return luaL_error(L, "failed to get media.camera service");
  }

  svc->service = ICameraService::asInterface(binder);
  return 1;
}

static int lacam_service_get_number_of_cameras(lua_State *L)
{
  int ncams = -1;
  struct lacam_service *svc = tolacam_service(L, 1);
  lua_Integer type = luaL_checkinteger(L, 2);
  auto status = svc->service->getNumberOfCameras(type, &ncams);
  checkstatus(L, status);
  lua_pushinteger(L, ncams);
  return 1;
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
  struct lacam_service *svc = tolacam_service(L, 1);
  const char *camera_id = luaL_checkstring(L, 2);
  const char *op_package_name = luaL_checkstring(L, 3);
  lua_Integer client_uid = luaL_checkinteger(L, 5);
  std::unique_ptr<String16> feat;

  if (!lua_isnil(L, 4)) {
    const char *feature_id = luaL_checkstring(L, 4);
    feat = std::unique_ptr<String16>(new String16(feature_id));
  } else {
    feat = std::unique_ptr<String16>();
  }

  /* TODO: move to own data struct once things work */
  sp<ICameraDeviceCallbacks> callbacks(new CameraDeviceCallbacks());
  sp<ICameraDeviceUser> device;
  auto status = svc->service->connectDevice(callbacks, String16(camera_id),
      String16(op_package_name), feat, client_uid, &device);
  checkstatus(L, status);

  /* TODO: return camera device object (callbacks + ICameraDeviceUser impl) */
  return 0;
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

static struct lacam_camera_metadata *new_camera_metadata(lua_State *L)
{
  struct lacam_camera_metadata *metadata;

  metadata = (struct lacam_camera_metadata*)lua_newuserdata(L,
      sizeof(struct lacam_camera_metadata));
  luaL_setmetatable(L, LACAM_CAMERA_METADATA);
  metadata->metadata = new CameraMetadata();
  return metadata;
}

static int lacam_service_get_camera_characteristics(lua_State *L)
{
  struct lacam_service *svc;
  struct lacam_camera_metadata *metadata;
  const char *camera_id;

  svc = tolacam_service(L, 1);
  camera_id = luaL_checkstring(L, 2);
  if (!camera_id || *camera_id == '\0') {
    return luaL_error(L, "invalid camera ID");
  }

  metadata = new_camera_metadata(L);
  auto status = svc->service->getCameraCharacteristics(String16(camera_id),
      metadata->metadata);
  checkstatus(L, status);

  return 1;
}

static int lacam_service_get_camera_vendor_tag_descriptor(lua_State *L)
{
  struct lacam_vendor_tag_descriptor *vtd;
  struct lacam_service *svc;

  svc = tolacam_service(L, 1);
  vtd = (struct lacam_vendor_tag_descriptor*)lua_newuserdata(L,
      sizeof(struct lacam_vendor_tag_descriptor));
  luaL_setmetatable(L, LACAM_VENDOR_TAG_DESCRIPTOR);
  vtd->desc = new VendorTagDescriptor();
  auto status = svc->service->getCameraVendorTagDescriptor(vtd->desc.get());
  checkstatus(L, status);

  return 1;
}

static int lacam_service_get_camera_vendor_tag_cache(lua_State *L)
{
  struct lacam_vendor_tag_descriptor_cache *vtc;
  struct lacam_service *svc;

  svc = tolacam_service(L, 1);
  vtc = (struct lacam_vendor_tag_descriptor_cache*)lua_newuserdata(L,
      sizeof(struct lacam_vendor_tag_descriptor_cache));
  luaL_setmetatable(L, LACAM_VENDOR_TAG_DESCRIPTOR_CACHE);
  vtc->cache = new VendorTagDescriptorCache();
  auto status = svc->service->getCameraVendorTagCache(vtc->cache.get());
  checkstatus(L, status);

  return 1;
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

static int lacam_service_gc(lua_State *L)
{
  struct lacam_service *svc;

  svc = tolacam_service(L, 1);
  svc->service = nullptr;
  return 0;
}

static const luaL_Reg lacamvendortagdesc[] = {
  {"__gc", lacam_vtd_gc},
  {"set_as_global", lacam_vtd_set_as_global},
  {"dump", lacam_vtd_dump},
  {},
};

static const luaL_Reg lacamvendortagdesccache[] = {
  {"__gc", lacam_vtc_gc},
  {"set_as_global", lacam_vtc_set_as_global},
  {"dump", lacam_vtc_dump},
  {},
};

static const luaL_Reg lacamcamerametadata[] = {
  {"__gc", lacam_camera_metadata_gc},
  {"dump", lacam_camera_metadata_dump},
  {},
};

static const luaL_Reg lacamservice[] = {
  {"__gc", lacam_service_gc},
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

static const struct metatbl metatbls[] = {
  {LACAM_SERVICE, lacamservice},
  {LACAM_VENDOR_TAG_DESCRIPTOR, lacamvendortagdesc},
  {LACAM_VENDOR_TAG_DESCRIPTOR_CACHE, lacamvendortagdesccache},
  {LACAM_CAMERA_METADATA, lacamcamerametadata},
};

static struct nameival intvals[] = {
  {"ERROR_PERMISSION_DENIED", ICameraService::ERROR_PERMISSION_DENIED},
  {"ERROR_ALREADY_EXISTS", ICameraService::ERROR_ALREADY_EXISTS},
  {"ERROR_ILLEGAL_ARGUMENT", ICameraService::ERROR_ILLEGAL_ARGUMENT},
  {"ERROR_DISCONNECTED", ICameraService::ERROR_DISCONNECTED},
  {"ERROR_TIMED_OUT", ICameraService::ERROR_TIMED_OUT},
  {"ERROR_DISABLED", ICameraService::ERROR_DISABLED},
  {"ERROR_CAMERA_IN_USE", ICameraService::ERROR_CAMERA_IN_USE},
  {"ERROR_MAX_CAMERAS_IN_USE", ICameraService::ERROR_MAX_CAMERAS_IN_USE},
  {"ERROR_DEPRECATED_HAL", ICameraService::ERROR_DEPRECATED_HAL},
  {"ERROR_INVALID_OPERATION", ICameraService::ERROR_INVALID_OPERATION},
  {"CAMERA_TYPE_BACKWARD_COMPATIBLE", ICameraService::CAMERA_TYPE_BACKWARD_COMPATIBLE},
  {"CAMERA_TYPE_ALL", ICameraService::CAMERA_TYPE_ALL},
  {"USE_CALLING_UID", ICameraService::USE_CALLING_UID},
  {"USE_CALLING_PID", ICameraService::USE_CALLING_PID},
  {"API_VERSION_1", ICameraService::API_VERSION_1},
  {"API_VERSION_2", ICameraService::API_VERSION_2},
  {"EVENT_NONE", ICameraService::EVENT_NONE},
  {"EVENT_USER_SWITCHED", ICameraService::EVENT_USER_SWITCHED},
  {"DEVICE_STATE_NORMAL", ICameraService::DEVICE_STATE_NORMAL},
  {"DEVICE_STATE_BACK_COVERED", ICameraService::DEVICE_STATE_BACK_COVERED},
  {"DEVICE_STATE_FRONT_COVERED", ICameraService::DEVICE_STATE_FRONT_COVERED},
  {"DEVICE_STATE_FOLDED", ICameraService::DEVICE_STATE_FOLDED},
  {"DEVICE_STATE_LAST_FRAMEWORK_BIT", ICameraService::DEVICE_STATE_LAST_FRAMEWORK_BIT},
};

LUALIB_API int luaopen_lacam(lua_State *L)
{
  int i;

  lua_newtable(L);
  luaL_setfuncs(L, lacamlib, 0);

  for (i = 0; i < ARRAY_SIZE(intvals); i++) {
    lua_pushinteger(L, intvals[i].value);
    lua_setfield(L, -2, intvals[i].name);
  }

  for (i = 0; i < ARRAY_SIZE(metatbls); i++) {
    luaL_newmetatable(L, metatbls[i].name);
    luaL_setfuncs(L, metatbls[i].reg, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -1, "__index");
    lua_pop(L, 1);
  }

  return 1;
}
