from lv_colors import lv_colors
style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(lv.STATE.DEFAULT, 5)
style.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)

# Set different background color in pressed state
style.set_bg_color(lv.STATE.PRESSED, lv_colors.GRAY)

# Set different transition time in default and pressed state
# fast press, slower revert to default
style.set_transition_time(lv.STATE.DEFAULT, 500)
style.set_transition_time(lv.STATE.PRESSED, 200)

# Small delay to make transition more visible
style.set_transition_delay(lv.STATE.DEFAULT, 100)

# Add `bg_color` to transitioned properties
style.set_transition_prop_1(lv.STATE.DEFAULT, lv.STYLE.BG_COLOR)

# Create an object with the new style
obj = lv.obj(lv.scr_act(), None)
obj.add_style(lv.obj.PART.MAIN, style);
obj.align(None, lv.ALIGN.CENTER, 0, 0);
