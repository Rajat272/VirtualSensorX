.PHONY: all driver userspace clean test

all: driver userspace

driver:
	mkdir -p /tmp/vs_build && cp -r ./* /tmp/vs_build/ && $(MAKE) -C /usr/src/linux-headers-7.0.0-34-generic M=/tmp/vs_build/driver modules W=1 && cp /tmp/vs_build/driver/virtualsensor.ko driver/

userspace:
	cmake -S userspace -B userspace/build
	cmake --build userspace/build -j

test: userspace
	ctest --test-dir userspace/build --output-on-failure
	./userspace/build/vsctl --mock test

clean:
	rm -rf userspace/build driver/*.ko driver/*.o driver/*.mod* driver/*.symvers driver/*.order
