CPPFLAGS = -std=c99 -g -O0 -Wall -Wextra -Wpedantic

BUILD_DIR = build
BUILD_GUARD = mkdir -p $(BUILD_DIR)

TARGET = $(BUILD_DIR)/server

SOURCE_FILES = $(shell find src/ -name '*.c')
OBJECT_FILES = $(SOURCE_FILES:src/%.c=$(BUILD_DIR)/%.o)
DEPEND_FILES = $(OBJECT_FILES:.o=.d)

.PHONY: clean run

$(TARGET): $(OBJECT_FILES)
	@$(BUILD_GUARD)
	gcc $(OBJECT_FILES) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.c
	@$(BUILD_GUARD)
	gcc $(CPPFLAGS) -MMD -MP $< -c -o $@

clean:
	rm -r $(BUILD_DIR)
	rm $(TARGET)

run: $(TARGET)
	./$(TARGET)

-include $(DEPEND_FILES)
