import sys
from lv_colors import lv_colors

# create the cogwheel image data
try:
    with open('../../assets/img_cogwheel_argb.bin','rb') as f:
        cogwheel_img_data = f.read()
except:
    try:
        with open('images/img_cogwheel_rgb565.bin','rb') as f:
            cogwheel_img_data = f.read()
    except:
        print("Could not find img_cogwheel binary image file")
        sys.exit()

# create the cogwheel image data

cogwheel_img_dsc = lv.img_dsc_t(
    {
        "header": {"always_zero": 0, "w": 100, "h": 100, "cf": lv.img.CF.TRUE_COLOR_ALPHA},
        "data": cogwheel_img_data,
        "data_size": len(cogwheel_img_data),
    }
)

style = lv.style_t()
style.init()
# Set a background color and a radius
style.set_radius(lv.STATE.DEFAULT, 5)
style.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)
style.set_border_width(lv.STATE.DEFAULT, 2)
style.set_border_color(lv.STATE.DEFAULT, lv_colors.BLUE)
style.set_pad_top(lv.STATE.DEFAULT, 10)
style.set_pad_bottom(lv.STATE.DEFAULT, 10)
style.set_pad_left(lv.STATE.DEFAULT, 10)
style.set_pad_right(lv.STATE.DEFAULT, 10)

style.set_image_recolor(lv.STATE.DEFAULT, lv_colors.BLUE)
style.set_image_recolor_opa(lv.STATE.DEFAULT, lv.OPA._50)

# Create an object with the new style
obj = lv.img(lv.scr_act(), None)
lv.img.cache_set_size(2)

obj.add_style(lv.img.PART.MAIN, style)
obj.set_src(cogwheel_img_dsc)
obj.align(None, lv.ALIGN.CENTER, 0, 0)

while True:
     lv.task_handler()
     time.sleep_ms(10)
