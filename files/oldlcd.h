
void init(void);
void command(unsigned char);
void write(unsigned char,unsigned char);
void disp(unsigned char,const unsigned char*);
void assign(unsigned char);
void delay(unsigned int);

#define rs 16
#define en 17
#define d4 18
#define d5 19
#define d6 20
#define d7 21

unsigned char z;

void init(void)
{
	command(0x33);
	command(0x32);
	command(0x28);
	command(0x0C);
	command(0x06);
	command(0x01);
	delay(2000);
}

void command(unsigned char x)
{
	z = (x & 0xf0);
	assign(z);
	IO1CLR=(1 << rs);
	IO1SET = (1<<en);
	delay(1);
	IO1CLR = (1<<en);
	delay(1);
	z = ((x<<4)&0xF0);
	assign(z);
	IO1SET = (1<<en);
	delay(1);
	IO1CLR = (1<<en);
	delay(1);
}

void write(unsigned char x,unsigned char y)
{
	command(x);
	z = (y & 0xF0);
	assign(z);
	IO1SET = (1<<rs);
	IO1SET = (1<<en);
	delay(1);
	IO1CLR = (1<<en);
	//IO1CLR = (1<<rs);
	delay(1);
	z = ((y<<4)&0xf0);
	assign(z);
	IO1SET = (1<<rs);
	IO1SET = (1<<en);
	delay(1);
	IO1CLR = (1<<en);
	//IO1CLR = (1<<rs);
	delay(1);
}

void disp(unsigned char x, const unsigned char *y)
{
	unsigned char q;
	for(q=0; y[q]!='\0' ; q++)
	{
		write(x+q,y[q]);
	}
}

void assign(unsigned char x)
{
	if(x & 0x10) IO1SET = (1<<d4); else IO1CLR = (1<<d4);
	if(x & 0x20) IO1SET = (1<<d5); else IO1CLR = (1<<d5);
	if(x & 0x40) IO1SET = (1<<d6); else IO1CLR = (1<<d6);
	if(x & 0x80) IO1SET = (1<<d7); else IO1CLR = (1<<d7);
}

void delay(unsigned int x) {
    volatile unsigned int i, j;
    for (i = 0; i < x; i++) {
        for (j = 0; j < 3000; j++);
    }
}