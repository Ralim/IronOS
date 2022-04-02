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
# Create an image using the decoder

img1 = lv.img(lv.scr_act(),None)
lv.img.cache_set_size(2)
img1.align(lv.scr_act(), lv.ALIGN.CENTER, 0, -50)
img1.set_src(cogwheel_img_dsc)

img2 = lv.img(lv.scr_act(), None)
img2.set_src(lv.SYMBOL.OK+"Accept")
img2.align(img1, lv.ALIGN.OUT_BOTTOM_MID, 0, 20)

