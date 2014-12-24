bin/netcp: src/netcp.h src/netcp.c
	@mkdir -p bin
	gcc src/netcp.c \
		-o bin/netcp \
		-Wall

clean:
	@rm -r bin

.PHONY: android
android: bin/netcp-arm

bin/netcp-arm: src/netcp.h src/netcp.c toolchain
	@mkdir -p bin
	toolchain/bin/arm-linux-androideabi-gcc \
		--sysroot=toolchain/sysroot \
	  src/netcp.c \
		-o $@ \
		-Wall

toolchain:
	# TODO: Err if ANDROID_NDK not set.
	${ANDROID_NDK}/build/tools/make-standalone-toolchain.sh \
		--platform=android-18 \
		--toolchain=arm-linux-androideabi-4.8 \
		--install-dir=${PWD}/toolchain
