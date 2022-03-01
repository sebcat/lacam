svc = lacam.get_service()
device = svc:connect_device("0", "me?", "wut", lacam.USE_CALLING_UID)
print(device)
