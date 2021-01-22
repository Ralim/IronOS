### CJK Notes

Unlike Latin and Cyrillic scripts, CJK Unified Ideographs cannot be legibly
displayed using the small font, which is only 6x8px in size. Therefore, Hanzi,
Kanji and Hanja can only be displayed using the 12x16px large font.

By default, menu items are shown using two lines of text with the small font.
When translating such items for CJK, leave the first line empty and put the
translated text on the second line. This way, the firmware will automatically
know to display the text using the large font. This also applies to the
`SettingsResetMessage` text -- just start the message with `\n`.
