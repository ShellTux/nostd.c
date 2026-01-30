TARGETS = \
					$(TARGET_DIR)/sleep

DEPS := src/sleep.c
$(TARGET_DIR)/sleep: $(DEPS:%.c=$(BUILD_DIR)/%.o)
	@mkdir --parents $(shell dirname $@)
	$(CC) -o $@ $^ $(LDFLAGS)
