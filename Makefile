PROGRAM = esp-homekit-servo-switch
PROGRAM_SRC_DIR = src

EXTRA_COMPONENTS = \
	extras/http-parser \
	$(abspath components/wolfssl) \
	$(abspath components/cjson) \
	$(abspath components/homekit)

FLASH_SIZE ?= 32
ESPPORT = rfc2217://192.168.31.172:4000
ESPBAUD = 921600

EXTRA_CFLAGS += -I../.. -DHOMEKIT_SHORT_APPLE_UUIDS

include $(SDK_PATH)/common.mk

test: flash
	miniterm.py $(ESPPORT) 115200

monitor:
	miniterm.py $(ESPPORT) 115200
