# Create a LED and switch it OFF
led1 = lv.led(lv.scr_act(),None)
led1.align(None,lv.ALIGN.CENTER,-80,0)
led1.off()

# Copy the previous LED and set a brightness
led2=lv.led(lv.scr_act(), led1)
led2.align(None,lv.ALIGN.CENTER,0,0)
led2.set_bright(190)

# Copy the previous LED and switch it ON
led3=lv.led(lv.scr_act(), led1)
led3.align(None,lv.ALIGN.CENTER,80,0)
led3.on()

