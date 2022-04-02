def event_handler(source,evt):
    if  evt == lv.EVENT.VALUE_CHANGED:
        if source.is_checked():
            print("State: checked")
        else:
            print("State: unchecked")

# create a container
cont = lv.cont(lv.scr_act(),None)
cont.set_auto_realign(True)                 # Auto realign when the size changes
cont.align_mid(None,lv.ALIGN.CENTER,0,0)  # This parameters will be sued when realigned
cont.set_fit(lv.FIT.TIGHT)
cont.set_layout(lv.LAYOUT.COLUMN_MID);
label = lv.label(cont)
label.set_text("Short text")

# Refresh and pause here for a while to see how `fit` works
time.sleep(1)

label = lv.label(cont)
label.set_text("It is a long text")

# Wait here too
time.sleep(1)

label = lv.label(cont)
label.set_text("Here an even longer text")
