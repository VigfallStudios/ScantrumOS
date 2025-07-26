#pragma once
#include "IO.cpp"
#include "Typedefs.cpp"
#include "TextModeColorCodes.cpp"
#define VGA_MEMORY (uint8_t*)0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

uint16_t CursorPosition;

void SetCursorPosition(uint16_t position)
{
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(position & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));

	CursorPosition = position;
}

uint16_t PositionFromCoords(uint8_t x, uint8_t y)
{
	return y * VGA_WIDTH + x;
}

void PutChars(const char *str, uint8_t color = BACKGROUND_WHITE | FOREGROUND_BLACK)
{
	uint8_t *charPtr = (uint8_t*)str;
	uint16_t index = CursorPosition;
	while (*charPtr != 0)
	{
		if (*charPtr == 10)
		{
			index += VGA_WIDTH;
			index -= index % VGA_WIDTH;
		}
		else
		{
			*(VGA_MEMORY + index * 2) = *charPtr;
			*(VGA_MEMORY + index * 2 + 1) = color;
			index++;
		}
		charPtr++;
	}
	SetCursorPosition(index);
}

void PutChar(char chr, uint8_t color = BACKGROUND_WHITE | FOREGROUND_BLACK)
{
	*(VGA_MEMORY + CursorPosition * 2) = chr;
	*(VGA_MEMORY + CursorPosition * 2 + 1) = color;
	SetCursorPosition(CursorPosition + 1);	
}

char hexToStr[128];

//This is some fucked CPP shit but it works and it reduces some stuff for me
template<typename T>
const char* HexToString(T value)
{
	T* valPtr = &value;
	uint8_t* ptr;
	uint8_t temp;
	uint8_t size = (sizeof(T)) * 2 - 1;
	uint8_t i;
	for (i = 0; i < size; i++)
	{
		ptr = ((uint8_t*)valPtr + i);
		temp = ((*ptr & 0xF0) >> 4);
		hexToStr[size - (i * 2 + 1)] = temp + (temp > 9 ? 55 : 48);
		temp = ((*ptr & 0x0F));
		hexToStr[size - (i * 2 + 0)] = temp + (temp > 9 ? 55 : 48);
	}
	hexToStr[size + 1] = 0;
	return hexToStr;
}

void ClearScreen(uint64_t color = BACKGROUND_WHITE | FOREGROUND_BLACK)
{
	//Clever 64-bit stuff that reduces loop intervals which results in less CPU time
	//and that is faster.
	//Take that, OSDev fuckers!
	uint64_t value = 0;
	value += color << 8;
	value += color << 24;
	value += color << 40;
	value += color << 56;

	for (uint64_t* i = (uint64_t*)VGA_MEMORY; i < (uint64_t*)(VGA_MEMORY + 4000); i++)
	{
		*i = value;
	}

	SetCursorPosition(PositionFromCoords(0, 0));
}