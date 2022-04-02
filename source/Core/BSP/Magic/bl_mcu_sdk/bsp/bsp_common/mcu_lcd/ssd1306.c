#include "ssd1306.h"
#include "hal_i2c.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "font.h"
#include "bflb_platform.h"

#define SSD1306_USING_I2C 0
#define SSD1306_USING_SPI 1

#define INTERFACE_SELECT SSD1306_USING_SPI

static uint8_t OLED_GRAM[128][8];

struct device *ssd_1306;

static void ssd1306_write_byte(uint8_t data, uint8_t cmd)
{
#if INTERFACE_SELECT == SSD1306_USING_I2C
    i2c_msg_t msg;

    if (cmd) {
        msg.subaddr = 0x40;
    } else {
        msg.subaddr = 0x00;
    }

    msg.buf = &data;
    msg.flags = SUB_ADDR_1BYTE | I2C_WR;
    msg.len = 1;
    msg.slaveaddr = 0x3c;

    i2c_transfer(ssd_1306, &msg, 1);
#else
    if (cmd) {
        CS1_LOW;
        DC_HIGH;
    } else {
        CS1_LOW;
        DC_LOW;
    }

    spi_transmit(ssd_1306, &data, 1, SPI_TRANSFER_TYPE_8BIT);
    CS1_HIGH;
    DC_HIGH;
#endif
}

void ssd1306_init(void)
{
#if INTERFACE_SELECT == SSD1306_USING_I2C
    i2c_register(I2C0_INDEX, "lcd_dev_ifs");
    ssd_1306 = device_find("lcd_dev_ifs");

    if (ssd_1306) {
        device_open(ssd_1306, 0);
    }

    ssd1306_write_byte(0xAE, SSD1306_CMD); //--turn off oled panel
    ssd1306_write_byte(0x00, SSD1306_CMD); //---set low column address
    ssd1306_write_byte(0x10, SSD1306_CMD); //---set high column address
    ssd1306_write_byte(0x40, SSD1306_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    ssd1306_write_byte(0x81, SSD1306_CMD); //--set contrast control register
    ssd1306_write_byte(0xCF, SSD1306_CMD); // Set SEG Output Current Brightness
    ssd1306_write_byte(0xA1, SSD1306_CMD); //--Set SEG/Column Mapping
    ssd1306_write_byte(0xC0, SSD1306_CMD); //Set COM/Row Scan Direction
    ssd1306_write_byte(0xA6, SSD1306_CMD); //--set normal display
    ssd1306_write_byte(0xA8, SSD1306_CMD); //--set multiplex ratio(1 to 64)
    ssd1306_write_byte(0x3f, SSD1306_CMD); //--1/64 duty
    ssd1306_write_byte(0xD3, SSD1306_CMD); //-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    ssd1306_write_byte(0x00, SSD1306_CMD); //-not offset
    ssd1306_write_byte(0xd5, SSD1306_CMD); //--set display clock divide ratio/oscillator frequency
    ssd1306_write_byte(0x80, SSD1306_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
    ssd1306_write_byte(0xD9, SSD1306_CMD); //--set pre-charge period
    ssd1306_write_byte(0xF1, SSD1306_CMD); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    ssd1306_write_byte(0xDA, SSD1306_CMD); //--set com pins hardware configuration
    ssd1306_write_byte(0x12, SSD1306_CMD);
    ssd1306_write_byte(0xDB, SSD1306_CMD); //--set vcomh
    ssd1306_write_byte(0x40, SSD1306_CMD); //Set VCOM Deselect Level
    ssd1306_write_byte(0x20, SSD1306_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
    ssd1306_write_byte(0x02, SSD1306_CMD); //
    ssd1306_write_byte(0x8D, SSD1306_CMD); //--set Charge Pump enable/disable
    ssd1306_write_byte(0x14, SSD1306_CMD); //--set(0x10) disable
    ssd1306_write_byte(0xA4, SSD1306_CMD); // Disable Entire Display On (0xa4/0xa5)
    ssd1306_write_byte(0xA6, SSD1306_CMD); // Disable Inverse Display On (0xa6/a7)
    ssd1306_write_byte(0xAF, SSD1306_CMD); //--turn on oled panel

    ssd1306_display_on();
    ssd1306_clear_screen(0xff);
#else
    gpio_set_mode(OLED_CS_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(OLED_DC_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(OLED_RES_PIN, GPIO_OUTPUT_MODE);
    gpio_write(OLED_CS_PIN, 1); //CS1
    gpio_write(OLED_DC_PIN, 1); //DC

    spi_register(SPI0_INDEX, "lcd_dev_ifs");
    ssd_1306 = device_find("lcd_dev_ifs");

    if (ssd_1306) {
        device_open(ssd_1306, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
    }

    CS1_HIGH;
    DC_HIGH;
    RES_LOW;
    bflb_platform_delay_ms(100);
    RES_HIGH;

    ssd1306_write_byte(0xAE, SSD1306_CMD);
    ssd1306_write_byte(0xD5, SSD1306_CMD);
    ssd1306_write_byte(0x50, SSD1306_CMD);
    ssd1306_write_byte(0xA8, SSD1306_CMD);
    ssd1306_write_byte(0x3F, SSD1306_CMD);
    ssd1306_write_byte(0xD3, SSD1306_CMD);
    ssd1306_write_byte(0x00, SSD1306_CMD);
    ssd1306_write_byte(0x40, SSD1306_CMD);
    ssd1306_write_byte(0x8D, SSD1306_CMD);
    ssd1306_write_byte(0x14, SSD1306_CMD);
    ssd1306_write_byte(0x20, SSD1306_CMD);
    ssd1306_write_byte(0x02, SSD1306_CMD);
    ssd1306_write_byte(0xA1, SSD1306_CMD);
    ssd1306_write_byte(0xC0, SSD1306_CMD);
    ssd1306_write_byte(0xDA, SSD1306_CMD);
    ssd1306_write_byte(0x12, SSD1306_CMD);
    ssd1306_write_byte(0x81, SSD1306_CMD);
    ssd1306_write_byte(0xEF, SSD1306_CMD);
    ssd1306_write_byte(0xD9, SSD1306_CMD);
    ssd1306_write_byte(0xF1, SSD1306_CMD);
    ssd1306_write_byte(0xDB, SSD1306_CMD);
    ssd1306_write_byte(0x30, SSD1306_CMD);
    ssd1306_write_byte(0xA4, SSD1306_CMD);
    ssd1306_write_byte(0xA6, SSD1306_CMD);
    ssd1306_write_byte(0xAF, SSD1306_CMD);

    ssd1306_clear_screen(0);
#endif
}

void ssd1306_display_on(void)
{
    ssd1306_write_byte(0x8D, SSD1306_CMD);
    ssd1306_write_byte(0x14, SSD1306_CMD);
    ssd1306_write_byte(0xAF, SSD1306_CMD);
}

/**
  * @brief  OLED turns off
  *
  * @param  None
  *
  * @retval  None
**/
void ssd1306_display_off(void)
{
    ssd1306_write_byte(0x8D, SSD1306_CMD);
    ssd1306_write_byte(0x10, SSD1306_CMD);
    ssd1306_write_byte(0xAE, SSD1306_CMD);
}

void ssd1306_refresh_gram(void)
{
    uint8_t i, j;

    for (i = 0; i < 8; i++) {
        ssd1306_write_byte(0xB0 + i, SSD1306_CMD);
        ssd1306_write_byte(0x00, SSD1306_CMD);
        ssd1306_write_byte(0x10, SSD1306_CMD);
        for (j = 0; j < 128; j++) {
            ssd1306_write_byte(OLED_GRAM[j][i], SSD1306_DAT);
        }
    }
}

void ssd1306_clear_screen(uint8_t fill)
{
    memset(OLED_GRAM, fill, sizeof(OLED_GRAM));
    ssd1306_refresh_gram();
}

/**
  * @brief  Draws a piont on the screen
  *
  * @param  x: Specifies the X position
  * @param  y: Specifies the Y position
  * @param  mode: 0: the point turns off    1: the piont turns on
  *
  * @retval None
**/
void ssd1306_draw_point(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t i, j, temp;

    if (x > 127 || y > 63)
        return;

    i = 7 - y / 8;
    j = y % 8;
    temp = 0x01 << (7 - j);
    if (mode == 0)
        OLED_GRAM[x][i] &= ~temp;
    else
        OLED_GRAM[x][i] |= temp;
}

/**
  * @brief Displays one character at the specified position
  *
  * @param  x: Specifies the X position
  * @param  y: Specifies the Y position
  * @param  size:
  * @param  mode
  * @retval
**/
void ssd1306_display_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';

    for (t = 0; t < csize; t++) {
        switch (size) {
            case 12:
                temp = asc2_1206[chr][t];
                break;
            case 16:
                temp = asc2_1608[chr][t];
                break;
            case 24:
                temp = asc2_2412[chr][t];
                break;
            default:
                return;
        }

        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80)
                ssd1306_draw_point(x, y, mode);
            else
                ssd1306_draw_point(x, y, !mode);

            temp <<= 1;
            y++;

            if ((y - y0) == size) {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**
  * @brief  Displays a string on the screen
  *
  * @param  x: Specifies the X position
  * @param  y: Specifies the Y position
  * @param  string: Pointer to a string to display on the screen
  *
  * @retval  None
**/
void ssd1306_display_string(uint8_t x, uint8_t y, const char *string, uint8_t size, uint8_t mode)
{
    while (*string != '\0') {
        if (x > (SSD1306_WIDTH - size / 2)) {
            x = 0;
            y += size;
            if (y > (SSD1306_HEIGHT - size)) {
                y = x = 0;
                ssd1306_clear_screen(0x00);
            }
        }

        ssd1306_display_char(x, y, *string, size, mode);
        x += size / 2;
        string++;
    }
}

void ssd1306_display_chinese(uint8_t x, uint8_t y, uint8_t no, uint8_t mode)
{
    uint8_t temp1;
    uint8_t temp2;
    uint8_t t1, t2;
    uint8_t x0 = x, y0 = y;

    for (t1 = 0; t1 < 16; t1++) {
        temp1 = Hzk[2 * no][t1];
        for (t2 = 0; t2 < 8; t2++) {
            if (temp1 & 0x01)
                ssd1306_draw_point(x, y, mode);
            else
                ssd1306_draw_point(x, y, !mode);
            temp1 >>= 1;
            y++;
        }
        y = y0;
        x++;
    }

    x = x0;
    y = y0 + 8;
    y0 = y;

    for (t1 = 0; t1 < 16; t1++) {
        temp2 = Hzk[2 * no + 1][t1];
        for (t2 = 0; t2 < 8; t2++) {
            if (temp2 & 0x01)
                ssd1306_draw_point(x, y, mode);
            else
                ssd1306_draw_point(x, y, !mode);
            temp2 >>= 1;
            y++;
        }
        y = y0;
        x++;
    }
}

void ssd1306_draw_bmp(uint8_t x, uint8_t y, const unsigned char *image, uint8_t index, uint8_t mode)
{
    uint8_t temp, t1;
    uint16_t i;
    uint8_t y0 = y;

    for (i = 0; i < 1024; i++) {
        temp = image[1024 * index + i];
        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80)
                ssd1306_draw_point(x, y, mode);
            else
                ssd1306_draw_point(x, y, !mode);
            temp <<= 1;
            y++;
            if ((y - y0) == 64) {
                y = y0;
                x++;
                break;
            }
        }
    }
}

void ssd1306_draw_alpha(uint8_t x, uint8_t y, const unsigned char *image, uint8_t index, uint8_t mode)
{
    uint8_t temp, t1;
    uint16_t i;
    uint8_t y0 = y;

    for (i = 0; i < 1024; i++) {
        temp = image[1024 * index + i];
        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80)
                ssd1306_draw_point(x, y, mode);
            temp <<= 1;
            y++;
            if ((y - y0) == 64) {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**
  * @brief  Fills a rectangle
  *
  * @param  x1: Specifies the X position 1 (X top left position)
  * @param  y1: Specifies the Y position 1 (Y top left position)
  * @param  x2: Specifies the X position 2 (X bottom right position)
  * @param  y3: Specifies the Y position 2 (Y bottom right position)
  *
  * @retval
**/
void ssd1306_fill_screen(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t chDot)
{
    uint8_t x, y;

    for (x = x1; x <= x2; x++) {
        for (y = y1; y <= y2; y++) {
            ssd1306_draw_point(x, y, chDot);
        }
    }

    ssd1306_refresh_gram();
}
