LV_HOR_RES=240
LV_VER_RES=240

def ta_event_cb(ta,event):
    if  event == lv.EVENT.CLICKED:
        # Focus on the clicked text area
       if kb != None:
           kb.set_textarea(ta)
       elif event == LV_EVENT_INSERT:
        string = lv.event.get_data()
        if string[0] == '\n':
            print("Ready")

# Create the password box
pwd_ta = lv.textarea(lv.scr_act(), None)
pwd_ta.set_text("")
pwd_ta.set_pwd_mode(True)
pwd_ta.set_one_line(True)
pwd_ta.set_cursor_hidden(True)
pwd_ta.set_width(LV_HOR_RES // 2 - 20)
pwd_ta.set_pos(5, 20)
pwd_ta.set_event_cb(ta_event_cb)

# Create a label and position it above the text box
pwd_label = lv.label(lv.scr_act(), None)
pwd_label.set_text("Password:")
pwd_label.align(pwd_ta, lv.ALIGN.OUT_TOP_LEFT, 0, 0)

# Create the one-line mode text area
oneline_ta = lv.textarea(lv.scr_act(), pwd_ta)
oneline_ta.set_pwd_mode(False)
oneline_ta.set_cursor_hidden(True)
oneline_ta.align(None, lv.ALIGN.IN_TOP_RIGHT, -5, 20)
oneline_ta.set_event_cb(ta_event_cb)

# Create a label and position it above the text box
oneline_label = lv.label(lv.scr_act(), None)
oneline_label.set_text("Text:")
oneline_label.align(oneline_ta, lv.ALIGN.OUT_TOP_LEFT, 0, 0)

# Create a keyboard
kb = lv.keyboard(lv.scr_act(), None)
kb.set_size(LV_HOR_RES, LV_VER_RES // 2)

kb.set_textarea(pwd_ta)      #Focus it on one of the text areas to start
kb.set_cursor_manage(True)   # Automatically show/hide cursors on text areas */

