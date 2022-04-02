def event_handler(source,evt):
    if evt == lv.EVENT.VALUE_CHANGED:
        print("Toggled")
    txt = source.get_active_btn_text()
    print("%s was pressed"%txt)

# Create a screen and load it
scr = lv.obj()
lv.scr_load(scr)

btnm_map = ["1","2", "3", "4", "5", "\n",
            "6", "7", "8", "9", "0", "\n",
            "Action1", "Action2", ""]

btnm_ctlr_map = [46|lv.btnmatrix.CTRL.NO_REPEAT,46,46,46,46,
                 46,46,46,46,46,
                 115,115]

# create a button matrix
btnm1 = lv.btnmatrix(lv.scr_act(),None)
btnm1.set_map(btnm_map)
#btnm1.set_ctrl_map(btnm_ctlr_map)
btnm1.set_btn_ctrl(10,lv.btnmatrix.CTRL.CHECKABLE)
btnm1.set_btn_ctrl(11,lv.btnmatrix.CTRL.CHECK_STATE)
btnm1.set_width(230)
btnm1.align(None,lv.ALIGN.CENTER,0,0)
# attach the callback
btnm1.set_event_cb(event_handler)
