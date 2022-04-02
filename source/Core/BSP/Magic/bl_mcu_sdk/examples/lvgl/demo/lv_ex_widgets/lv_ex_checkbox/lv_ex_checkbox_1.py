def event_handler(source,evt):
    if  evt == lv.EVENT.VALUE_CHANGED:
        if source.is_checked():
            print("State: checked")
        else:
            print("State: unchecked")

# create a checkbox
cb = lv.checkbox(lv.scr_act(),None)
cb.set_text("I agree to terms\nand conditions.")
cb.align(None,lv.ALIGN.CENTER, 0, 0)
# attach the callback
cb.set_event_cb(event_handler)

