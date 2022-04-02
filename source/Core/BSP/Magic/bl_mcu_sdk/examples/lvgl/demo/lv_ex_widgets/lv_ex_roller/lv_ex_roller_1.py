def event_handler(source,evt):
    if evt == lv.EVENT.VALUE_CHANGED:
        month=" "*10
        roller.get_selected_str(month,len(month))
        print("Selected month: ",month)

roller = lv.roller(lv.scr_act(),None)
roller.set_options("January\n"
                   "February\n"
                   "March\n"
                   "April\n"
                   "May\n"
                   "June\n"
                   "July\n"
                   "August\n"
                   "September\n"
                   "October\n"
                   "November\n"
                   "December",
                   lv.roller.MODE.INIFINITE)
roller.set_visible_row_count(4)
roller.align(None,lv.ALIGN.CENTER,0,0)
roller.set_event_cb(event_handler)

