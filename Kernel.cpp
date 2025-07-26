#include "TextPrint.cpp"
#include "IDT.cpp"
#include "Assembler.cpp"

//Current working directory
const char *CurrentDir = "C:/Home";

//Test.BA
extern const char AsmCode[];

//Welcome.TXT
extern const char Welcome[];

int LeftShiftPressed = 0;
#define MAX_CMD_LENGTH 128
char CommandBuffer[MAX_CMD_LENGTH];
int CommandLength = 0;

#define MAX_TEDIT_SIZE 1024
char TEditBuffer[MAX_TEDIT_SIZE];
int TEditLength = 0;
bool InTextEditMode = false;

void ProcessCommand(const char* cmd)
{
	if (strcmp(cmd, "help") == 0)
	{
		PutChars("Available commands: help, clear, echo <text>, tedit\n", BACKGROUND_WHITE | FOREGROUND_BLUE);
	}
	else if (strcmp(cmd, "clear") == 0)
	{
		ClearScreen();
		SetCursorPosition(0);
	}
	else if (strncmp(cmd, "echo ", 5) == 0)
	{
		PutChars(cmd + 5, BACKGROUND_WHITE | FOREGROUND_BLUE);
		PutChars("\n");
	}
	else if (strcmp(cmd, "tedit") == 0)
	{
		InTextEditMode = true;
		TEditLength = 0;
		TEditBuffer[0] = '\0';
		PutChars("Write code (ESC to run & exit):\n", BACKGROUND_WHITE | FOREGROUND_GREEN);
	}
	else if (strcmp(cmd, "") == 0)
	{
		//Fuck you
	}
	else
	{
		PutChars("Unknown program or command: ", BACKGROUND_WHITE | FOREGROUND_BLUE);
		PutChars(cmd, BACKGROUND_WHITE | FOREGROUND_RED);
		PutChars("\n");
	}
}

void UppercaseExceptHex(char* buffer) {
	bool inHex = false;
	for (int i = 0; buffer[i] != '\0'; i++) {
		// Detect start of 0x sequence
		if (buffer[i] == '0' && (buffer[i + 1] == 'x' || buffer[i + 1] == 'X')) {
			inHex = true;
			i += 1; // skip 'x'
			continue;
		}

		// If in hex mode, skip until space, comma, newline, or null
		if (inHex) {
			if (buffer[i] == ' ' || buffer[i] == ',' || buffer[i] == '\n' || buffer[i] == '\0') {
				inHex = false;
			}
			continue;
		}

		// Convert lowercase letter to uppercase
		if (buffer[i] >= 'a' && buffer[i] <= 'z') {
			buffer[i] -= 32;
		}
	}
}

void KeyboardHandler(uint8_t scanCode, uint8_t chr)
{
	if (InTextEditMode)
	{
		if (scanCode == 0x01) // ESC key
		{
			InTextEditMode = false;
			TEditBuffer[TEditLength] = '\0';
			PutChars("\nText edit ended. Final content:\n", BACKGROUND_WHITE | FOREGROUND_GREEN);
			PutChars(TEditBuffer, BACKGROUND_WHITE | FOREGROUND_GREEN);
			UppercaseExceptHex(TEditBuffer);
			CompileFromSource(TEditBuffer);
			RunCode();
			PutChars("\n", BACKGROUND_WHITE | FOREGROUND_GREEN);
			PutChars(CurrentDir, BACKGROUND_WHITE | FOREGROUND_BLUE);
			PutChars("> ", BACKGROUND_WHITE | FOREGROUND_BLUE);
			return;
		}
		else if (scanCode == 0x1C) // Enter key
		{
			if (TEditLength < MAX_TEDIT_SIZE - 1)
			{
				TEditBuffer[TEditLength++] = '\n';
				TEditBuffer[TEditLength] = '\0';
				PutChars("\n");
			}
		}
		else if (scanCode == 0x0E) // Backspace
		{
			if (TEditLength > 0)
			{
				TEditLength--;
				TEditBuffer[TEditLength] = '\0';
				SetCursorPosition(CursorPosition - 1);
				PutChar(' ');
				SetCursorPosition(CursorPosition - 1);
			}
		}
		else if (scanCode == 0x3A)
		{
			LeftShiftPressed = !LeftShiftPressed;
		}
		else if (scanCode == 0x2A || scanCode == 0x36)
		{
			LeftShiftPressed = 1;
		}
		else if (scanCode == 0xAA || scanCode == 0xB6)
		{
			LeftShiftPressed = 0;
		}
		else if (scanCode < 128 && TEditLength < MAX_TEDIT_SIZE - 1)
		{
			char finalChar = KBSet1::ScanCodeLookupTable[scanCode];
			if (finalChar == 0) return; // unsupported key

			if (LeftShiftPressed)
			{
				if (finalChar == '\'')
					finalChar = '"';
				else if (finalChar == '-')
					finalChar = '_';
				else
					finalChar -= 32;
			}
			TEditBuffer[TEditLength++] = finalChar;
			TEditBuffer[TEditLength] = '\0';
			PutChar(finalChar);
		}
		return;
	}

	if (chr != 0)
	{
		if (CommandLength >= MAX_CMD_LENGTH - 1)
			return;

		char finalChar = chr;
		if (LeftShiftPressed)
			finalChar -= 32;

		CommandBuffer[CommandLength++] = finalChar;
		CommandBuffer[CommandLength] = '\0';
		PutChar(finalChar);
	}
	else
	{
		switch (scanCode)
		{
			case 0x8E:
				if (CommandLength > 0)
				{
					CommandLength--;
					SetCursorPosition(CursorPosition - 1);
					PutChar(' ');
					SetCursorPosition(CursorPosition - 1);
				}
				break;

			case 0x2A: LeftShiftPressed = 1; break;
			case 0xAA: LeftShiftPressed = 0; break;
			case 0x3A: LeftShiftPressed = !LeftShiftPressed; break;

			case 0x9C:
				PutChars("\n");
				ProcessCommand(CommandBuffer);
				CommandLength = 0;
				CommandBuffer[0] = '\0';
				if (!InTextEditMode) {
					PutChars(CurrentDir, BACKGROUND_WHITE | FOREGROUND_BLUE);
					PutChars("> ", BACKGROUND_WHITE | FOREGROUND_BLUE);
				}
				break;
		}
	}
}

//Entry point, we have to use the extern C because it for some reason hates C++.
extern "C" void _start()
{
	//Clear the screen
	ClearScreen();

	//Welcome banner
	SetCursorPosition(PositionFromCoords(24, 0));
	PutChars("      ScantrumOS V0.01            \n", BACKGROUND_WHITE | FOREGROUND_MAGENTA);
	SetCursorPosition(PositionFromCoords(24, 1));
	PutChars("Public Domain Operating System\n", BACKGROUND_WHITE | FOREGROUND_BLUE);
	SetCursorPosition(PositionFromCoords(0, 3));

	//Init the interrupt descriptor table
	InitializeIDT();

	//Init drivers
	MainKeyboardHandler = KeyboardHandler;

	//System text
	PutChars(Welcome, BACKGROUND_WHITE | FOREGROUND_BLUE);
	PutChars(AsmCode, BACKGROUND_WHITE | FOREGROUND_BLUE);
	PutChars("\n");

	//Compile the assembly and run it
	CompileFromSource(AsmCode);
	RunCode();

	//Print the working directory
	PutChars(CurrentDir, BACKGROUND_WHITE | FOREGROUND_BLUE);
	PutChars("> ", BACKGROUND_WHITE | FOREGROUND_BLUE);

	//Just halt
	for(;;);

	//We should never get here, hopefully
	return;
}