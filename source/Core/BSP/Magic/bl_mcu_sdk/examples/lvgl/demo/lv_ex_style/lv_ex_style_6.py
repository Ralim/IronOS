from lv_colors import lv_colors
style = lv.style_t()
style.init()
# Set a background color and a radius
style.set_radius(lv.STATE.DEFAULT, 5)
style.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)

# Add a value text properties
style.set_value_color(lv.STATE.DEFAULT, lv_colors.BLUE)
style.set_value_align(lv.STATE.DEFAULT, lv.ALIGN.IN_BOTTOM_RIGHT)
style.set_value_ofs_x(lv.STATE.DEFAULT, 10)
style.set_value_ofs_y(lv.STATE.DEFAULT, 10)

# Create an object with the new style
obj = lv.obj(lv.scr_act(), None)
obj.add_style(lv.obj.PART.MAIN, style)
obj.align(None, lv.ALIGN.CENTER, 0, 0)

# Add a value text to the local style. This way every object can have different text
obj.set_style_local_value_str(lv.obj.PART.MAIN, lv.STATE.DEFAULT, "Text")
