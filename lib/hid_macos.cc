#include "hid_macos.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


HIDevice::HIDevice(int vid, int pid, QObject *parent)
  : QObject(parent), _dev(nullptr)
{
  // Create the USB HID Manager.
  _HIDManager = IOHIDManagerCreate(kCFAllocatorDefault,
                                   kIOHIDOptionsTypeNone);

  // Create an empty matching dictionary for filtering USB devices in our HID manager.
  CFMutableDictionaryRef matchDict = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

  // Specify the USB device manufacturer and product in our matching dictionary.
  CFDictionarySetValue(matchDict, CFSTR(kIOHIDVendorIDKey), CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &vid));
  CFDictionarySetValue(matchDict, CFSTR(kIOHIDProductIDKey), CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &pid));

  // Apply the matching to our HID manager.
  IOHIDManagerSetDeviceMatching(_HIDManager, matchDict);
  CFRelease(matchDict);

  // The HID manager will use callbacks when specified USB devices are connected/disconnected.
  IOHIDManagerRegisterDeviceMatchingCallback(_HIDManager, &callback_open, this);
  IOHIDManagerRegisterDeviceRemovalCallback(_HIDManager, &callback_close, this);

  // Add the HID manager to the main run loop
  IOHIDManagerScheduleWithRunLoop(_HIDManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);

  // Open the HID mangager
  IOReturn IOReturn = IOHIDManagerOpen(_HIDManager, kIOHIDOptionsTypeNone);
  if (IOReturn != kIOReturnSuccess) {
    _errorMessage = QString("%1: Cannot find USB device %2:%3").arg(__func__).arg(vid).arg(pid);
    IOHIDManagerUnscheduleFromRunLoop(_HIDManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
    _HIDManager = nullptr;
    return;
  }

  // Run main application loop until device found.
  int k;
  for (k=0; ; k++) {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, 0);
    if (_dev)
      return;

    if (k >= 3) {
      _errorMessage = QString("Cannot find USB device %1:%2").arg(vid).arg(pid);
      IOHIDManagerUnscheduleFromRunLoop(_HIDManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
      IOHIDManagerClose(_HIDManager, kIOHIDOptionsTypeNone);
      _HIDManager = nullptr;
      return;
    }
    usleep(10000);
  }
}

HIDevice::~HIDevice() {
  if (_dev)
    close();
  if (_HIDManager) {
    IOHIDManagerUnscheduleFromRunLoop(_HIDManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
    IOHIDManagerClose(_HIDManager, kIOHIDOptionsTypeNone);
  }
}
bool
HIDevice::isOpen() const {
  return nullptr != _dev;
}

//
// Send a request to the device.
// Store the reply into the rdata[] array.
// Terminate in case of errors.
//
bool
HIDevice::hid_send_recv(const unsigned char *data, unsigned nbytes, unsigned char *rdata, unsigned rlength)
{
  unsigned char buf[42];
  unsigned k;
  IOReturn result;

  memset(buf, 0, sizeof(buf));
  buf[0] = 1;
  buf[1] = 0;
  buf[2] = nbytes;
  buf[3] = nbytes >> 8;
  if (nbytes > 0)
    memcpy(buf+4, data, nbytes);
  nbytes += 4;

  _nbytes_received = 0;
  memset(_receive_buf, 0, sizeof(_receive_buf));
again:
  // Write to HID device.
  result = IOHIDDeviceSetReport(_dev, kIOHIDReportTypeOutput, 0, buf, sizeof(buf));
  if (result != kIOReturnSuccess) {
    _errorMessage = QString("%1: HID output error: %2!").arg(__func__).arg(result);
    fprintf(stderr, "HID output error: %d!\n", result);
    return false;
  }

  // Run main application loop until reply received.
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, 0);
  for (k = 0; _nbytes_received <= 0; k++) {
    usleep(100);
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, 0);
    if (k >= 1000) {
      goto again;
    }
  }

  if (_nbytes_received != sizeof(_receive_buf)) {
    _errorMessage = QString("%1: Short read: %2 bytes instead of %3!").arg(__func__).arg(result)
        .arg(_nbytes_received).arg((int)sizeof(_receive_buf));
    return false;
  }
  if ((_receive_buf[0] != 3) || (_receive_buf[1] != 0) || (_receive_buf[3] != 0)) {
    _errorMessage = QString("%1: Incorrect reply!").arg(__func__);
    return false;
  }
  if (_receive_buf[2] != rlength) {
    _errorMessage = QString("%1: Incorrect reply length %2, expected %3!").arg(__func__)
        .arg(_receive_buf[2]).arg(rlength);
    return false;
  }
  memcpy(rdata, _receive_buf+4, rlength);
  return true;
}

//
// Callback: data is received from the HID device
//
void
HIDevice::callback_input(void *context, IOReturn result, void *sender, IOHIDReportType type,
                         uint32_t reportID, uint8_t *data, CFIndex nbytes)
{
  Q_UNUSED(sender);
  Q_UNUSED(type);
  Q_UNUSED(reportID);

  HIDevice *self = reinterpret_cast<HIDevice *>(context);

  if (result != kIOReturnSuccess) {
    self->_errorMessage = QString("%1: HID input error: %2!").arg(__func__).arg(result);
    self->close();
    return;
  }

  if (nbytes > CFIndex(sizeof(self->_receive_buf))) {
    self->_errorMessage = QString("%1: Too large HID input: %d bytes!").arg(__func__).arg((int)nbytes);
    self->close();
    return;
  }

  self->_nbytes_received = nbytes;
  if (nbytes > 0)
    memcpy(self->_receive_buf, data, nbytes);
}

//
// Callback: device specified in the matching dictionary has been added
//
void
HIDevice::callback_open(void *context, IOReturn result, void *sender, IOHIDDeviceRef deviceRef)
{
  Q_UNUSED(result);
  Q_UNUSED(sender);

  HIDevice *self = reinterpret_cast<HIDevice *>(context);

  IOReturn o = IOHIDDeviceOpen(deviceRef, kIOHIDOptionsTypeSeizeDevice);
  if (o != kIOReturnSuccess) {
    self->_errorMessage = QString("%1: Cannot open HID device!").arg(__func__);
    return;
  }

  // Register input callback.
  IOHIDDeviceRegisterInputReportCallback(deviceRef, self->_transfer_buf, sizeof(self->_transfer_buf),
                                         callback_input, self);

  self->_dev = deviceRef;
}

//
// Callback: device specified in the matching dictionary has been removed
//
void
HIDevice::callback_close(void *context, IOReturn result, void *sender, IOHIDDeviceRef deviceRef)
{
  Q_UNUSED(result);
  Q_UNUSED(sender);

  HIDevice *self = reinterpret_cast<HIDevice *>(context);

  // De-register input callback.
  IOHIDDeviceRegisterInputReportCallback(
        deviceRef, self->_transfer_buf, sizeof(self->_transfer_buf), NULL, NULL);
}

//
// Close HID device.
//
void
HIDevice::close()
{
  if (! _dev)
    return;

  IOHIDDeviceClose(_dev, kIOHIDOptionsTypeNone);
  _dev = nullptr;
}

