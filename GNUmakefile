ARCH ?= x86_64
KERN_FILE = kernel.sys

# Check that the architecture is supported and set relevant variables.
ifeq ($(ARCH),x86_64)
    override EFI_ARCH := X64
else
    $(error Architecture $(ARCH) not supported)
endif

.PHONY: all
all: Nexus.iso

.PHONY: run
run: Nexus.iso
	qemu-system-$(ARCH) -M q35 -m 2G -drive file=Nexus.iso -boot d -monitor stdio --enable-kvm

.PHONY: debug
debug:
	qemu-system-$(ARCH) -M q35 -m 2G -drive file=Nexus.iso -boot d -monitor stdio -d int -D logfile.txt

.PHONY: debug1
debug1:
	qemu-system-$(ARCH) -M q35 -m 2G -drive file=Nexus.iso -boot d -monitor stdio -d int -D logfile.txt -s -S

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
	make -C limine

.PHONY: kernel
kernel:
	cd kernel; bash buildall

Nexus.iso: limine kernel
	cd system/nexd; make; mv nexd.bin ../../base/internals/nexd.sys
	rm -rf iso_root
	mkdir -p iso_root
	mkdir -p iso_root/Nexus
	cp base/limine.cfg \
		limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
	cp kernel/$(KERN_FILE) base/internals/* iso_root/Nexus/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o Nexus.iso
	limine/limine-deploy Nexus.iso
	@ rm -rf iso_root
	bash kernel/clean
	@ rm -rf limine

.PHONY: clean
clean:
	rm -rf iso_root Nexus.iso Nexus.hdd
	bash kernel/clean

.PHONY: distclean
distclean: clean
	rm -rf limine ovmf-$(EFI_ARCH)
	$(MAKE) -C kernel distclean
