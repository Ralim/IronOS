from lv_colors import lv_colors
style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(lv.STATE.DEFAULT, 5)
style.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)

# Add a repeating pattern
style.set_pattern_image(lv.STATE.DEFAULT, lv.SYMBOL.OK)
style.set_pattern_recolor(lv.STATE.DEFAULT, lv_colors.BLUE)
style.set_pattern_opa(lv.STATE.DEFAULT, lv.OPA._50);
style.set_pattern_repeat(lv.STATE.DEFAULT, True)

# Create an object with the new style
obj = lv.obj(lv.scr_act(), None)
obj.add_style(lv.obj.PART.MAIN, style)
obj.align(None, lv.ALIGN.CENTER, 0, 0)

