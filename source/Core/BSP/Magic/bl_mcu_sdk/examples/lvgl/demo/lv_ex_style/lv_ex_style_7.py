from lv_colors import lv_colors
style = lv.style_t()
style.init()
# Set a background color and a radius
style.set_radius(lv.STATE.DEFAULT, 5)
style.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)
style.set_border_width(lv.STATE.DEFAULT, 2)
style.set_border_color(lv.STATE.DEFAULT, lv_colors.BLUE)

style.set_pad_top(lv.STATE.DEFAULT, 10)
style.set_pad_bottom(lv.STATE.DEFAULT, 10)
style.set_pad_left(lv.STATE.DEFAULT, 10)
style.set_pad_right(lv.STATE.DEFAULT, 10)

style.set_text_color(lv.STATE.DEFAULT, lv_colors.BLUE)
style.set_text_letter_space(lv.STATE.DEFAULT, 5)
style.set_text_line_space(lv.STATE.DEFAULT, 20)
style.set_text_decor(lv.STATE.DEFAULT, lv.TEXT_DECOR.UNDERLINE)

# Create an object with the new style
obj = lv.label(lv.scr_act(), None)
obj.add_style(lv.label.PART.MAIN, style)
obj.set_text("Text of\n"
                            "a label")
obj.align(None, lv.ALIGN.CENTER, 0, 0)
