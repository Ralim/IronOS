/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "bflb_platform.h"
#include "hal_gpio.h"
#include "hal_dma.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static bool touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
//static bool touchpad_is_pressed(void);
static lv_coord_t touchpad_get_xy(lv_coord_t *x, lv_coord_t *y);

// static void mouse_init(void);
// static bool mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
// static bool mouse_is_pressed(void);
// static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static bool keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static uint32_t keypad_get_key(void);

// static void encoder_init(void);
// static bool encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
// static void encoder_handler(void);

// static void button_init(void);
// static bool button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
// static int8_t button_get_pressed_id(void);
// static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t *indev_touchpad;
//lv_indev_t * indev_mouse;
lv_indev_t *indev_keypad;
//lv_indev_t * indev_encoder;
//lv_indev_t * indev_button;

// static int32_t encoder_diff;
// static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /* Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);

    // lv_obj_t *touchpad_cursor = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    // lv_img_set_src(touchpad_cursor, LV_SYMBOL_USB);
    // lv_indev_set_cursor(indev_touchpad, touchpad_cursor);

    /*------------------
     * Mouse
     * -----------------*/

    /*Initialize your touchpad if you have*/
    // mouse_init();

    /*Register a mouse input device*/
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_POINTER;
    // indev_drv.read_cb = mouse_read;
    // indev_mouse = lv_indev_drv_register(&indev_drv);

    /*Set cursor. For simplicity set a HOME symbol now.*/
    // lv_obj_t * mouse_cursor = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    // lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
    // lv_indev_set_cursor(indev_mouse, mouse_cursor);

    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv);

    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_keypad, group);` */

    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    // encoder_init();

    /*Register a encoder input device*/
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_ENCODER;
    // indev_drv.read_cb = encoder_read;
    // indev_encoder = lv_indev_drv_register(&indev_drv);

    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_encoder, group);` */

    /*------------------
     * Button
     * -----------------*/

    /*Initialize your button if you have*/
    // button_init();

    /*Register a button input device*/
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_BUTTON;
    // indev_drv.read_cb = button_read;
    // indev_button = lv_indev_drv_register(&indev_drv);

    /*Assign buttons to points on the screen*/
    // static const lv_point_t btn_points[2] = {
    //         {10, 10},   /*Button 0 -> x:10; y:10*/
    //         {40, 100},  /*Button 1 -> x:40; y:100*/
    // };
    // lv_indev_set_button_points(indev_button, btn_points);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/
#include "touch.h"
#include "hal_spi.h"

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
    if (device_find("lcd_dev_ifs") == NULL) {
        xpt2046_init();
    } else {
        gpio_set_mode(TOUCH_PIN_CS, GPIO_OUTPUT_MODE);
        gpio_write(TOUCH_PIN_CS, 1);
        touch_spi = device_find("lcd_dev_ifs");
    }
}

/* Will be called by the library to read the touchpad */
static bool touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    lv_coord_t xt, yt;

    /*Save the pressed coordinates and the state*/
    if (touchpad_get_xy(&xt, &yt)) {
        last_x = xt;
        last_y = yt;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

/*Return true is the touchpad is pressed*/
// static bool touchpad_is_pressed(void)
// {
//     /*Your code comes here*/
//     return false;
// }

/*Get the x and y coordinates if the touchpad is pressed*/
static lv_coord_t touchpad_get_xy(lv_coord_t *x, lv_coord_t *y)
{
    /*Your code comes here*/
    lv_disp_t *p_disp_drv_cb;
    uint8_t res;

    p_disp_drv_cb = lv_disp_get_default();

    while (p_disp_drv_cb->driver.buffer->flushing)
        ;

    uint32_t spi_clock = SPI_DEV(touch_spi)->clk;

    device_control(touch_spi, DEVICE_CTRL_SPI_CONFIG_CLOCK, (void *)360000);
    res = touch_read(x, y);
    device_control(touch_spi, DEVICE_CTRL_SPI_CONFIG_CLOCK, (void *)spi_clock);
    return res;
}

/*------------------
 * Mouse
 * -----------------*/

/* Initialize your mouse */
// static void mouse_init(void)
// {
//     /*Your code comes here*/
// }

/* Will be called by the library to read the mouse */
// static bool mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
// {
//     /*Get the current x and y coordinates*/
//     mouse_get_xy(&data->point.x, &data->point.y);

//     /*Get whether the mouse button is pressed or released*/
//     if(mouse_is_pressed()) {
//         data->state = LV_INDEV_STATE_PR;
//     } else {
//         data->state = LV_INDEV_STATE_REL;
//     }

//     /*Return `false` because we are not buffering and no more data to read*/
//     return false;
// }

/*Return true is the mouse button is pressed*/
// static bool mouse_is_pressed(void)
// {
//     /*Your code comes here*/

//     return false;
// }

/*Get the x and y coordinates if the mouse is pressed*/
// static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
// {
//     /*Your code comes here*/

//     (*x) = 0;
//     (*y) = 0;
// }

/*------------------
 * Keypad
 * -----------------*/
#include "hal_adc.h"
#include "hal_gpio.h"
uint8_t PinList[] = { GPIO_PIN_18 };
adc_channel_t posChList[] = { ADC_CHANNEL8 };
adc_channel_t negChList[] = { ADC_CHANNEL_GND };

adc_channel_val_t result_val;
struct device *adc_key;

uint16_t key_value[] = { 283, 89, 198, 0, 406 };

/* Initialize your keypad */
static void keypad_init(void)
{
    /*Your code comes here*/
    adc_channel_cfg_t adc_channel_cfg;

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    adc_register(ADC0_INDEX, "adc_key");

    adc_key = device_find("adc_key");

    if (adc_key) {
        ADC_DEV(adc_key)->continuous_conv_mode = ENABLE;
        device_open(adc_key, DEVICE_OFLAG_STREAM_RX);
        device_control(adc_key, DEVICE_CTRL_ADC_CHANNEL_CONFIG, &adc_channel_cfg);
        adc_channel_start(adc_key);
    }
}

/* Will be called by the library to read the keypad */
static bool keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    //mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();

    if (act_key != 0) {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch (act_key) {
            case 1:
                act_key = LV_KEY_LEFT;
                break;

            case 2:
                act_key = LV_KEY_RIGHT;
                break;

            case 3:
                act_key = LV_KEY_UP;
                break;

            case 4:
                act_key = LV_KEY_DOWN;
                break;

            case 5:
                act_key = LV_KEY_ENTER;
                break;

            default:
                break;
        }

        last_key = act_key;

    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    static uint8_t old_key_v = 0;
    static uint8_t old_key_num = 0;
    static uint8_t last_key;
    uint8_t key;
    uint16_t key_voltage;
    /*Your code comes here*/
    device_read(adc_key, 0, (void *)&result_val, sizeof(result_val) / sizeof(adc_channel_val_t));
    key_voltage = result_val.volt * 1000;

    for (key = 0; key < sizeof(key_value) / sizeof(key_value[0]); key++) {
        if (DIFF(key_voltage, key_value[key]) < KEY_ADC_DIFF_MAX) {
            break;
        }
    }

    key += 1;

    if (key > sizeof(key_value)) {
        key = 0;
    }

    if (key == last_key) {
        old_key_v = key;
        old_key_num = 0;
    } else if (key == old_key_v) {
        old_key_num++;

        if (old_key_num >= KEY_NOISE_NUM_MAX) {
            last_key = key;
            old_key_num = 0;
        }
    } else {
        old_key_num = 0;
        old_key_v = key;
    }

    return last_key;
}

/*------------------
 * Encoder
 * -----------------*/

/* Initialize your keypad */
// static void encoder_init(void)
// {
//     /*Your code comes here*/
// }

/* Will be called by the library to read the encoder */
// static bool encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
// {

//     data->enc_diff = encoder_diff;
//     data->state = encoder_state;

//     /*Return `false` because we are not buffering and no more data to read*/
//     return false;
// }

/*Call this function in an interrupt to process encoder events (turn, press)*/
// static void encoder_handler(void)
// {
//     /*Your code comes here*/

//     encoder_diff += 0;
//     encoder_state = LV_INDEV_STATE_REL;
// }

/*------------------
 * Button
 * -----------------*/

/* Initialize your buttons */
// static void button_init(void)
// {
//     /*Your code comes here*/
// }

/* Will be called by the library to read the button */
// static bool button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
// {

//     static uint8_t last_btn = 0;

//     /*Get the pressed button's ID*/
//     int8_t btn_act = button_get_pressed_id();

//     if(btn_act >= 0) {
//         data->state = LV_INDEV_STATE_PR;
//         last_btn = btn_act;
//     } else {
//         data->state = LV_INDEV_STATE_REL;
//     }

//     /*Save the last pressed button's ID*/
//     data->btn_id = last_btn;

//     /*Return `false` because we are not buffering and no more data to read*/
//     return false;
// }

/*Get ID  (0, 1, 2 ..) of the pressed button*/
// static int8_t button_get_pressed_id(void)
// {
//     uint8_t i;

//     /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
//     for(i = 0; i < 2; i++) {
//         /*Return the pressed button's ID*/
//         if(button_is_pressed(i)) {
//             return i;
//         }
//     }

//     /*No button pressed*/
//     return -1;
// }

/*Test if `id` button is pressed or not*/
// static bool button_is_pressed(uint8_t id)
// {

//     /*Your code comes here*/

//     return false;
// }

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
