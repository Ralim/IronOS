def event_handler(source,evt):
    if evt == lv.EVENT.CLICKED:
        if source == btn1:
            # treat "clicked" events only for btn1
            print("Clicked")
        elif evt == lv.EVENT.VALUE_CHANGED:
            print("Toggled")

path_overshoot = lv.anim_path_t()
path_overshoot.init()
path_overshoot.set_cb(lv.anim_path_t.overshoot)

path_ease_out = lv.anim_path_t()
path_ease_out.init()
path_overshoot.set_cb(lv.anim_path_t.ease_out)

path_ease_in_out = lv.anim_path_t()
path_ease_in_out.init()
path_overshoot.set_cb(lv.anim_path_t.ease_in_out)

# Gum line button
style_gum = lv.style_t()
style_gum.init()
style_gum.set_transform_width(lv.STATE.PRESSED, 10)
style_gum.set_transform_height(lv.STATE.PRESSED, -10)
style_gum.set_value_letter_space(lv.STATE.PRESSED, 5)
style_gum.set_transition_path(lv.STATE.DEFAULT,path_overshoot)
style_gum.set_transition_path(lv.STATE.PRESSED,path_ease_in_out)
style_gum.set_transition_time(lv.STATE.DEFAULT, 250)
style_gum.set_transition_delay(lv.STATE.DEFAULT, 100)
style_gum.set_transition_prop_1(lv.STATE.DEFAULT, lv.STYLE.TRANSFORM_WIDTH)
style_gum.set_transition_prop_2(lv.STATE.DEFAULT, lv.STYLE.TRANSFORM_HEIGHT)
style_gum.set_transition_prop_3(lv.STATE.DEFAULT, lv.STYLE.VALUE_LETTER_SPACE)

btn1 = lv.btn(lv.scr_act(),None)
btn1.align(None,lv.ALIGN.CENTER,0,-80)
btn1.add_style(lv.btn.PART.MAIN,style_gum)
# Instead of creating a label add a values string*
btn1.set_style_local_value_str(lv.btn.PART.MAIN, lv.STATE.DEFAULT, "Gum");

# Halo on press
style_halo=lv.style_t()
style_halo.init()
style_halo.set_transition_time(lv.STATE.PRESSED, 400)xb
style_halo.set_transition_time(lv.STATE.DEFAULT, 0)
style_halo.set_transition_delay(lv.STATE.DEFAULT, 200)
style_halo.set_outline_width(lv.STATE.DEFAULT, 0)
style_halo.set_outline_width(lv.STATE.PRESSED, 20)
style_halo.set_outline_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style_halo.set_outline_opa(lv.STATE.FOCUSED, lv.OPA.COVER)   # Just to be sure, the theme might use it
style_halo.set_outline_opa(lv.STATE.PRESSED, lv.OPA.TRANSP)
style_halo.set_transition_prop_1(lv.STATE.DEFAULT, lv.STYLE.OUTLINE_OPA)
style_halo.set_transition_prop_2(lv.STATE.DEFAULT, lv.STYLE.OUTLINE_WIDTH)

btn2=lv.btn(lv.scr_act(), None);
btn2.align(None, lv.ALIGN.CENTER, 0, 0);
btn2.add_style(lv.btn.PART.MAIN, style_halo);
btn2.set_style_local_value_str(lv.btn.PART.MAIN, lv.STATE.DEFAULT, "Halo");

# Ripple on press
style_ripple=lv.style_t()
style_ripple.init()
style_ripple.set_transition_time(lv.STATE.PRESSED, 300)
style_ripple.set_transition_time(lv.STATE.DEFAULT, 0)
style_ripple.set_transition_delay(lv.STATE.DEFAULT, 300)
style_ripple.set_bg_opa(lv.STATE.DEFAULT, 0)
style_ripple.set_bg_opa(lv.STATE.PRESSED, lv.OPA._80)
style_ripple.set_border_width(lv.STATE.DEFAULT, 0)
style_ripple.set_outline_width(lv.STATE.DEFAULT, 0)
style_ripple.set_transform_width(lv.STATE.DEFAULT, -20)
style_ripple.set_transform_height(lv.STATE.DEFAULT, -20)
style_ripple.set_transform_width(lv.STATE.PRESSED, 0)
style_ripple.set_transform_height(lv.STATE.PRESSED, 0)

style_ripple.set_transition_path(lv.STATE.DEFAULT, path_ease_out)
style_ripple.set_transition_prop_1(lv.STATE.DEFAULT, lv.STYLE.BG_OPA)
style_ripple.set_transition_prop_2(lv.STATE.DEFAULT, lv.STYLE.TRANSFORM_WIDTH)
style_ripple.set_transition_prop_3(lv.STATE.DEFAULT, lv.STYLE.TRANSFORM_HEIGHT)

btn3 = lv.btn(lv.scr_act(), None)
btn3.align(None, lv.ALIGN.CENTER, 0, 80)
btn3.add_style(lv.btn.PART.MAIN, style_ripple)
btn3.set_style_local_value_str(lv.btn.PART.MAIN, lv.STATE.DEFAULT, "Ripple")

