def event_handler(source,evt):
    if  evt == lv.EVENT.VALUE_CHANGED:
        print("Value:",slider.get_value())

# create a slider
slider = lv.slider(lv.scr_act(),None)
slider.set_width(200)
slider.align(None,lv.ALIGN.CENTER,0,0)
slider.set_event_cb(event_handler)

