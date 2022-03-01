#ifndef LACAM_CAMERA_DEVICE_CALLBACKS__
#define LACAM_CAMERA_DEVICE_CALLBACKS__

#include <vector>
#include <binder/IInterface.h>
#include <android/hardware/camera2/BnCameraDeviceCallbacks.h>

using android::hardware::camera2::BnCameraDeviceCallbacks;
using android::hardware::camera2::impl::CaptureResultExtras;
using android::hardware::camera2::impl::CameraMetadataNative;
using android::hardware::camera2::impl::PhysicalCaptureResultInfo;
using android::binder::Status;
using android::IBinder;

class CameraDeviceCallbacks : public BnCameraDeviceCallbacks {
public:
  Status onDeviceError(int32_t errorCode, const CaptureResultExtras& resultExtras) override;
  Status onDeviceIdle() override;
  Status onCaptureStarted(const CaptureResultExtras& resultExtras, int64_t timestamp) override;
  Status onResultReceived(const CameraMetadataNative& result,
      const CaptureResultExtras& resultExtras,
      const std::vector<PhysicalCaptureResultInfo>& physicalCaptureResultInfos) override;
  Status onPrepared(int32_t streamId) override;
  Status onRepeatingRequestError(int64_t lastFrameNumber, int32_t repeatingRequestId) override;
  Status onRequestQueueEmpty() override;
private:
};

#endif
