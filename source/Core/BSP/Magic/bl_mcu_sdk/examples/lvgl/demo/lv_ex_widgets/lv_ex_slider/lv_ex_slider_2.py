def event_handler(source,evt):
    if evt == lv.EVENT.VALUE_CHANGED:
        # print(slider.get_value())
        slider_label.set_text(str(slider.get_value()))

# Create a slider in the center of the display
slider = lv.slider(lv.scr_act(),None)
slider.set_width(200)
slider.align(None,lv.ALIGN.CENTER,0,0)
slider.set_event_cb(event_handler)
slider.set_range(0, 100)

# Create a label below the slider
slider_label=lv.label(lv.scr_act(), None)
slider_label.set_text("0")
slider_label.set_auto_realign(True)
slider_label.align(slider,lv.ALIGN.OUT_BOTTOM_MID,0,10)

# Create an informative label
info_label=lv.label(lv.scr_act(), None)
info_label.set_text(
"""Welcome to the slider +
label demo!
Move the slider
and see that the label
updates to match it."""
)
info_label.align(None, lv.ALIGN.IN_TOP_LEFT, 10, 10)

