class TextArea():
    def __init__(self):
        self.i=0
        # create a textarea
        self.ta = lv.textarea(lv.scr_act(),None)
        self.ta.set_size(200,100)
        self.ta.align(None,lv.ALIGN.CENTER,0,0)
        self.ta.set_text("A text in a Text Area")
        self.ta.set_event_cb(self.event_handler)

    def event_handler(self,source,event):
        if  event == lv.EVENT.VALUE_CHANGED:
            print("i= ",self.i)
            print("Value:",self.ta.get_text())
        elif event == lv.EVENT.LONG_PRESSED_REPEAT:
            # For simple test: Long press the Text are to add the text below
            txt = "\n\nYou can scroll it if the text is long enough.\n\n"
            if self.i < len(txt):
                self.ta.add_char(ord(txt[self.i]))
                self.i += 1

TextArea()

