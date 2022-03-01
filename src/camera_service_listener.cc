#include "camera_service_listener.h"

#ifndef UNUSED
#define UNUSED(x) (void)(x);
#endif

Status onStatusChanged(int32_t status, const String16& cameraId)
{
  UNUSED(status);
  UNUSED(cameraId);
  return Status::ok();
}

Status onPhysicalCameraStatusChanged(int32_t status,
    const String16& cameraId, const String16& physicalCameraId)
{
  UNUSED(status);
  UNUSED(cameraId);
  UNUSED(physicalCameraId);
  return Status::ok();
}

Status onTorchStatusChanged(int32_t status, const String16& cameraId)
{
  UNUSED(status);
  UNUSED(cameraId);
  return Status::ok();
}

Status onCameraAccessPrioritiesChanged()
{
  return Status::ok();
}

Status onCameraOpened(const String16& cameraId,
    const String16& clientPackageName)
{
  UNUSED(cameraId);
  UNUSED(clientPackageName);
  return Status::ok();
}

Status onCameraClosed(const String16& cameraId)
{
  UNUSED(cameraId);
  return Status::ok();
}
