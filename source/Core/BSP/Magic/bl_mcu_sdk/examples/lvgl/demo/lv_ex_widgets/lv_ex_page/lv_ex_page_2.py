# create a page
page = lv.page(lv.scr_act(),None)
page.set_size(200,200)
page.align(None,lv.ALIGN.CENTER,0,0)
# page.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)

# Create a label on the page
label = lv.label(page, None);
#label.set_long_mode(lv.label.LONG.BREAK);            # Automatically break long lines
label.set_width(lv.page.get_width_fit(page));        # Set the label width to max value to not show hor. scroll bars*/
label.set_text("Lorem ipsum dolor sit amet, consectetur adipiscing elit,\n"
               "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
               "Ut enim ad minim veniam, quis nostrud exercitation ullamco\n"
               "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure\n"
               "dolor in reprehenderit in voluptate velit esse cillum dolore\n"
               "eu fugiat nulla pariatur.\n"
               "Excepteur sint occaecat cupidatat non proident, sunt in culpa\n"
               "qui officia deserunt mollit anim id est laborum.")


