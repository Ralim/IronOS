# Set a very visible color for the screen to clearly see what happens
lv.scr_act().set_style_local_bg_color(lv.obj.PART.MAIN, lv.STATE.DEFAULT, lv.color_hex3(0xf33))

om = lv.objmask(lv.scr_act(), None)
om.set_size(200, 200)
om.align(None, lv.ALIGN.CENTER, 0, 0)
label = lv.label(om, None)
label.set_long_mode(lv.label.LONG.BREAK)
label.set_align(lv.label.ALIGN.CENTER)
label.set_width(180)
label.set_text("This label will be masked out. See how it works.")
label.align(None, lv.ALIGN.IN_TOP_MID, 0, 20)

cont = lv.cont(om, None)
cont.set_size(180, 100)
cont.set_drag(True)
cont.align(None, lv.ALIGN.IN_BOTTOM_MID, 0, -10)

btn = lv.btn(cont, None)
btn.align(None, lv.ALIGN.CENTER, 0, 0)
btn.set_style_local_value_str(lv.btn.PART.MAIN, lv.STATE.DEFAULT, "Button")

lv.refr_now(None);
t = lv.tick_get();
while lv.tick_elaps(t) < 1000:
    pass

a=lv.area_t()
r1=lv.draw_mask_radius_param_t()

a.x1 = 10
a.y1 = 10
a.x2 = 190
a.y2 = 190
r1.init(a, lv.RADIUS.CIRCLE, False)
om.add_mask(r1)

lv.refr_now(None);
t = lv.tick_get();
while lv.tick_elaps(t) < 1000:
    pass

a.x1 = 100
a.y1 = 100
a.x2 = 150
a.y2 = 150
r1.init(a, lv.RADIUS.CIRCLE, True)
om.add_mask(r1)

lv.refr_now(None)
t = lv.tick_get()
while lv.tick_elaps(t) < 1000:
    pass

l1=lv.draw_mask_line_param_t()
l1.points_init(0, 0, 100, 200, lv.DRAW_MASK_LINE_SIDE.TOP)
om.add_mask(l1)

lv.refr_now(None)
t = lv.tick_get()
while lv.tick_elaps(t) < 1000:
    pass

f1= lv.draw_mask_fade_param_t()
a.x1 = 100
a.y1 = 0
a.x2 = 200
a.y2 = 200
f1.init(a, lv.OPA.TRANSP, 0, lv.OPA.COVER, 150)
om.add_mask(f1)

