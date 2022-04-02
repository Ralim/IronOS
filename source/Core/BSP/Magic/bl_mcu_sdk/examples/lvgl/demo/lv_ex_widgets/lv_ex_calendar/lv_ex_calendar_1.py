def event_handler(source,evt):
    if  evt == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar.get_pressed_date(source)
        if date:
            print("Clicked date: %02d.%02d.%02d"%(date.day, date.month, date.year))

# create a calendar
calendar = lv.calendar(lv.scr_act(),None)
calendar.set_size(235,235)
calendar.align(None,lv.ALIGN.CENTER,0,0)
calendar.set_event_cb(event_handler)

# Make the date number smaller to be sure they fit into their area
calendar.set_style_local_text_font(lv.calendar.PART.DATE,lv.STATE.DEFAULT,lv.theme_get_font_small())
today = lv.calendar_date_t()
today.year = 2020;
today.month = 10;
today.day = 5

calendar.set_today_date(today)
calendar.set_showed_date(today)

# Highlight a few days
highlighted_days=[]
for i in range(3):
    highlighted_days.append(lv.calendar_date_t())

highlighted_days[0].year=2020
highlighted_days[0].month=10
highlighted_days[0].day=6

highlighted_days[1].year=2020
highlighted_days[1].month=10
highlighted_days[1].day=11

highlighted_days[2].year=2020
highlighted_days[2].month=10
highlighted_days[2].day=22

calendar.set_highlighted_dates(highlighted_days,3)

