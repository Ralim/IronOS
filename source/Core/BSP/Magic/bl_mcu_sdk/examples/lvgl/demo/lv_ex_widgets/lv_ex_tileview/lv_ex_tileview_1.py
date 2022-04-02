LV_HOR_RES=240
LV_VER_RES=240

# create a tileview
valid_pos =  [{"x":0, "y": 0}, {"x": 0, "y": 1}, {"x": 1,"y": 1}]
tileview = lv.tileview(lv.scr_act())
tileview.set_valid_positions(valid_pos, len(valid_pos))
tileview.set_edge_flash(True)

tile1 = lv.obj(tileview)
tile1.set_size(LV_HOR_RES, LV_VER_RES)
tileview.add_element(tile1)

# Tile1: just a label
label = lv.label(tile1)
label.set_text("Scroll Down")
label.align(None, lv.ALIGN.CENTER, 0, 0)

# Tile2: a list
lst = lv.list(tileview)
lst.set_size(LV_HOR_RES, LV_VER_RES)
lst.set_pos(0, LV_VER_RES)
lst.set_scroll_propagation(True)
lst.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
tileview.add_element(lst)

list_btn = lst.add_btn(None, "One")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Two")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Three")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Four")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Five")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Six")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Seven")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Eight")
tileview.add_element(list_btn)

# Tile3: a button
tile3 = lv.obj(tileview, tile1)
tile3.set_pos(LV_HOR_RES, LV_VER_RES)
tileview.add_element(tile3)

btn = lv.btn(tile3)
btn.align(None, lv.ALIGN.CENTER, 0, 0)

label = lv.label(btn)
label.set_text("No scroll up")

