# atktool - Alientek T90 firmware image toolkit.
# pack/unpack/info build with no dependencies; probe/flash additionally need hidapi.
#   make                       # auto-detects hidapi via pkg-config
#   brew install hidapi        # (macOS) or: apt install libhidapi-dev  (Debian/Ubuntu)

CC      ?= cc
CFLAGS  ?= -O2 -Wall -Wextra -std=c11
BIN      = atktool

HIDAPI_CFLAGS := $(shell pkg-config --cflags hidapi 2>/dev/null || pkg-config --cflags hidapi-hidraw 2>/dev/null)
HIDAPI_LIBS   := $(shell pkg-config --libs   hidapi 2>/dev/null || pkg-config --libs   hidapi-hidraw 2>/dev/null)

ifneq ($(strip $(HIDAPI_LIBS)),)
CFLAGS += -DUSE_HIDAPI $(HIDAPI_CFLAGS)
LIBS   += $(HIDAPI_LIBS)
$(info atktool: building WITH hidapi (probe/flash enabled))
else
$(info atktool: building WITHOUT hidapi (pack/unpack/info only; install hidapi for probe/flash))
endif

$(BIN): atktool.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(BIN)

.PHONY: clean
