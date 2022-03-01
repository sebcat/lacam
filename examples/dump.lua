svc = lacam.get_service()
vtd = svc:get_camera_vendor_tag_descriptor()
vtd:set_as_global()
vtc = svc:get_camera_vendor_tag_cache()
vtc:set_as_global()
print(" === VENDOR TAG DESCRIPTOR DUMP === ")
vtd:dump(1,2,2)
print(" === VENDOR TAG CACHE DUMP === ")
vtc:dump(1,2,2)
ncams = svc:get_number_of_cameras(lacam.CAMERA_TYPE_ALL)
for i=0,ncams-1 do
  print(string.format(" === DUMP CAMERA %d === ", i))
  metadata = svc:get_camera_characteristics(i)
  metadata:dump(1,2,2)
end

