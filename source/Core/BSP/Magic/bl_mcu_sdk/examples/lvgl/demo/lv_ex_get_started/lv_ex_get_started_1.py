class counting_button(lv.btn):
    def __init__(self,parent):
        super().__init__(parent)
        self.set_pos(10, 10)                  # Set its position
        self.set_size(120, 50 )               #  Set its size
        self.set_event_cb(self.btn_event_cb)       # Assign a callback to the button
        self.label = lv.label(self, None)     # Add a label to the button
        self.label.set_text("Button")         # Set the labels text
        self.cnt=0

    def btn_event_cb(self,btn,event):
        if event == lv.EVENT.CLICKED:
            self.cnt += 1
            # Get the first child of the button which is the label and change its text
            label = btn.get_child(None)
            label.set_text("Button: %d"%self.cnt)

#
# Create a button with a label and react on Click event
#

btn = counting_button(lv.scr_act())



