from lv_colors import lv_colors
style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(lv.STATE.DEFAULT, 5)
style.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)

# Set some paddings
style.set_pad_inner(lv.STATE.DEFAULT, 20)
style.set_pad_top(lv.STATE.DEFAULT, 20)
style.set_pad_left(lv.STATE.DEFAULT, 5)
style.set_pad_right(lv.STATE.DEFAULT, 5)

style.set_scale_end_color(lv.STATE.DEFAULT, lv_colors.RED)
style.set_line_color(lv.STATE.DEFAULT, lv_colors.WHITE)
style.set_scale_grad_color(lv.STATE.DEFAULT, lv_colors.BLUE)
style.set_line_width(lv.STATE.DEFAULT, 2)
style.set_scale_end_line_width(lv.STATE.DEFAULT, 4)
style.set_scale_end_border_width(lv.STATE.DEFAULT, 4)

# Gauge has a needle but for simplicity its style is not initialized here

# Create an object with the new style*/
obj = lv.gauge(lv.scr_act(), None)
obj.add_style(lv.gauge.PART.MAIN, style)
obj.align(None, lv.ALIGN.CENTER, 0, 0)
