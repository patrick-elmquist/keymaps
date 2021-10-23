USER = pket
KEYBOARDS = lily58 kyria 

PATH_lily58 = lily58
PATH_kyria = splitkb/kyria

all: $(KEYBOARDS)

.PHONY: $(KEYBOARDS)
$(KEYBOARDS):
	# init submodule
	git submodule update --init --recursive

	# cleanup old symlinks
	rm -rf qmk_firmware/keyboards/$(PATH_$@)/keymaps/$(USER)
	rm -rf qmk_firmware/users/$(USER)

	# add new symlinks
	ln -s $(shell pwd)/user qmk_firmware/users/$(USER)
	ln -s $(shell pwd)/$@ qmk_firmware/keyboards/$(PATH_$@)/keymaps/$(USER)

	# run lint check
	cd qmk_firmware; qmk lint -km $(USER) -kb $(PATH_$@) --strict

	# run build
	make BUILD_DIR=$(shell pwd)/build -j1 -C qmk_firmware $(PATH_$@):$(USER)

	# cleanup symlinks
	rm -rf qmk_firmware/keyboards/$(PATH_$@)/keymaps/$(USER)
	rm -rf qmk_firmware/users/$(USER)

clean:
	rm -rf ./build/
	rm -rf qmk_firmware/keyboards/$(PATH_lily58)/keymaps/$(USER)
	rm -rf qmk_firmware/keyboards/$(PATH_kyria)/keymaps/$(USER)
	rm -rf qmk_firmware/users/$(USER)

