def slider_event_cb(btn,event):
    global cnt
    if event == lv.EVENT.VALUE_CHANGED:
        # Refresh the text
        label.set_text(str(slider.get_value()))

#
# Create a slider and write its value on a label.
#

slider = lv.slider(lv.scr_act(), None)
slider.set_width(200)                         # Set the width
slider.align(None, lv.ALIGN.CENTER, 0, 0);    # Align to the center of the parent (screen)
slider.set_event_cb(slider_event_cb);         # Assign an event function

# Create a label below the slider
label = lv.label(lv.scr_act(), None)
label.set_text("0")
label.set_auto_realign(True)                  # To keep center alignment when the width of the text changes
label.align(slider, lv.ALIGN.OUT_BOTTOM_MID, 0, 20)    # Align below the slider
