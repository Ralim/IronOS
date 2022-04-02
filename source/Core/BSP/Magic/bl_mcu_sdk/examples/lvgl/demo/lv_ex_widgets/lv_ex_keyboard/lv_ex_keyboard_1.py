LV_DPI=130
LV_VER_RES=240

def event_handler(source,evt):
    if  evt == lv.EVENT.VALUE_CHANGED:
        print("Value:",textarea.get_text())

# create a keyboard and apply the styles
keyb = lv.keyboard(lv.scr_act(),None)
keyb.set_cursor_manage(True)

#Create a text area. The keyboard will write here
ta=lv.textarea(lv.scr_act(),None)
ta.align(None,lv.ALIGN.IN_TOP_MID,0,LV_DPI//16)
ta.set_text("")
max_h = LV_VER_RES // 2 - LV_DPI // 8
if ta.get_height() > max_h:
    ta.set_height(max_h)

# Assign the text area to the keyboard*/
keyb.set_textarea(ta)

