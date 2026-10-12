// Original: d:\build\custom\temp\code\client\device\device.h
#ifndef PG_CLIENT_DEVICE_DEVICE_H
#define PG_CLIENT_DEVICE_DEVICE_H

class WVideoDev;
class WInputDev;

// The DLL's device registry (device.cpp): the device implementations add
// themselves; WEnumVideoDevices / WEnumInputDevices hand them out by index.
void AddVideoDevice(WVideoDev *video);
void AddInputDevice(WInputDev *input);

#endif // PG_CLIENT_DEVICE_DEVICE_H
