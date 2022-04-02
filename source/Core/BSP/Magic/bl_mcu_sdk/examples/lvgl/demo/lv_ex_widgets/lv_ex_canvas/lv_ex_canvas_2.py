def LV_IMG_BUF_SIZE_ALPHA_1BIT(w, h):
    return int(((w / 8) + 1) * h)

def LV_IMG_BUF_SIZE_INDEXED_1BIT(w, h):
    return LV_IMG_BUF_SIZE_ALPHA_1BIT(w, h) + 4 * 2

def LV_CANVAS_BUF_SIZE_INDEXED_1BIT(w, h):
    return LV_IMG_BUF_SIZE_INDEXED_1BIT(w, h)

CANVAS_WIDTH  = 50
CANVAS_HEIGHT = 50

# Create a button to better see the transparency
btn=lv.btn(lv.scr_act(), None)

# Create a buffer for the canvas
cbuf= bytearray(LV_CANVAS_BUF_SIZE_INDEXED_1BIT(CANVAS_WIDTH, CANVAS_HEIGHT))

# Create a canvas and initialize its the palette
canvas = lv.canvas(lv.scr_act(), None)
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.img.CF.INDEXED_1BIT)
canvas.set_palette(0, LV_COLOR_LIME)
canvas.set_palette(1, LV_COLOR_RED)

# Create colors with the indices of the palette
c0 = lv.color_t()
c1 = lv.color_t()

c0.full = 0
c1.full = 1

# Transparent background
canvas.fill_bg(c1, lv.OPA.TRANSP)

# Create hole on the canvas
for y in range(10,30):
    for x in range(5,20):
        canvas.set_px(x, y, c0)

