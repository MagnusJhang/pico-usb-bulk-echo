#include "tusb.h"

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0210, 
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCafe,
    .idProduct          = 0x4910, // Brand new PID
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor (No IAD, back to pure Vendor)
//--------------------------------------------------------------------+
uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN, 0, 100),
    TUD_VENDOR_DESCRIPTOR(0, 0, 0x01, 0x81, 64)
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
    (void) index;
    return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors (MS OS 1.0 Support)
//--------------------------------------------------------------------+
char const* string_desc_arr [] = {
    (const char[]) { 0x09, 0x04 }, 
    "TinyUSB",                     
    "Pico Bulk Echo",       
    "123456",                      
    "MSFT100\x01\x01", // MS OS 1.0 Index 0xEE (WinUSB Trigger)
};

static uint16_t _desc_str[32];
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;
    uint8_t chr_count;
    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } 
    else if (index == 0xEE) {
        const char* str = string_desc_arr[4];
        chr_count = strlen(str);
        for (uint8_t i=0; i<chr_count; i++) _desc_str[1+i] = str[i];
    }
    else {
        if (!(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0]))) return NULL;
        const char* str = string_desc_arr[index];
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31;
        for (uint8_t i=0; i<chr_count; i++) _desc_str[1+i] = str[i];
    }
    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
    return _desc_str;
}

//--------------------------------------------------------------------+
// BOS & MS OS 2.0 (Verified Byte-by-Byte)
//--------------------------------------------------------------------+
#define VENDOR_REQUEST_MS 0x01

uint8_t const desc_ms_os_20[] = {
  // Set header: size=10, type=MS_OS_20_SET_HEADER_DESCRIPTOR, windows_version=0x06030000, total_len=162
  0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0xA2, 0x00,
  // Configuration subset header: size=8, type=MS_OS_20_SUBSET_HEADER_CONFIGURATION, length=152
  0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x98, 0x00,
  // Function subset header: size=8, type=MS_OS_20_SUBSET_HEADER_FUNCTION, interface=0, length=144
  0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90, 0x00,
  // Compatible ID: size=20, type=MS_OS_20_FEATURE_COMPATBLE_ID, ID=WINUSB
  0x14, 0x00, 0x03, 0x00, 'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // Registry property: size=124, type=MS_OS_20_FEATURE_REG_PROPERTY, data_type=REG_SZ, name_len=42, name="DeviceInterfaceGUID"
  0x7C, 0x00, 0x04, 0x00, 0x01, 0x00, 0x2A, 0x00,
  'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0, 'G', 0, 'U', 0, 'I', 0, 'D', 0, 0, 0,
  0x50, 0x00,
  '{', 0, '9', 0, '7', 0, '5', 0, '2', 0, '4', 0, 'A', 0, 'D', 0, 'E', 0, '-', 0, 'A', 0, 'B', 0, 'C', 0, 'E', 0, '-', 0, '4', 0, '1', 0, '7', 0, 'D', 0, '-', 0, '9', 0, 'E', 0, '3', 0, '5', 0, '-', 0, 'C', 0, 'B', 0, 'F', 0, 'B', 0, '5', 0, 'B', 0, '1', 0, 'D', 0, '0', 0, 'F', 0, '9', 0, '7', 0, '}', 0, 0, 0
};

uint8_t const desc_bos[] = {
    5, TUSB_DESC_BOS, 33, 0x00, 1, 
    28, TUSB_DESC_DEVICE_CAPABILITY, 0x05, 0x00,
    0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C, 0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F,
    0x00, 0x00, 0x03, 0x06, sizeof(desc_ms_os_20), 0x00, VENDOR_REQUEST_MS, 0x00
};

uint8_t const * tud_descriptor_bos_cb(void) { return desc_bos; }

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request) {
    if (stage != CONTROL_STAGE_SETUP) return true;
    if (request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR && 
        request->bRequest == VENDOR_REQUEST_MS && request->wIndex == 7) {
        return tud_control_xfer(rhport, request, (void*) (uintptr_t) desc_ms_os_20, sizeof(desc_ms_os_20));
    }
    return false;
}
