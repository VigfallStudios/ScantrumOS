#pragma once
#include "Typedefs.cpp"
#include "IO.cpp"
#include "KBScanCodeSet1.cpp"

struct IDT_64
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t types_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

//Weird approach of keeping the IDT thing in the linker script, but it works
extern IDT_64 _idt[256];
extern uint64_t isr1;
extern "C" void LoadIDT();

void InitializeIDT()
{
    //Map the IRQ1 to our keyboard handler
    //TODO: Make an IDT Set Gate function to make my life easier
	_idt[1].zero = 0;
	_idt[1].offset_low = (uint16_t)(((uint64_t)&isr1 & 0x000000000000ffff));
	_idt[1].offset_mid = (uint16_t)(((uint64_t)&isr1 & 0x00000000ffff0000) >> 16);
	_idt[1].offset_high = (uint32_t)(((uint64_t)&isr1 & 0xffffffff00000000) >> 32);
	_idt[1].ist = 0;
	_idt[1].selector = 0x08;
	_idt[1].types_attr = 0x8e;

    RemapPic();

    outb(0x21, 0xfd);
    outb(0xa1, 0xff);
    LoadIDT();
}

void(*MainKeyboardHandler)(uint8_t scanCode, uint8_t chr);

//Keyboard driver
extern "C" void isr1_handler()
{
    uint8_t scanCode = inb(0x60);
    uint8_t chr = 0;

    //Check if a key has been pressed, not released
    if (scanCode < 0x3A)
        chr = KBSet1::ScanCodeLookupTable[scanCode];

    if (MainKeyboardHandler != 0)
        MainKeyboardHandler(scanCode, chr);

    //PIC stuff
    outb(0x20, 0x20);
    outb(0xa0, 0x20);
}