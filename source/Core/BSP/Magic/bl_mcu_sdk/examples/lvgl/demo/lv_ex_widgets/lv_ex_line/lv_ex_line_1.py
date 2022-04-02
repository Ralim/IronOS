from lv_colors import lv_colors
import time

# Create an array for the points of the line
line_points = [ {"x":5, "y":5},
                {"x":70, "y":70},
                {"x":120, "y":10},
                {"x":180, "y":60},
                {"x":240, "y":10}]

# Create new style (thick dark blue)
style_line = lv.style_t()
style_line.init()
style_line.set_line_width(lv.STATE.DEFAULT, 8)
style_line.set_line_color(lv.STATE.DEFAULT, lv_colors.BLUE)
style_line.set_line_rounded(lv.STATE.DEFAULT, True)

# Copy the previous line and apply the new style
line1 = lv.line(lv.scr_act())
line1.set_points(line_points, len(line_points))      # Set the points
line1.add_style(lv.line.PART.MAIN, style_line)
line1.align(None, lv.ALIGN.CENTER, 0, 0)

while True:
    lv.task_handler()
    time.sleep_ms(5)
