# Hardware Issues

While we would love everything to work perfectly, sometimes that just doesn't happen.
Please do not email maintainers directly, these will generally be ignored.
Keep issue discussions to GitHub issues or the discussions page so that the whole community can help and work together.

## No Accelerometer detected

If your iron was previously working, this could be a bug (and we are very sorry). Please check the currently open and recently closed issues to check if anyone else has run into this. You can try going back to a release on the firmware to test if this is a new issue before opening an issue.

If this is a new iron, also feel free to open an issue if you don't see any; a vendor _could_ have changed the model of the accelerometer on us without warning _again_. In which case, support should come shortly.

If your iron is new, there is a slim chance your accelerometer may be DOA and need replacement.

**Note this warning will only be shown the first few times until settings are reset**

## No USB-PD IC detected

Generally, this means either something went very awry in the firmware, or the chip is not answering as would normally be expected. Try rolling back to an earlier release to confirm if the issue still persists then the device may need repair. If you have some form of seller protection/support, you most likely want to reach out to this to be safe. If you don't, you can always attempt to replace the IC yourself. As of writing both the TS80P and Pinecil use the FUSB302.

**Note this warning will only be shown the first few times until settings are reset**

## No tip detected

if your tip is not being detected, the most likely cause is that the heater element has been damaged from over-temperature, being dropped or bad luck. As the heater coil is part of the temperature measurement circuit neither will work if it's damaged.

The best way to see if this is the case is to measure the resistance across the contacts to the tip using a multimeter.
you are expecting to see measurements in the range of 4-10 ohms. Anything higher than 10 ohms is _generally_ an issue.
