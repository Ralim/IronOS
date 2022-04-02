from lv_colors import lv_colors

# needle colors
needle_colors=[lv_colors.BLUE,lv_colors.ORANGE,lv_colors.PURPLE]

# create the gauge
gauge1=lv.gauge(lv.scr_act(),None)
gauge1.set_needle_count(3, needle_colors)
gauge1.set_size(200,200)
gauge1.align(None,lv.ALIGN.CENTER,0,0)

# Set the values
gauge1.set_value(0, 10)
gauge1.set_value(1, 20)
gauge1.set_value(2, 50)

