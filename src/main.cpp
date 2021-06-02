#include <Arduino.h>
#include "esp_heap_caps.h"
#include "epd_highlevel.h"
#include "esp_sleep.h"

// fonts
#include "opensans8b.h"
#include "opensans12b.h"
#include "sonnets.h"

// battery

#define WAVEFORM EPD_BUILTIN_WAVEFORM
#define STATUS_INFO // Display Battery % et bootCount

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
int64_t sleepTime = 1;          // in minutes

#ifdef STATUS_INFO
    #include <driver/adc.h>
    #include "esp_adc_cal.h"
    #define BATT_PIN 36
    int vref = 1100;                // ? for battery ?
    double_t battery;
    double_t get_battery_percentage()
{
    uint16_t v = analogRead(BATT_PIN);
    double_t battery_voltage = ((double_t)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);

    // Better formula needed I suppose
    // experimental super simple percent estimate no lookup anything just divide by 100
    double_t percent_experiment = ((battery_voltage - 3.7) / 0.5) * 100;

    // cap out battery at 100%
    // on charging it spikes higher
    if (percent_experiment > 100) {
        percent_experiment = 100;
    }
    return percent_experiment;
}
/**
 * Correct the ADC reference voltage. Was in example of lilygo epd47 repository to calc battery percentage
*/
void correct_adc_reference()
{
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        vref = adc_chars.vref;
    }
}

#endif

void display_full_screen_left_aligned_text(const char* text,const char* auteur,const char* titre) {
    EpdFontProperties font_props = epd_font_properties_default();
    font_props.flags = EPD_DRAW_ALIGN_LEFT;

    epd_hl_set_all_white(&hl); // first set full screen to white

    int cursor_x = 150;
    int cursor_y = 150;
    epd_write_string(&OpenSans12B, titre, &cursor_x, &cursor_y, fb, &font_props);
    cursor_x = 35;
    cursor_y = cursor_y + 50;
    epd_write_string(&OpenSans8B, text, &cursor_x, &cursor_y, fb, &font_props);
    cursor_x = 250;
    cursor_y = cursor_y + 50;
    epd_write_string(&OpenSans8B, auteur, &cursor_x, &cursor_y, fb, &font_props);

    #ifdef STATUS_INFO
        /************ Battery percentage display ****************/
        cursor_x = 30;
        cursor_y = cursor_y + 50;
        String status = "Batterie : " + String(battery) + " / #Boots :" + String(bootCount);
        epd_write_string(&OpenSans8B, status.c_str(), &cursor_x, &cursor_y, fb, &font_props);
    #endif
    epd_poweron();
    err = epd_hl_update_screen(&hl, MODE_GC16, temperature);
    delay(1500);
    epd_poweroff();
    delay(1000);
}

void setup() {
    bootCount++;

    epd_init(EPD_OPTIONS_DEFAULT);
    hl = epd_hl_init(WAVEFORM);
    epd_set_rotation(orientation);
    fb = epd_hl_get_framebuffer(&hl);
    epd_poweron();
    epd_clear();
    
#ifdef STATUS_INFO
    correct_adc_reference();
    battery  = get_battery_percentage();
#endif
    epd_poweroff();

    Sonnet monSonnet;
    monSonnet = Sonnets[bootCount%nbrSonnets]; 
    display_full_screen_left_aligned_text(monSonnet.texte,monSonnet.auteur,monSonnet.titre);

    esp_sleep_enable_ext0_wakeup(FIRST_BTN_PIN, 0);
    esp_sleep_enable_timer_wakeup(sleepTime * 60 * 1000000ULL);
    esp_deep_sleep_start();
}

void loop()
{
}
