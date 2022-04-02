from lv_colors import lv_colors

# Create a style for the shadow
label_shadow_style = lv.style_t()
label_shadow_style.init()
label_shadow_style.set_text_opa(lv.STATE.DEFAULT,lv.OPA._50)
label_shadow_style.set_text_color(lv.STATE.DEFAULT, lv_colors.RED);

# Create a label for the shadow first (it's in the background)
shadow_label = lv.label(lv.scr_act())
shadow_label.add_style(lv.label.PART.MAIN, label_shadow_style)

# Create the main label
main_label = lv.label(lv.scr_act())
main_label.set_text("A simple method to create\n" +
                              "shadows on text\n" +
                              "It even works with\n\n" +
                              "newlines     and spaces.")

# Set the same text for the shadow label
shadow_label.set_text(main_label.get_text())

# Position the main label
main_label.align(None, lv.ALIGN.CENTER, 0, 0)

# Shift the second label down and to the right by 1 pixel
shadow_label.align(main_label, lv.ALIGN.IN_TOP_LEFT, 1, 1)

while True:
    lv.task_handler()
    time.sleep_ms(5)
