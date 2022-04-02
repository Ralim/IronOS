from lv_colors import lv_colors

MASK_WIDTH=100
MASK_HEIGHT=50

# Create the mask of a text by drawing it to a canvas
mask_map = bytearray (MASK_WIDTH * MASK_HEIGHT *4)

# Create a "8 bit alpha" canvas and clear it
canvas = lv.canvas(lv.scr_act(), None)
canvas.set_buffer(mask_map, MASK_WIDTH, MASK_HEIGHT, lv.img.CF.ALPHA_8BIT)
canvas.fill_bg(LV_COLOR_BLACK, lv.OPA.TRANSP)

# Draw a label to the canvas. The result "image" will be used as mask
label_dsc = lv.draw_label_dsc_t()
label_dsc.init()
label_dsc.color = LV_COLOR_WHITE
canvas.draw_text(5, 5, MASK_WIDTH, label_dsc, "Text with gradient", lv.label.ALIGN.CENTER)

# The mask is read, the canvas is not required anymore
canvas.delete()

# Create an object mask which will use the created mask
om = lv.objmask(lv.scr_act(), None)
om.set_size(MASK_WIDTH, MASK_HEIGHT)
om.align(None, lv.ALIGN.CENTER, 0, 0)

# Add the created mask map to the object mask
m = lv.draw_mask_map_param_t()
a=lv.area_t()
a.x1 = 0
a.y1 = 0
a.x2 = MASK_WIDTH - 1
a.y2 = MASK_HEIGHT - 1

m.init(a,mask_map)
om.add_mask(m)

# Create a style with gradient
style_bg= lv.style_t()
style_bg.init()
style_bg.set_bg_opa(lv.STATE.DEFAULT, lv.OPA.COVER)
style_bg.set_bg_color(lv.STATE.DEFAULT, lv_colors.RED)
style_bg.set_bg_grad_color(lv.STATE.DEFAULT, lv_colors.BLUE)
style_bg.set_bg_grad_dir(lv.STATE.DEFAULT, lv.GRAD_DIR.HOR)

# Create and object with the gradient style on the object mask.
# The text will be masked from the gradient
bg = lv.obj(om, None)
bg.reset_style_list(lv.obj.PART.MAIN)
bg.add_style(lv.obj.PART.MAIN, style_bg)
bg.set_size(MASK_WIDTH, MASK_HEIGHT)

