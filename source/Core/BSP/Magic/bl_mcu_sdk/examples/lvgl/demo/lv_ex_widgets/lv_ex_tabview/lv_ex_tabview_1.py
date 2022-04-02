# Create a Tab view object
tabview = lv.tabview(lv.scr_act(), None)

# Add 3 tabs (the tabs are page (lv_page) and can be scrolled
tab1=tabview.add_tab("Tab 1")
tab2=tabview.add_tab("Tab 2")
tab3=tabview.add_tab("Tab 3")


# Add content to the tabs
label = lv.label(tab1,None)
label.set_text('''
This the first tab\n
If the content
of a tab
become too long
then it
automatically
becomes
scrollable.
''')

label = lv.label(tab2,None)
label.set_text("Second tab")

label = lv.label(tab3, None)
label.set_text("Third tab");
