#include <LPC214x.h>
#include "lcd.h"

#define col1 0
#define col2 1
#define col3 2

#define row1 (IO0PIN & (1<<3))
#define row2 (IO0PIN & (1<<4))
#define row3 (IO0PIN & (1<<5))
#define row4 (IO0PIN & (1<<6))



int main() {
    IO1DIR = (1 << en) | (1 << rs) | (1 << d4) | (1 << d5) | (1 << d6) | (1 << d7); //lcd set
		IO0DIR = (1 << col1) | (1 << col2) | (1 << col3); 
		//IO0DIR &= ~((1 << 3) | (1 << 4) | (1 << 5) | (1 << 6)); //&
    init();
   
    while (1) {
        disp(0x80, "PRESSED KEY:");
        IO0CLR = (1 << col1); IO0SET = (1 << col2); IO0SET = (1 << col3);
        if (!row1)  { while (!row1); write(0x8D, '1'); }
        if (!row2)  { while (!row2); write(0x8D, '4'); }
        if (!row3) { while (!row3); write(0x8D, '7'); }
        if (!row4) { while (!row4); write(0x8D, '*'); }

        IO0CLR = (1 << col2); IO0SET = (1 << col1); IO0SET = (1 << col3);
        if (!row1)  { while (!row1); write(0x8D, '2'); }
        if (!row2)  { while (!row2); write(0x8D, '5'); }
        if (!row3)  { while (!row3); write(0x8D, '8'); }
        if (!row4)  { while (!row4); write(0x8D, '0'); }

        IO0CLR = (1 << col3); IO0SET = (1 << col1); IO0SET = (1 << col2);
        if (!row1) { while (!row1); write(0x8D, '3'); }
        if (!row2) { while (!row2); write(0x8D, '6'); }
        if (!row3) { while (!row3); write(0x8D, '9'); }
        if (!row4) { while (!row4); write(0x8D, '#'); }
    }
}
