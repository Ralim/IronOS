obj1 = lv.obj(lv.scr_act(),None)
obj1.set_size(100,50)
obj1.align(None,lv.ALIGN.CENTER, -60, -30)

# Copy the previous object and enable drag
obj2 = lv.obj(lv.scr_act(),obj1)
#obj2.set_size(100,50)
obj2.align(None,lv.ALIGN.CENTER, 0, 0)
obj2.set_drag(True)

# create style
style_shadow = lv.style_t()
style_shadow.init()
style_shadow.set_shadow_width(lv.STATE.DEFAULT, 10)
style_shadow.set_shadow_spread(lv.STATE.DEFAULT, 5)
LV_COLOR_BLUE=lv.color_hex3(0xF)
style_shadow.set_shadow_color(lv.STATE.DEFAULT,LV_COLOR_BLUE)

# Copy the previous object (drag is already enabled)
obj3 = lv.obj(lv.scr_act(),obj2)
obj3.add_style(obj3.PART.MAIN,style_shadow)
obj3.align(None,lv.ALIGN.CENTER, 60, 30)

