#include <p18f4321.h>

#pragma config OSC = INTIO2
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOR = OFF

unsigned char SPI_data,count;
void Init(void);
void SPI_out(unsigned char SPI_data);
void chk_isr(void);
void My_HiPrio_Int(void);
void T0ISR(void);

void main() 
{
	Init();
	while(1)
	{
		if(SSPSTATbits.BF==1)
			PORTD=SSPBUF;
	}
}

void Init(void)
{ 
	
	TRISC=0X00; // Setup port C with output
	SSPCON1=0x24; // SPI Slave mode, clock = SCK pin, SS pin control enabled
	SSPSTAT=0x00; //
	
	TRISCbits.RC3 = 1; 
	TRISCbits.RC4 = 1;
	TRISB = 0x00;
	TRISE = 0x00;
	TRISD = 0x00;
	ADCON1 = 0xFF;
}
