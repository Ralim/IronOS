from lv_colors import lv_colors

# create a chart
chart = lv.chart(lv.scr_act(),None)
chart.set_size(200,150)
chart.align(None,lv.ALIGN.CENTER,0,0)
chart.set_type(lv.chart.TYPE.LINE)
# Add a faded are effect
chart.set_style_local_bg_opa(lv.chart.PART.SERIES, lv.STATE.DEFAULT, lv.OPA._50)               # Max. opa.
chart.set_style_local_bg_grad_dir(lv.chart.PART.SERIES, lv.STATE.DEFAULT, lv.GRAD_DIR.VER)
chart.set_style_local_bg_main_stop(lv.chart.PART.SERIES, lv.STATE.DEFAULT, 255)                # Max opa on the top
chart.set_style_local_bg_grad_stop(lv.chart.PART.SERIES, lv.STATE.DEFAULT, 0)                  # Transparent on the bottom

ser1=chart.add_series(lv_colors.RED)
ser2=chart.add_series(lv_colors.GREEN)

# Set next points on ser1
chart.set_next(ser1,31)
chart.set_next(ser1,66)
chart.set_next(ser1,10)
chart.set_next(ser1,89)
chart.set_next(ser1,63)
chart.set_next(ser1,56)
chart.set_next(ser1,32)
chart.set_next(ser1,35)
chart.set_next(ser1,57)
chart.set_next(ser1,85)

# Set points on ser2
chart.set_points(ser2, [92, 71, 61, 15, 21, 35, 35, 58, 31, 53])
