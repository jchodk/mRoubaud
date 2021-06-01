#include <Arduino.h>
//#include "epd_driver.h"
#include "epd_highlevel.h"
#include "esp_sleep.h"
#include "opensans8b.h"
#include "opensans12b.h"
#include "sonnets.h"

#define WAVEFORM EPD_BUILTIN_WAVEFORM


/**
 * Upper most button on side of device. Used to setup as wakeup source to start from deepsleep.
 * Pinout here https://ae01.alicdn.com/kf/H133488d889bd4dd4942fbc1415e0deb1j.jpg
 */
gpio_num_t FIRST_BTN_PIN = GPIO_NUM_39;
EpdiyHighlevelState hl;
int temperature = 20;           // ambient temperature around device (fake value)
uint8_t *fb;
enum EpdDrawError err;
EpdRotation orientation = EPD_ROT_PORTRAIT;
RTC_DATA_ATTR int bootCount;    // RTC Memory preserved across deepsleeps
int64_t sleepTime = 60;          // in minutes

void display_full_screen_left_aligned_text(const char* text,const char* auteur,const char* titre) {
    EpdFontProperties font_props = epd_font_properties_default();
    font_props.flags = EPD_DRAW_ALIGN_LEFT;

    epd_hl_set_all_white(&hl); // first set full screen to white

    int cursor_x = 150;
    int cursor_y = 200;
    epd_write_string(&OpenSans12B, titre, &cursor_x, &cursor_y, fb, &font_props);
    cursor_x = 35;
    cursor_y = cursor_y + 50;
    epd_write_string(&OpenSans8B, text, &cursor_x, &cursor_y, fb, &font_props);
    cursor_x = 250;
    cursor_y = cursor_y + 50;
    epd_write_string(&OpenSans8B, auteur, &cursor_x, &cursor_y, fb, &font_props);
    epd_poweron();
    err = epd_hl_update_screen(&hl, MODE_GC16, temperature);
    delay(500);
    epd_poweroff();
    delay(1000);
}

void setup() {
    epd_init(EPD_OPTIONS_DEFAULT);
    hl = epd_hl_init(WAVEFORM);
    epd_set_rotation(orientation);
    fb = epd_hl_get_framebuffer(&hl);
    epd_clear();
    
    bootCount++;
    Sonnet monSonnet;
    monSonnet = Sonnets[bootCount%nbrSonnets]; // todo:
    display_full_screen_left_aligned_text(monSonnet.texte,monSonnet.auteur,monSonnet.titre);
    delay(1500);

    epd_poweroff();
    delay(400);
    esp_sleep_enable_ext0_wakeup(FIRST_BTN_PIN, 0);
    esp_sleep_enable_timer_wakeup(sleepTime * 60 * 1000000ULL);
    esp_deep_sleep_start();
}

void loop()
{
}
