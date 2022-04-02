import usys as sys
from lv_colors import lv_colors

SLIDER_WIDTH=15

try:
  with open('../../../assets/img_cogwheel_argb.bin','rb') as f:
    cogwheel_img_data = f.read()
except:
  try:
    with open('images/img_cogwheel_rgb565.bin','rb') as f:
      cogwheel_img_data = f.read()
  except:
    print("Could not find binary img_cogwheel file")

# create the cogwheel image data

cogwheel_img_dsc = lv.img_dsc_t(
    {
        "header": {"always_zero": 0, "w": 100, "h": 100, "cf": lv.img.CF.TRUE_COLOR_ALPHA},
        "data": cogwheel_img_data,
        "data_size": len(cogwheel_img_data),
    }
)

def slider_event_cb(slider,event):
    if event == lv.EVENT.VALUE_CHANGED:
      # Recolor the image based on the sliders' values
      color  = lv.color_make(red_slider.get_value(), green_slider.get_value(), blue_slider.get_value())
      intense = intense_slider.get_value()
      img1.set_style_local_image_recolor_opa(lv.img.PART.MAIN, lv.STATE.DEFAULT, intense)
      img1.set_style_local_image_recolor(lv.img.PART.MAIN, lv.STATE.DEFAULT, color)

# Create a set of RGB sliders
# Use the red one as a base for all the settings
red_slider = lv.slider(lv.scr_act(), None)
red_slider.set_range(0, 255)
red_slider.set_size(SLIDER_WIDTH, 200)   # Be sure it's a vertical slider
red_slider.set_style_local_bg_color(lv.slider.PART.INDIC, lv.STATE.DEFAULT, lv_colors.RED)
red_slider.set_event_cb(slider_event_cb)

# Copy it for the other three sliders
green_slider = lv.slider(lv.scr_act(), red_slider)
green_slider.set_style_local_bg_color(lv.slider.PART.INDIC, lv.STATE.DEFAULT, lv_colors.LIME)
green_slider.set_event_cb(slider_event_cb)

blue_slider = lv.slider(lv.scr_act(), red_slider)
blue_slider.set_style_local_bg_color(lv.slider.PART.INDIC, lv.STATE.DEFAULT, lv_colors.BLUE)
blue_slider.set_event_cb(slider_event_cb)

intense_slider = lv.slider(lv.scr_act(), red_slider)
intense_slider.set_style_local_bg_color(lv.slider.PART.INDIC, lv.STATE.DEFAULT, lv_colors.GRAY)
intense_slider.set_value(255, lv.ANIM.OFF)
intense_slider.set_event_cb(slider_event_cb)

red_slider.align(None, lv.ALIGN.IN_LEFT_MID, 15, 0)
green_slider.align(red_slider, lv.ALIGN.OUT_RIGHT_MID, 15, 0)
blue_slider.align(green_slider, lv.ALIGN.OUT_RIGHT_MID, 15, 0)
intense_slider.align(blue_slider, lv.ALIGN.OUT_RIGHT_MID, 15, 0)


img1 = lv.img(lv.scr_act(),None)
lv.img.cache_set_size(2)
img1.align(lv.scr_act(), lv.ALIGN.CENTER, 50, -30)
img1.set_src(cogwheel_img_dsc)

