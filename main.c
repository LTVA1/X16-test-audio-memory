#include <stdint.h>
#include <stdio.h>
#include <conio.h>
#include <6502.h>

#include <cx16.h>

#include "PSG.h"

#define VERA_PSG_BASE_ADDR 0xF9C0

#define WRITE_YM2612(regi, value) YM2151.reg = regi; YM2151.data = value; asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

#define START_VERA_PSG_WRITE() VERA.address_hi = 1 | VERA_INC_1; VERA.address = VERA_PSG_BASE_ADDR;

#define WRITE_VERA_PSG(value) VERA.data0 = value;

void border_color(uint8_t color)
{
    vpoke(color, 0x01fa0c); //avoid using this at all costs, it's too slow
} //instead use macros above and autoinc feature, it speeds things up
//don't forget to semi-unroll your loops!

uint16_t frequency;

uint8_t psg_write_iterator;

void update_psg_registers()
{
    START_VERA_PSG_WRITE();

    for(psg_write_iterator = 0; psg_write_iterator < PSG_NUM_CHANNELS / 4; psg_write_iterator += 4)
    {
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 1]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 2]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 3]);

        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 4]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 5]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 6]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 7]);

        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 8]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 9]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 10]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 11]);

        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 12]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 13]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 14]);
        WRITE_VERA_PSG(VERA_PSG.registers[psg_write_iterator + 15]);
    }
}

#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

uint16_t old_vector;

void VBI_handler()
{
    asm("pha        ;store register A in stack");
    asm("txa");
    asm("pha        ;store register X in stack");
    asm("tya");
    asm("pha        ;store register Y in stack");

    VERA.irq_flags = 0;

    frequency += max(frequency, 1);

    VERA_PSG.registers[0] = frequency;
    VERA_PSG.registers[1] = frequency >> 8;
    VERA_PSG.registers[2] = 0xFF;
    VERA_PSG.registers[3] = 0x3F;

    START_VERA_PSG_WRITE();

    WRITE_VERA_PSG(VERA_PSG.registers[0]);
    WRITE_VERA_PSG(VERA_PSG.registers[1]);
    WRITE_VERA_PSG(VERA_PSG.registers[2]);
    WRITE_VERA_PSG(VERA_PSG.registers[3]);

    /*WRITE_YM2612(8, 0x78);

    WRITE_YM2612(0x20, 0b11000000);

    WRITE_YM2612(0x28, 0x3f);

    WRITE_YM2612(0x60, 0);
    WRITE_YM2612(0x68, 0);
    WRITE_YM2612(0x70, 0);
    WRITE_YM2612(0x78, 0);

    WRITE_YM2612(0xa0, 0);
    WRITE_YM2612(0xa8, 0);
    WRITE_YM2612(0xb0, 0);
    WRITE_YM2612(0x88, 0);

    WRITE_YM2612(0xe0, 0xf0);
    WRITE_YM2612(0xe8, 0xf0);
    WRITE_YM2612(0xf0, 0xf0);
    WRITE_YM2612(0xf8, 0xf0);*/

    asm("pla");
    asm("tay        ;restore register Y from stack");
    asm("pla");
    asm("tax        ;restore register X from stack");
    asm("pla        ;restore register A from stack");

    asm("jmp (%v)", old_vector);

    asm("rti        ;Return From Interrupt");
}

//

uint8_t i;

void update_YM2151_and_psg()
{
    border_color(52);

    for (i = 0; i < 0x80; ++i)
    {
        WRITE_YM2612(i, 0);
        WRITE_YM2612(i + 0x80, 0);
    }

    border_color(15);

    update_psg_registers();

    border_color(0);
}

#pragma code-name (push,"BANKRAM03")

void update_and_psg() //this function isn't used in test program and goes to B3.BIN
{
    border_color(52);

    for (i = 0; i < 0x80; ++i)
    {
        WRITE_YM2612(i, 0);
        WRITE_YM2612(i + 0x80, 0);
    }

    border_color(15);

    update_psg_registers();

    border_color(0);

    border_color(52);

    for (i = 0; i < 0x80; ++i)
    {
        WRITE_YM2612(i, 0);
        WRITE_YM2612(i + 0x80, 0);
    }

    border_color(15);

    update_psg_registers();

    border_color(0);
}

#pragma code-name (pop)

void play()
{
    while (1)
    {
        update_YM2151_and_psg();

        waitvsync();

        if (kbhit()) 
        {
            cgetc();
            return;
        }
    }
}

uint16_t* p;

int main()
{
    for(psg_write_iterator = 0; psg_write_iterator < PSG_NUM_CHANNELS * PSG_CHN_REGISTERS; psg_write_iterator++)
    {
        VERA_PSG.registers[psg_write_iterator] = 0;
    }

    update_YM2151_and_psg();

    clrscr();
    printf("\ntest\n\n");
    // run until key press
    printf("\npress any key to stop\n");

    p = (uint16_t*)0x0314;
    old_vector = *p;
    *p = (uint16_t*)VBI_handler;
    VERA.irq_enable = 1;

    play();

    for(psg_write_iterator = 0; psg_write_iterator < PSG_NUM_CHANNELS * PSG_CHN_REGISTERS; psg_write_iterator++)
    {
        VERA_PSG.registers[psg_write_iterator] = 0;
    }

    update_YM2151_and_psg();

    VERA.irq_enable = 0;

    while(1) {}

    return 0;
}
