firmware:
	git submodule update --init --recursive

	rm -rf qmk_firmware/keyboards/splitkb/kyria/keymaps/pket
	rm -rf qmk_firmware/keyboards/lily58/keymaps/pket
	rm -rf qmk_firmware/users/pket

	ln -s $(shell pwd)/kyria qmk_firmware/keyboards/splitkb/kyria/keymaps/pket
	ln -s $(shell pwd)/lily58 qmk_firmware/keyboards/lily58/keymaps/pket
	ln -s $(shell pwd)/user qmk_firmware/users/pket

	cd qmk_firmware; qmk lint -km pket -kb splitkb/kyria --strict
	cd qmk_firmware; qmk lint -km pket -kb lily58 --strict

	make BUILD_DIR=$(shell pwd) -j4 -C qmk_firmware all:pket

	rm -rf qmk_firmware/keyboards/splitkb/kyria/keymaps/pket
	rm -rf qmk_firmware/keyboards/lily58/keymaps/pket
	rm -rf qmk_firmware/users/pket