LV_HOR_RES = 240
LV_VER_RES = 240

def ta_event_cb(ta,event):
    if  event == lv.EVENT.VALUE_CHANGED:
        txt = ta.get_text()
        pos = ta.get_cursor_pos()
        # find position of ":" in text
        colon_pos= txt.find(":")
        # if there are more than 2 digits before the colon, remove the last one entered
        if colon_pos == 3:
            ta.del_char()
        if colon_pos != -1:
            # if there are more than 3 digits after the ":" remove the last one entered
            rest = txt[colon_pos:]
            if len(rest) > 3:
                ta.del_char()

        if len(txt) < 2:
            return
        if ":" in txt:
            return
        if  txt[0] >= '0' and txt[0] <= '9' and \
            txt[1] >= '0' and txt[1] <= '9':
            if len(txt) == 2 or txt[2] != ':' :
                ta.set_cursor_pos(2)
                ta.add_char(ord(':'))

# create a textarea
ta = lv.textarea(lv.scr_act(),None)
ta.set_event_cb(ta_event_cb)
ta.set_accepted_chars("0123456789:")
ta.set_max_length(5)
ta.set_one_line(True)
ta.set_text("");

kb_map = ["1","2", "3", " ","\n",
          "4", "5", "6", " ", "\n",
          "7", "8", "9",lv.SYMBOL.BACKSPACE,"\n",
          "0",lv.SYMBOL.LEFT,lv.SYMBOL.RIGHT," ",""]

kb_ctlr_map = [lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.HIDDEN,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.HIDDEN,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.NO_REPEAT,
               lv.btnmatrix.CTRL.HIDDEN]


# Create a keyboard
kb = lv.keyboard(lv.scr_act(), None)
kb.set_size(LV_HOR_RES, LV_VER_RES // 2)
kb.set_mode(lv.keyboard.MODE.SPECIAL)
kb.set_map(lv.keyboard.MODE.SPECIAL,kb_map)
kb.set_ctrl_map(lv.keyboard.MODE.SPECIAL,kb_ctlr_map)
kb.set_textarea(ta)

