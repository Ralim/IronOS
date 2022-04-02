lmeter = lv.linemeter(lv.scr_act(),None)
lmeter.set_range(0,100)         # Set the range
lmeter.set_value(80)            # Set the current value
lmeter.set_scale(240,21)        # Set the angle and number of lines
lmeter.set_size(150,150)
lmeter.align(None,lv.ALIGN.CENTER,0,0)
