#include "tusb.h"

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0210, // USB 2.10
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCafe,
    .idProduct          = 0x4900, // New PID for clean start
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
// Configuration Descriptor
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
// String Descriptors
//--------------------------------------------------------------------+
char const* string_desc_arr [] = {
    (const char[]) { 0x09, 0x04 }, 
    "TinyUSB",                     
    "Pico Bulk Echo",       
    "123456",                      
    "MSFT100\x01\x01", // 4: MS OS 1.0 Index 0xEE (Signature + VendorCode)
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
        // 特別處理 Windows MS OS 1.0 請求
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
// BOS Descriptor (Precision Aligned)
//--------------------------------------------------------------------+
#define MS_OS_20_DESC_LEN  0xB2
#define VENDOR_REQUEST_MS  0x01

uint8_t const desc_bos[] = {
    // BOS Header
    5, TUSB_DESC_BOS, 0x21, 0x00, 1, 
    // Platform Capability Descriptor (28 bytes)
    28, TUSB_DESC_DEVICE_CAPABILITY, 0x05, 0x00,
    0x67, 0x39, 0xDD, 0xD8, 0x10, 0xA3, 0xFA, 0x42, 0x95, 0x62, 0x37, 0x3D, 0x7E, 0x7A, 0x5D, 0x7C,
    0x00, 0x00, 0x03, 0x06, // Windows Version
    MS_OS_20_DESC_LEN, 0x00, // Descriptor Set Length
    VENDOR_REQUEST_MS, 0x00  // Vendor Code, Alt Enum
};

uint8_t const * tud_descriptor_bos_cb(void) {
    return desc_bos;
}

//--------------------------------------------------------------------+
// MS OS 2.0 Descriptor Set
//--------------------------------------------------------------------+
uint8_t const desc_ms_os_20[] = {
    0x0A, 0x00, MS_OS_20_SET_HEADER_DESCRIPTOR, 0x00, 0x00, 0x03, 0x06, 0x00, 0xB2, 0x00,
    0x08, 0x00, MS_OS_20_SUBSET_HEADER_CONFIGURATION, 0x00, 0x00, 0x00, 0xA8, 0x00,
    0x08, 0x00, MS_OS_20_SUBSET_HEADER_FUNCTION, 0x00, 0x00, 0xA0, 0x00,
    0x14, 0x00, MS_OS_20_FEATURE_COMPATBLE_ID, 'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x84, 0x00, MS_OS_20_FEATURE_REG_PROPERTY, 0x01, 0x00, 0x28, 0x00,
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00,
    'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00,
    0x50, 0x00,
    '{', 0x00, '9', 0x00, '7', 0x00, '5', 0x00, '2', 0x00, '4', 0x00, 'A', 0x00, 'D', 0x00, 'E', 0x00, '-', 0x00,
    'A', 0x00, 'B', 0x00, 'C', 0x00, 'E', 0x00, '-', 0x00, '4', 0x00, '1', 0x00, '7', 0x00, 'D', 0x00, '-', 0x00,
    '9', 0x00, 'E', 0x00, '3', 0x00, '5', 0x00, '-', 0x00, 'C', 0x00, 'B', 0x00, 'F', 0x00, 'B', 0x00, '5', 0x00,
    'B', 0x00, '1', 0x00, 'D', 0x00, '0', 0x00, 'F', 0x00, '9', 0x00, '7', 0x00, '}', 0x00, 0x00, 0x00
};

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request) {
    if (stage != CONTROL_STAGE_SETUP) return true;
    if (request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR) {
        if (request->bRequest == VENDOR_REQUEST_MS) {
            if (request->wIndex == 7) {
                return tud_control_xfer(rhport, request, (void*) (uintptr_t) desc_ms_os_20, sizeof(desc_ms_os_20));
            }
        }
    }
    return false;
}
