def event_handler(source,evt):
    if evt == lv.EVENT.CLICKED:
        if source == btn1:
            # treat "clicked" events only for btn1
            print("Clicked")
        elif evt == lv.EVENT.VALUE_CHANGED:
            print("Toggled")


# create a simple button
btn1 = lv.btn(lv.scr_act(),None)
# attach the callback
btn1.set_event_cb(event_handler)
btn1.align(None,lv.ALIGN.CENTER,0,-40)
label=lv.label(btn1,None)
label.set_text("Button")

# create a toggle button
btn2 = lv.btn(lv.scr_act(),None)
# attach the callback
btn2.set_event_cb(event_handler)
btn2.align(None,lv.ALIGN.CENTER,0,40)
btn2.set_checkable(True)
btn2.toggle()
#btn2.set_fit2(lv.FIT.NONE,lv.FIT.TIGHT)
label=lv.label(btn2,None)
label.set_text("Toggled")

