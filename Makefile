BUILD_DIR=build
BUILD_FILE=$(BUILD_DIR)/shit.txt
INSTALL_DIR=$(DESTDIR)/usr/local/share/doc/shit

.PHONY: shit install clean

shit:
	@mkdir -p $(BUILD_DIR)
	@echo "💩" > $(BUILD_FILE)
	@cp README.md $(BUILD_DIR)/
	@echo "💩 built at $(BUILD_FILE)"

install: shit
	@mkdir -p $(INSTALL_DIR)
	@cp -r $(BUILD_DIR) $(INSTALL_DIR)
	@echo "💩 installed to" $(INSTALL_DIR)

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned the 💩"
