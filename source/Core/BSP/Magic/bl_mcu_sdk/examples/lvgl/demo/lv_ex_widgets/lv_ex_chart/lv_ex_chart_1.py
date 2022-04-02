from lv_colors import lv_colors

# create a chart
chart = lv.chart(lv.scr_act(),None)
chart.set_size(200,150)
chart.align(None,lv.ALIGN.CENTER,0,0)
chart.set_type(lv.chart.TYPE.LINE)

ser1=chart.add_series(lv_colors.RED)
ser2=chart.add_series(lv_colors.GREEN)

# Set next points on ser1
chart.set_next(ser1,10)
chart.set_next(ser1,10)
chart.set_next(ser1,10)
chart.set_next(ser1,10)
chart.set_next(ser1,10)
chart.set_next(ser1,10)
chart.set_next(ser1,10)
chart.set_next(ser1,30)
chart.set_next(ser1,70)
chart.set_next(ser1,90)

# Set points on ser2
chart.set_points(ser2, [90, 70, 65, 65, 65, 65, 65, 65, 65, 65])
