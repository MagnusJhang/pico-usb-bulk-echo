#include "tusb.h"
tusb_desc_device_t const desc_device = {
    .bLength = 18, .bDescriptorType = 1, .bcdUSB = 0x0200, 
    .bDeviceClass = 0x00, .bDeviceSubClass = 0x00, .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64, .idVendor = 0xCafe, .idProduct = 0x4888, 
    .bcdDevice = 0x0100, .iManufacturer = 1, .iProduct = 2, .iSerialNumber = 3, .bNumConfigurations = 1
};
uint8_t const * tud_descriptor_device_cb(void) { return (uint8_t const *) &desc_device; }
uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN, 0, 100),
    TUD_VENDOR_DESCRIPTOR(0, 0, 0x01, 0x81, 64)
};
uint8_t const * tud_descriptor_configuration_cb(uint8_t index) { (void) index; return desc_configuration; }
char const* string_desc_arr [] = { (const char[]) { 0x09, 0x04 }, "TinyUSB", "Pico Echo", "123456" };
static uint16_t _desc_str[32];
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid; uint8_t chr_count;
    if ( index == 0) { memcpy(&_desc_str[1], string_desc_arr[0], 2); chr_count = 1; }
    else {
        if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;
        const char* str = string_desc_arr[index]; chr_count = (uint8_t) strlen(str);
        if ( chr_count > 31 ) chr_count = 31;
        for(uint8_t i=0; i<chr_count; i++) _desc_str[1+i] = str[i];
    }
    _desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8 ) | (2*chr_count + 2));
    return _desc_str;
}
