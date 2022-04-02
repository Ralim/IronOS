from lv_colors import lv_colors
from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

# Create an image

with open('img_hand.png','rb') as f:
  png_data = f.read()

img_hand_dsc = lv.img_dsc_t({
    'data_size': len(png_data),
    'data': png_data
})
# needle colors
needle_colors=[lv_colors.BLUE,lv_colors.ORANGE,lv_colors.PURPLE]

# create the gauge
gauge1=lv.gauge(lv.scr_act(),None)
gauge1.set_needle_count(3, needle_colors)
gauge1.set_size(200,200)
gauge1.align(None,lv.ALIGN.CENTER,0,0)
gauge1.set_needle_img(img_hand_dsc, 4, 4)
gauge1.set_style_local_image_recolor_opa(lv.gauge.PART.NEEDLE, lv.STATE.DEFAULT, lv.OPA.COVER)

# Set the values
gauge1.set_value(0, 10)
gauge1.set_value(1, 20)
gauge1.set_value(2, 30)

