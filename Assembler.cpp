#pragma once

#include "Typedefs.cpp"
#include "TextPrint.cpp"

//Output buffer
uint8_t MachineCodeBuffer[64];
int MachineCodeSize = 0;

//Simple hex parser
uint64_t ParseHex(const char* str)
{
    uint64_t val = 0;
    while (*str)
    {
        val <<= 4;
        if (*str >= '0' && *str <= '9') val |= *str - '0';
        else if (*str >= 'A' && *str <= 'F') val |= *str - 'A' + 10;
        else if (*str >= 'a' && *str <= 'f') val |= *str - 'a' + 10;
        str++;
    }
    return val;
}

//Literal string buffer
const char* LiteralPool[] = {
    "GOD", //index 0
};

void PutChars_Asm(const char *str)
{
    PutChars(str);
}

void CompileFromSource(const char *src)
{
    MachineCodeSize = 0;

    //A buffer for each line
    char line[128];
    int i = 0;

    while (*src)
    {
        //Read one line
        i = 0;
        while (*src && *src != '\n' && i < sizeof(line) - 1)
            line[i++] = *src++;
        line[i] = 0;

        //Skip newline
        if (*src == '\n') src++;

        //Skip empty lines
        if (line[0] == 0) continue;

        //Compile this line
        if (strncmp(line, "MOV RAX, 0x", 11) == 0)
        {
            const char* immStr = line + 11;
            uint64_t imm = ParseHex(immStr);

            if (MachineCodeSize + 10 > sizeof(MachineCodeBuffer) - 1)
                break;

            MachineCodeBuffer[MachineCodeSize++] = 0x48;
            MachineCodeBuffer[MachineCodeSize++] = 0xB8;
            for (int j = 0; j < 8; j++)
                MachineCodeBuffer[MachineCodeSize++] = (imm >> (j * 8)) & 0xFF;
        }
        else if (strncmp(line, "MOV RAX, \"", 10) == 0)
        {
            const char* quote_start = line + 10;
            const char* quote_end = strchr(quote_start, '"');
            if (!quote_end) {
                PutChars("Syntax error: missing closing quote\n");
                return;
            }

            char literal[64] = {};
            uint64_t len = quote_end - quote_start;
            if (len >= sizeof(literal)) len = sizeof(literal) - 1;
            strncpy(literal, quote_start, len);

            static char static_literal[64];
            strcpy(static_literal, literal);

            uint64_t addr = (uint64_t)static_literal;

            if (MachineCodeSize + 10 > sizeof(MachineCodeBuffer) - 1)
                break;

            MachineCodeBuffer[MachineCodeSize++] = 0x48;
            MachineCodeBuffer[MachineCodeSize++] = 0xB8;
            for (int j = 0; j < 8; j++)
                MachineCodeBuffer[MachineCodeSize++] = (addr >> (j * 8)) & 0xFF;
        }
        else if (strncmp(line, "CALL PUT_CHARS", 14) == 0)
        {
            if (MachineCodeSize + 15 > sizeof(MachineCodeBuffer) - 1)
                break;

            //mov rdi, rax
            MachineCodeBuffer[MachineCodeSize++] = 0x48;
            MachineCodeBuffer[MachineCodeSize++] = 0x89;
            MachineCodeBuffer[MachineCodeSize++] = 0xC7;

            void (*Pcs)(const char*) = PutChars_Asm;
            uint64_t addr = (uint64_t)Pcs;

            MachineCodeBuffer[MachineCodeSize++] = 0x48;
            MachineCodeBuffer[MachineCodeSize++] = 0xB8;
            for (int j = 0; j < 8; j++)
                MachineCodeBuffer[MachineCodeSize++] = (addr >> (j * 8)) & 0xFF;

            MachineCodeBuffer[MachineCodeSize++] = 0xFF;
            MachineCodeBuffer[MachineCodeSize++] = 0xD0;
        }
        else
        {
            PutChars("\nInvalid instruction!\n");
        }
    }

    //Add RET at the end
    if (MachineCodeSize < sizeof(MachineCodeBuffer))
        MachineCodeBuffer[MachineCodeSize++] = 0xC3;
}

void RunCode()
{
    if (MachineCodeBuffer != 0)
    {
        uint64_t rax_value;

        //Call machine code and then move RAX to rax_value
        asm volatile (
            "call *%1\n\t"
            "mov %%rax, %0\n\t"
            : "=r"(rax_value)
            : "r"(MachineCodeBuffer)
            : "rax"
        );

        //Print RAX value
        PutChars("\nRAX = ");
        PutChars(HexToString(rax_value));
        PutChars("\n");
    }
}