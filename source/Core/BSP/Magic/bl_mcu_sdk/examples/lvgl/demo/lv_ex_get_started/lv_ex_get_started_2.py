from lv_colors import lv_colors
#
# Create styles from scratch for buttons.
#

# Create a simple button style
style_btn=lv.style_t()
style_btn.set_radius(lv.STATE.DEFAULT, 10)
style_btn.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style_btn.set_bg_color(lv.STATE.DEFAULT, lv_colors.SILVER)
style_btn.set_bg_grad_color(lv.STATE.DEFAULT, lv_colors.GRAY)
style_btn.set_bg_grad_dir(lv.STATE.DEFAULT, lv.GRAD_DIR.VER)

# Swap the colors in pressed state
style_btn.set_bg_color(lv.STATE.PRESSED, lv_colors.GRAY)
style_btn.set_bg_grad_color(lv.STATE.PRESSED, lv_colors.SILVER)

# Add a border
style_btn.set_border_color(lv.STATE.DEFAULT, lv_colors.WHITE)
style_btn.set_border_opa(lv.STATE.DEFAULT, lv.OPA._70)
style_btn.set_border_width(lv.STATE.DEFAULT, 2)

# Different border color in focused state
style_btn.set_border_color(lv.STATE.FOCUSED, lv_colors.BLUE)
style_btn.set_border_color(lv.STATE.FOCUSED | lv.STATE.PRESSED, lv_colors.NAVY)

# Set the text style
style_btn.set_text_color(lv.STATE.DEFAULT, lv_colors.WHITE)

# Make the button smaller when pressed
style_btn.set_transform_height(lv.STATE.PRESSED, -5)
style_btn.set_transform_width(lv.STATE.PRESSED, -10)

# Add a transition to the size change*/
anim_path=lv.anim_path_t()
anim_path.init()
anim_path.set_cb(lv.anim_path_t.overshoot);

style_btn.set_transition_prop_1(lv.STATE.DEFAULT, lv.STYLE.TRANSFORM_HEIGHT);
style_btn.set_transition_prop_2(lv.STATE.DEFAULT, lv.STYLE.TRANSFORM_WIDTH);
style_btn.set_transition_time(lv.STATE.DEFAULT, 300);
style_btn.set_transition_path(lv.STATE.DEFAULT, anim_path);

# Create a red style. Change only some colors.
style_btn_red=lv.style_t()
style_btn_red.init()
style_btn_red.set_bg_color(lv.STATE.DEFAULT, lv_colors.RED)
style_btn_red.set_bg_grad_color(lv.STATE.DEFAULT, lv_colors.MAROON)
style_btn_red.set_bg_color(lv.STATE.PRESSED, lv_colors.MAROON)
style_btn_red.set_bg_grad_color(lv.STATE.PRESSED, lv_colors.RED)
style_btn_red.set_text_color(lv.STATE.DEFAULT, lv_colors.WHITE)

# Create buttons and use the new styles
btn = lv.btn(lv.scr_act(), None)                            # Add a button the current screen
btn.set_pos(10, 10)                                         # Set its position
btn.set_size(120, 50)                                       # Set its size
btn.reset_style_list(lv.btn.PART.MAIN)                      # Remove the styles coming from the theme
btn.add_style(lv.btn.PART.MAIN, style_btn)

label = lv.label(btn, None)                                 # Add a label to the button
label.set_text("Button")                                    # Set the labels text

# Create a new button
btn2 = lv.btn(lv.scr_act(), btn)
btn2.set_pos(10, 80)
btn2.set_size(120, 50)                                       # Set its size
btn2.reset_style_list(lv.btn.PART.MAIN)                      # Remove the styles coming from the theme
btn2.add_style(lv.btn.PART.MAIN, style_btn)
btn2.add_style(lv.btn.PART.MAIN, style_btn_red)              # Add the red style on top of the current
btn2.set_style_local_radius(lv.btn.PART.MAIN, lv.STATE.DEFAULT, lv.RADIUS.CIRCLE); # Add a local style

label = lv.label(btn2, None)                                 # Add a label to the button
label.set_text("Button 2")                                   # Set the labels text
