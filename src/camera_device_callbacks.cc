#include "camera_device_callbacks.h"

#ifndef UNUSED
#define UNUSED(x) (void)(x);
#endif

Status CameraDeviceCallbacks::onDeviceError(int32_t errorCode, const CaptureResultExtras& resultExtras)
{
  UNUSED(errorCode);
  UNUSED(resultExtras);
  return Status::ok(); /* TODO: Implement */
}

Status CameraDeviceCallbacks::onDeviceIdle()
{
  return Status::ok(); /* TODO: Implement */
}

Status CameraDeviceCallbacks::onCaptureStarted(const CaptureResultExtras& resultExtras,
    int64_t timestamp)
{
  UNUSED(resultExtras);
  UNUSED(timestamp);
  return Status::ok(); /* TODO: Implement */
}

Status CameraDeviceCallbacks::onResultReceived(const CameraMetadataNative& result,
      const CaptureResultExtras& resultExtras,
      const std::vector<PhysicalCaptureResultInfo>& physicalCaptureResultInfos)
{
  UNUSED(result);
  UNUSED(resultExtras);
  UNUSED(physicalCaptureResultInfos);
  return Status::ok(); /* TODO: Implement */
}

Status CameraDeviceCallbacks::onPrepared(int32_t streamId)
{
  UNUSED(streamId);
  return Status::ok(); /* TODO: Implement */
}

Status CameraDeviceCallbacks::onRepeatingRequestError(int64_t lastFrameNumber,
    int32_t repeatingRequestId)
{
  UNUSED(lastFrameNumber);
  UNUSED(repeatingRequestId);
  return Status::ok(); /* TODO: Implement */
}

Status CameraDeviceCallbacks::onRequestQueueEmpty()
{
  return Status::ok(); /* TODO: Implement */
}
