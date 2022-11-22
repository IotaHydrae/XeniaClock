/**
 * @file main.c
 * @author IotaHydrae (writeforever@foxmail.com)
 * @brief file of the epink-1.54.
 * @version 0.2
 * @date 2022-08-14
 *
 * Hi, guys!
 *
 * Special thanks to :
 *   Raspberry Pi () -- MCU
 *   LuatOS (https://gitee.com/openLuat/LuatOS) -- Display module
 *   Espressif (https://github.com/espressif) -- Net module
 *
 * MIT License
 *
 * Copyright (c) 2022 IotaHydrae(writeforever@foxmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *
 */


#include <stdio.h>
#include <string.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/types.h"
#include "pico/platform.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"

/* Header files we defined */
#include "common/tools.h"
#include "common/vals.h"
#include "display/epd.h"
#include "rtc/native_rtc.h"
#include "sensors/aht10.h"
#include "net/esp01s.h"

/* Header files lvgl defined */
#include "port/lv_port_disp.h"
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/src/extra/libs/qrcode/lv_qrcode.h"
#include "ui/ui.h"
#include "ui/ui_comp.h"

/**
 * @brief hardware layer initialize
 * for each platform, do it's iomux and pinctl here
 */
static void hal_init(void)
{
#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) || !defined(EPINK_CS_PIN)
#warning spi/bme280_spi example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else
    /* Useing default SPI0 at 50MHz */
    spi_init(spi_default, DEFAULT_SPI_SPEED);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    /* could check this using `picotools` */
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN,
                               GPIO_FUNC_SPI));

    gpio_init(EPINK_CS_PIN);
    gpio_set_dir(EPINK_CS_PIN, GPIO_OUT);
    gpio_put(EPINK_CS_PIN, 1);
    bi_decl(bi_1pin_with_name(EPINK_CS_PIN, "SPI CS"));

    gpio_init(EPINK_RES_PIN);
    gpio_set_dir(EPINK_RES_PIN, GPIO_OUT);

    gpio_init(EPINK_DC_PIN);
    gpio_set_dir(EPINK_DC_PIN, GPIO_OUT);

    gpio_init(EPINK_BUSY_PIN);
    gpio_set_dir(EPINK_BUSY_PIN, GPIO_IN);
#endif

    /* initialize i2c host */
    i2c_init(i2c_default, DEFAULT_I2C_SPEED);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    /* initialize uart */
    uart_init(uart1, DEFAULT_UART_SPEED);
    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);
}

static void sys_clk_init()
{

}

extern lv_obj_t *ui_RollerHour;
extern lv_obj_t *ui_RollerMinute;
extern lv_obj_t *ui_RollerHour;
extern lv_obj_t *ui_LabelDate;
extern lv_obj_t *ui_LabelTips;

/* Whew, we are getting off work! */
static uint8_t hour = 18;
static uint8_t minute = 30;
static uint8_t second = 0;

/* initialize all stuff with RTC releated */
static void native_rtc_init()
{
    datetime_t t;
    /* TODO: init rtc device */
    rtc_device_init();

    /* set a test time to device */
    // t.hour = 16;
    // t.min = 32;
    // t.sec = 30;
    // p_rtc_device_set_time(t);

    /* init rtc host in mcu */
    rtc_host_init();

    /* read RTC time from mcu */
    t = rtc_host_get_datetime();

    hour = t.hour;
    minute = t.min;
    second = t.sec;

    /* write back to lvgl */
    lv_roller_set_selected(ui_RollerHour, hour, LV_ANIM_OFF);
    lv_roller_set_selected(ui_RollerMinute, minute, LV_ANIM_OFF);
    lv_roller_set_selected(ui_RollerSecond, second, LV_ANIM_OFF);
}

/* This logic should never modified unless have a better idea
/* This callback must be use a  high-res timer to handle
/* Be sure to use a monospaced font
 */
static bool lv_timer_roller_time_cb(struct repeating_timer *t)
{
    lv_roller_set_selected(ui_RollerSecond, ++second, LV_ANIM_OFF);

    if (60 == second) {
        second = 0;     /* update first, then write back */
        lv_roller_set_selected(ui_RollerSecond, second, LV_ANIM_OFF);
        lv_roller_set_selected(ui_RollerMinute, ++minute, LV_ANIM_OFF);
    }

    if (60 == minute) {
        minute = 0;
        lv_roller_set_selected(ui_RollerMinute, minute, LV_ANIM_OFF);
        lv_roller_set_selected(ui_RollerHour, ++hour, LV_ANIM_OFF);
    }

    if (24 == hour) {
        hour = 0;
        lv_roller_set_selected(ui_RollerHour, hour, LV_ANIM_OFF);
    }

    return true;
}

/* TODO: These tips should get from network */
static const char *g_tips[] = {
    "Stay Hungry, Stay Foolish.",
    "Learn to pause.",
    "Don't Worry, Be Happy.",
    "Do Androids Dream of Electric Sheep?",
    "Gah! YEEEEEEEE!",
    "Never laugh at live dragons.",
    "What are the characteristics of a person?",
    "You are not dead yet.",
    "Cheers!",
    "Sleep seven to eight hours per night.",
    "Keep company with good people.",
    "Avoid news overdose.",
    "Get regular exercise.",
    "Do something meaningful each day.",
    "Think good thoughts for others.",
    "Be humble and curious.",
    "I don't want to surive, I want to live!",
};
static uint8_t tips_index = 0;

static inline void lv_timer_label_tips_cb()
{
    lv_label_set_text(ui_LabelTips, g_tips[tips_index++]);
    if (tips_index > (ARRAY_SIZE(g_tips) - 1))
        tips_index = 0;
}

// extern lv_obj_t *ui_LabelBattery;
extern lv_obj_t *ui_PanelStatusBar;

/* TODO: Real battery percent detect */
static inline void lv_timer_battery_cb()
{
    lv_obj_t *ui_LabelBattery = ui_comp_get_child(ui_PanelStatusBar,
                                                  UI_COMP_PANELSTATUSBAR_LABELBATTERY);

    static uint8_t battery_percent = 100;
    lv_label_set_text_fmt(ui_LabelBattery, "%d%%", --battery_percent);

    if (battery_percent == 0)
        battery_percent = 100;
}

extern lv_obj_t *ui_LabelTemperture;
extern lv_obj_t *ui_LabelHumidity;

static inline void lv_timer_temp_humid_cb()
{
    struct aht10_data data = aht10_read_humidity_temperture();

    lv_label_set_text_fmt(ui_LabelHumidity, "%d%%", (int)data.real_humidity);
    lv_label_set_text_fmt(ui_LabelTemperture, "%d*C", (int)data.real_temperture);
}

static inline bool lvgl_clock_cb(struct repeating_timer *t)
{
    lv_timer_handler();
    lv_tick_inc(5);
    return true;
}

static void post_timers_init()
{
    struct repeating_timer roller_timer;
    add_repeating_timer_ms(1000, lv_timer_roller_time_cb, NULL, &roller_timer);

    /* timer for updating daily tips, should requst tips from internet */
    lv_timer_t *timer_tips = lv_timer_create_basic();
    timer_tips->timer_cb = lv_timer_label_tips_cb;
    timer_tips->period = 10000;

    /* timer for updating battery percent, just a demo for now */
    lv_timer_t *timer_battery = lv_timer_create_basic();
    timer_battery->timer_cb = lv_timer_battery_cb;
    timer_battery->period = 2000;

    /* timer for updating temperture and humidity */
    lv_timer_t *timer_temp_humid = lv_timer_create_basic();
    timer_temp_humid->timer_cb = lv_timer_temp_humid_cb;
    timer_temp_humid->period = 2000;
}

static void network_config()
{
    const char *data = "https://192.168.4.1";

    /* make a QR code on it, (0, -5), 120x120 */
    lv_obj_t *qr_code = lv_qrcode_create(ui_ScreenEpinkConfig, 120, lv_color_hex(0x0),
                                         lv_color_hex(0xffffffff));
    lv_obj_align(qr_code, LV_ALIGN_CENTER, 0, -5);
    lv_qrcode_update(qr_code, data, strlen(data));
    lv_disp_load_scr(ui_ScreenEpinkConfig);

    /* lol, persent configurating */
    sleep_ms(3000);

    /* network configuration is okay, switch to home */
    lv_disp_load_scr(ui_ScreenEpinkHome);
}

int main(void)
{
    stdio_init_all();
    /* system up hardware init */
    hal_init();

    /* lvgl init */
    struct repeating_timer lvgl_clock_timer;
    lv_init();
    lv_port_disp_init();
    /* start a timer for lvgl clock */
    add_repeating_timer_us(5000, lvgl_clock_cb, NULL, &lvgl_clock_timer);

    epink_blank();      /* a global reflush for E-paper is required */
    ui_init();          /* call qquareline project initialization process */

    network_config();

    native_rtc_init();  /* some post hardware init */
    post_timers_init();

    while (1) {
        tight_loop_contents();

        sleep_ms(3000);
        lv_disp_load_scr(ui_ScreenSleep);
        break;
    }

    while (1) {
        sleep_ms(3000);
        lv_disp_load_scr(ui_ScreenEpinkHome);
    }


    return 0;
}