table = lv.table(lv.scr_act(), None)
table.set_col_cnt(2)
table.set_row_cnt(4)
table.set_size(200,200)
table.align(lv.scr_act(), lv.ALIGN.CENTER, -15, 0)

# Make the cells of the first row center aligned
table.set_col_width(0,100)
table.set_cell_align(0, 0, lv.label.ALIGN.CENTER)
table.set_cell_align(0, 1, lv.label.ALIGN.CENTER)

# Align the price values to the right in the 2nd column
table.set_col_width(1,100)
table.set_cell_align(1, 1, lv.label.ALIGN.RIGHT)
table.set_cell_align(2, 1, lv.label.ALIGN.RIGHT)
table.set_cell_align(3, 1, lv.label.ALIGN.RIGHT)

table.set_cell_type(0, 0, 2)
table.set_cell_type(0, 1, 2)

# Fill the first column
table.set_cell_value(0, 0, "Name")
table.set_cell_value(1, 0, "Apple")
table.set_cell_value(2, 0, "Banana")
table.set_cell_value(3, 0, "Citron")

# Fill the second column
table.set_cell_value(0, 1, "Price")
table.set_cell_value(1, 1, "$7")
table.set_cell_value(2, 1, "$4")
table.set_cell_value(3, 1, "$6")

