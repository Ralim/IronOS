from lv_colors import lv_colors
style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(lv.STATE.DEFAULT, 20)
style.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)

# Add border to the bottom+right
style.set_border_color(lv.STATE.DEFAULT, lv_colors.BLUE)
style.set_border_width(lv.STATE.DEFAULT, 5);
style.set_border_opa(lv.STATE.DEFAULT, lv.OPA._50)
style.set_border_side(lv.STATE.DEFAULT, lv.BORDER_SIDE.BOTTOM | lv.BORDER_SIDE.RIGHT);

# Create an object with the new style
obj = lv.obj(lv.scr_act(), None)
obj.add_style(lv.obj.PART.MAIN, style);
obj.align(None, lv.ALIGN.CENTER, 0, 0);
