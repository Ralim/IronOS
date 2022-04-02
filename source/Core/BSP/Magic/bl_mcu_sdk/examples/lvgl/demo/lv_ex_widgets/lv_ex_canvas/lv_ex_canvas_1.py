from lv_colors import lv_colors
CANVAS_WIDTH  = 200
CANVAS_HEIGHT = 150
LV_IMG_ZOOM_NONE = 256

# create a canvas
rect_dsc = lv.draw_rect_dsc_t()
rect_dsc.init()
rect_dsc.radius = 10
rect_dsc.bg_opa = lv.OPA.COVER
rect_dsc.bg_grad_dir = lv.GRAD_DIR.HOR
rect_dsc.bg_color = LV_COLOR_RED;
rect_dsc.bg_grad_color = LV_COLOR_BLUE
rect_dsc.border_width = 2
rect_dsc.border_opa = lv.OPA._90
rect_dsc.border_color = LV_COLOR_WHITE
rect_dsc.shadow_width = 5
rect_dsc.shadow_ofs_x = 5
rect_dsc.shadow_ofs_y = 5

label_dsc = lv.draw_label_dsc_t()
label_dsc.init()
label_dsc.color = lv_colors.YELLOW

cbuf=bytearray(CANVAS_WIDTH * CANVAS_HEIGHT * 4)

canvas = lv.canvas(lv.scr_act(),None)
canvas.set_buffer(cbuf,CANVAS_WIDTH,CANVAS_HEIGHT,lv.img.CF.TRUE_COLOR)
canvas.align(None,lv.ALIGN.CENTER,0,0)
canvas.fill_bg(lv_colors.SILVER, lv.OPA.COVER);
canvas.draw_rect(70, 60, 100, 70, rect_dsc)
canvas.draw_text(40, 20, 100, label_dsc, "Some text on text canvas", lv.label.ALIGN.LEFT);

# Test the rotation. It requires an other buffer where the orignal image is stored.
# So copy the current image to buffer and rotate it to the canvas

img = lv.img_dsc_t()
img.data = cbuf[:]
img.header.cf = lv.img.CF.TRUE_COLOR
img.header.w = CANVAS_WIDTH
img.header.h = CANVAS_HEIGHT

canvas.fill_bg(lv_colors.SILVER, lv.OPA.COVER)
canvas.transform(img, 30, LV_IMG_ZOOM_NONE, 0, 0, CANVAS_WIDTH // 2, CANVAS_HEIGHT // 2, True);

