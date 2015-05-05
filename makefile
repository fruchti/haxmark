PROJECT = main
LOCAL_SOURCE_DIR = src
BUILD_DIR = build

MCU = attiny2313
# internal oscillator
# LFUSE = 0xe4
# crystal
LFUSE = 0xff
HFUSE = 0xdf
EFUSE = 0xff

FORMAT = ihex

LOCAL_SOURCES = $(wildcard $(LOCAL_SOURCE_DIR)/*.c)

CFLAGS = -mmcu=$(MCU) -Wall -Wstrict-prototypes -gstabs -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Os -lm

RM = rm -f
CC = avr-gcc
NM = avr-nm
AVRDUDE = avrdude
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

LOCAL_OBJECTS = $(patsubst $(LOCAL_SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(LOCAL_SOURCES))
OBJECTS = $(LOCAL_OBJECTS)
LOCAL_DEPENDS = $(patsubst $(LOCAL_SOURCE_DIR)/%.c, $(BUILD_DIR)/%.d, $(LOCAL_SOURCES))

.DEFAULT_GOAL = all
.DELETE_ON_ERROR:

$(LOCAL_OBJECTS): $(BUILD_DIR)/%.o: $(LOCAL_SOURCE_DIR)/%.c
	$(CC) $^ -c -o $@ $(CFLAGS)

$(LOCAL_DEPENDS): $(BUILD_DIR)/%.d: $(LOCAL_SOURCE_DIR)/%.c
	@set -e; rm -f $@; $(CC) -MM $(CFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

include $(LOCAL_DEPENDS) 

$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(OBJECTS) $(CFLAGS) --output $@

$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf | $(BUILD_DIR)
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

$(BUILD_DIR)/$(PROJECT).eep: $(BUILD_DIR)/$(PROJECT).elf | $(BUILD_DIR)
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O $(FORMAT) $< $@

$(BUILD_DIR)/$(PROJECT).lss: $(BUILD_DIR)/$(PROJECT).elf | $(BUILD_DIR)
	$(OBJDUMP) -h -S $< > $@

$(BUILD_DIR)/$(PROJECT).sym: $(BUILD_DIR)/$(PROJECT).elf | $(BUILD_DIR)
	$(NM) -n $< > $@

$(LOCAL_DEPENDS): | $(BUILD_DIR)
$(BUILD_DIR):
	if [ ! -d "$(BUILD_DIR)" ]; then mkdir "$(BUILD_DIR)"; fi

.PHONY: all
all: $(BUILD_DIR) $(OBJECTS) $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(PROJECT).eep $(BUILD_DIR)/$(PROJECT).lss $(BUILD_DIR)/$(PROJECT).sym
	$(SIZE) $(BUILD_DIR)/$(PROJECT).elf

.PHONY: program
program: $(BUILD_DIR)/$(PROJECT).hex
	avrdude -c usbasp -p $(MCU) -U flash:w:$(BUILD_DIR)/$(PROJECT).hex -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m 
	# avrdude -c usbasp -p $(MCU) -U flash:w:$(BUILD_DIR)/$(PROJECT).hex -U eeprom:w:$(BUILD_DIR)/$(PROJECT).eep -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m 

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)/*
