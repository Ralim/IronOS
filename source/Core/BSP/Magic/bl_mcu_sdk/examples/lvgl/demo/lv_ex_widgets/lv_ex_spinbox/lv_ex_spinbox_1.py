def increment_event_cb(source,evt):
    if evt == lv.EVENT.SHORT_CLICKED or evt == lv.EVENT.LONG_PRESSED_REPEAT:
        spinbox.increment()

def decrement_event_cb(source,evt):
    if evt == lv.EVENT.SHORT_CLICKED or evt == lv.EVENT.LONG_PRESSED_REPEAT:
        spinbox.decrement()

spinbox = lv.spinbox(lv.scr_act(),None)
spinbox.set_range(-1000,90000)
spinbox.set_digit_format(5,2)
spinbox.step_prev()
spinbox.set_width(100)
spinbox.align(None,lv.ALIGN.CENTER,0,0)

h = spinbox.get_height()
btn = lv.btn(lv.scr_act(),None)
btn.set_size(h,h)
btn.align(spinbox,lv.ALIGN.OUT_RIGHT_MID, 5, 0)
lv.theme_apply(btn,lv.THEME.SPINBOX_BTN)

btn.set_style_local_value_str(lv.btn.PART.MAIN, lv.STATE.DEFAULT, lv.SYMBOL.PLUS)
btn.set_event_cb(increment_event_cb)

btn = lv.btn(lv.scr_act(), btn)
btn.align(spinbox, lv.ALIGN.OUT_LEFT_MID, -5, 0)
btn.set_event_cb(decrement_event_cb)
btn.set_style_local_value_str(lv.btn.PART.MAIN, lv.STATE.DEFAULT, lv.SYMBOL.MINUS)
