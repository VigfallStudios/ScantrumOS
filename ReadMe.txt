            ScantrumOS

ScantrumOS is 64-bit and will not run on 32-bit hardware.

To get started, point your virtual machine to the ScantrumOS.img and boot it.

I advise you use QEMU as it is also used for testing the operating system
during development.

            Debugging Steps

If you get stuck on a blue screen, the jump to C did not happen. This is
most likely caused by not reading enough sectors. Check the bootloader if
it's reading not enough sectors or your disk is not big enough.

If you see "READ FAILED" that means the interrupt 0x13 has failed, this
can be caused for a number of reasons.

If you just see a H printed on the top left of the screen, that means
your CPU does not support CPUID or Long Mode.

            System folder structure
I don't fucking know lol