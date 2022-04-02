def event_handler(obj,evt):
    if evt == lv.EVENT.VALUE_CHANGED:
        state = obj.get_state()
        if state:
            print("State: On")
        else:
            print("State: Off")

#Create a switch and apply the styles
sw1 = lv.switch(lv.scr_act(), None)
sw1.align(None, lv.ALIGN.CENTER, 0, -50)
sw1.set_event_cb(event_handler)

# Copy the first switch and turn it ON
sw2=lv.switch(lv.scr_act(),sw1)
sw2.on(lv.ANIM.ON)
sw2.set_event_cb(event_handler)
sw2.align(None, lv.ALIGN.CENTER, 0, 50)

