# Create three labels to demonstrate the alignments.
labels = []

# `label.set_align` is not required to align the object itslef.
# It's used only when the text has multiple lines

# Create a label on the top.
# No additional alignment so it will be the reference
label = lv.label(lv.scr_act())
label.align(None, lv.ALIGN.IN_TOP_MID, 0, 5)
label.set_align(lv.label.ALIGN.CENTER)
labels.append(label)

# Create a label in the middle.
# `obj.align` will be called every time the text changes
# to keep the middle position
label = lv.label(lv.scr_act())
label.align(None, lv.ALIGN.CENTER, 0, 0)
label.set_align(lv.label.ALIGN.CENTER)
labels.append(label)

# Create a label in the bottom.
# Enable auto realign.
label = lv.label(lv.scr_act())
label.set_auto_realign(True)
label.align(None, lv.ALIGN.IN_BOTTOM_MID, 0, -5)
label.set_align(lv.label.ALIGN.CENTER)
labels.append(label)

class TextChanger:
    """Changes texts of all labels every second"""
    def __init__(self, labels,
                texts=["Text", "A very long text", "A text with\nmultiple\nlines"],
                rate=1000):
        self.texts = texts
        self.labels = labels
        self.rate = rate
        self.counter = 0

    def start(self):
        lv.task_create(self.task_cb, self.rate, lv.TASK_PRIO.LOWEST, None)

    def task_cb(self, task):
        for label in labels:
            label.set_text(self.texts[self.counter])

        # Manually realaign `labels[1]`
        if len(self.labels) > 1:
            self.labels[1].align(None, lv.ALIGN.CENTER, 0, 0)

        self.counter = (self.counter + 1) % len(self.texts)

text_changer = TextChanger(labels)
text_changer.start()

