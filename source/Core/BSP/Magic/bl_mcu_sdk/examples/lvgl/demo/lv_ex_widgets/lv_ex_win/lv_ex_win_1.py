# create a window
win = lv.win(lv.scr_act(),None)
win.set_title("Window title")                   # Set the title

win_style = lv.style_t()
win_style.init()
win_style.set_margin_right(lv.STATE.DEFAULT, 50)
win.add_style(lv.win.PART.CONTENT_SCROLLABLE,win_style)

# Add control button to the header
close_btn = win.add_btn_right(lv.SYMBOL.CLOSE)
close_btn.set_event_cb(lambda obj,e:  lv.win.close_event_cb(lv.win.__cast__(obj), e))


win.add_btn_right(lv.SYMBOL.SETTINGS)           # Add a setup button

# Add some dummy content
txt = lv.label(win)
txt.set_text(
"""This is the content of the window

You can add control buttons to
the window header

The content area becomes automatically
scrollable if it's large enough.

You can scroll the content
See the scroll bar on the bottom!"""
)

