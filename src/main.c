#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "tusb.h"

int main(void) {
    set_sys_clock_khz(125000, true);
    tusb_init();
    static uint8_t buf[256];
    while (1) {
        tud_task();
        if (tud_mounted()) {
            if (tud_vendor_available()) {
                uint32_t count = tud_vendor_read(buf, sizeof(buf));
                if (count > 0) {
                    for(uint32_t i=0; i<count; i++) {
                        if(buf[i] >= 'a' && buf[i] <= 'z') buf[i] -= 32;
                    }
                    while (tud_vendor_write_available() < count) {
                        tud_task();
                    }
                    tud_vendor_write(buf, count);
                    tud_vendor_write_flush();
                }
            }
        }
    }
    return 0;
}
void tud_mount_cb(void) { tud_vendor_read_flush(); }
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) { (void) itf; (void) dtr; (void) rts; }
void tud_cdc_rx_cb(uint8_t itf) { (void) itf; }
