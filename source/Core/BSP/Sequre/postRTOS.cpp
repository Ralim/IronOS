#include "BSP.h"

// Initialisation to be performed with scheduler active.
// Note: this runs at the start of the power thread, right before the USB-PD negotiation is kicked off, so it must
// not block - the start-up chirp lives in the GUI thread (after the boot logo) for that reason.
void postRToSInit() {}
