from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

with open('img_cogwheel_argb.png','rb') as f:
  png_data = f.read()

png_img_dsc = lv.img_dsc_t({
    'data_size': len(png_data),
    'data': png_data
})

# Create an image using the decoder

img1 = lv.img(lv.scr_act(),None)
lv.img.cache_set_size(2)
img1.align(lv.scr_act(), lv.ALIGN.CENTER, 0, -50)
img1.set_src(png_img_dsc)

img2 = lv.img(lv.scr_act(), None)
img2.set_src(lv.SYMBOL.OK+"Accept")
img2.align(img1, lv.ALIGN.OUT_BOTTOM_MID, 0, 20)

