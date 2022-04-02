#!/opt/bin/lv_micropython
import lvgl as lv
import init_gui
import time

cpicker = lv.cpicker(lv.scr_act(),None)
cpicker.set_type(lv.cpicker.TYPE.RECT);
cpicker.set_size(200, 50);
cpicker.align(None, lv.ALIGN.CENTER, 0, 0)

