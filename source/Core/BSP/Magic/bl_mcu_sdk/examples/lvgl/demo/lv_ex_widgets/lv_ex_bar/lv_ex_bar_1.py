def event_handler(source,evt):
    if  evt == lv.EVENT.VALUE_CHANGED:
        if source.is_checked():
            print("State: checked")
        else:
            print("State: unchecked")

# create a simple bar
bar1 = lv.bar(lv.scr_act(),None)
bar1.set_size(200,20)
bar1.align(None,lv.ALIGN.CENTER,0,0)
bar1.set_anim_time(2000)
bar1.set_value(100,lv.ANIM.ON)

