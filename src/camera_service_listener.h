#ifndef LACAM_CAMERA_SERVICE_LISTENER__
#define LACAM_CAMERA_SERVICE_LISTENER__

#include <android/hardware/BnCameraServiceListener.h>

using android::binder::Status;
using android::hardware::BnCameraServiceListener;
using android::String16;

class CameraServiceListener : public BnCameraServiceListener {
public:
  Status onStatusChanged(int32_t status,
      const String16& cameraId) override;
  Status onPhysicalCameraStatusChanged(int32_t status,
      const String16& cameraId, const String16& physicalCameraId) override;
  Status onTorchStatusChanged(int32_t status, const String16& cameraId)
      override;
  Status onCameraAccessPrioritiesChanged() override;
  Status onCameraOpened(const String16& cameraId,
      const String16& clientPackageName) override;
  Status onCameraClosed(const String16& cameraId) override;
};

#endif
