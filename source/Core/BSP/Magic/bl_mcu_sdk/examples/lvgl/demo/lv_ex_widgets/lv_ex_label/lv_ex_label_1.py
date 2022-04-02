label1 = lv.label(lv.scr_act(),None)
label1.set_long_mode(lv.label.LONG.BREAK)  # Break the long lines
label1.set_recolor(True)                   # Enable re-coloring by commands in the text
label1.set_align(lv.label.ALIGN.CENTER)        # Center aligned lines
label1.set_text(
"""#0000ff Re-color# #ff00ff words# #ff0000 of a# label
and  wrap long text automatically.""")
label1.set_width(150)
label1.align(None,lv.ALIGN.CENTER, 0, -30)

label2 = lv.label(lv.scr_act(), None)
label2.set_long_mode(lv.label.LONG.SROLL_CIRC)
label2.set_width(150)
label2.set_text("It is a circularly scrolling text. ")
label2.align(None, lv.ALIGN.CENTER, 0, 30)
