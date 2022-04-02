from lv_colors import lv_colors
style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(lv.STATE.DEFAULT, 5)
style.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)

# Add outline
style.set_outline_width(lv.STATE.DEFAULT, 2)
style.set_outline_color(lv.STATE.DEFAULT, lv_colors.BLUE)
style.set_outline_pad(lv.STATE.DEFAULT, 8)

# Create an object with the new style
obj = lv.obj(lv.scr_act(), None)
obj.add_style(lv.obj.PART.MAIN, style)
obj.align(None, lv.ALIGN.CENTER, 0, 0)
