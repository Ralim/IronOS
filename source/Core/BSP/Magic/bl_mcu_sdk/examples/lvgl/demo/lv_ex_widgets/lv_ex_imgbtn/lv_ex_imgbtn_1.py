import usys as sys
from lv_colors import lv_colors

# create the green image data
try:
    with open('../../../assets/imgbtn_green_argb.bin','rb') as f:
        green_img_data = f.read()
except:
    try:
        with open('images/imgbtn_green_rgb565.bin','rb') as f:
            green_img_data = f.read()
    except:
        print("Cannot open green buttom image file")
        sys.exit()

# create the blue image data
try:
    with open('../../../assets/imgbtn_blue_argb.bin','rb') as f:
        blue_img_data = f.read()
except:
    try:
        with open('images/imgbtn_blue_rgb565.bin','rb') as f:
            blue_img_data = f.read()
    except:
        print("Cannot open green buttom image file")
        sys.exit()

imgbtn_green_dsc = lv.img_dsc_t(
    {
        "header": {"always_zero": 0, "w": 125, "h": 40, "cf": lv.img.CF.TRUE_COLOR_ALPHA},
        "data": green_img_data,
        "data_size": len(green_img_data),
    }
)

# create the blue image data

imgbtn_blue_dsc = lv.img_dsc_t(
    {
        "header": {"always_zero": 0, "w": 125, "h": 40, "cf": lv.img.CF.TRUE_COLOR_ALPHA},
        "data": blue_img_data,
        "data_size": len(blue_img_data),
    }
)
    # Darken the button when pressed
style=lv.style_t()
style.init()
style.set_image_recolor_opa(lv.STATE.PRESSED, lv.OPA._30)
style.set_image_recolor(lv.STATE.PRESSED, lv_colors.BLACK)
style.set_text_color(lv.STATE.DEFAULT, lv_colors.WHITE)

# Create an Image button
imgbtn1 = lv.imgbtn(lv.scr_act(), None)
imgbtn1.set_src(lv.btn.STATE.RELEASED, imgbtn_green_dsc)
imgbtn1.set_src(lv.btn.STATE.PRESSED, imgbtn_green_dsc)
imgbtn1.set_src(lv.btn.STATE.CHECKED_RELEASED, imgbtn_blue_dsc)
imgbtn1.set_src(lv.btn.STATE.CHECKED_PRESSED, imgbtn_blue_dsc)
imgbtn1.set_checkable(True)
imgbtn1.add_style(lv.imgbtn.PART.MAIN, style)
imgbtn1.align(None, lv.ALIGN.CENTER, 0, -40)

# Create a label on the Image button
label = lv.label(imgbtn1, None)
label.set_text("Button")
