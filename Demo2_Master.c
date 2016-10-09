#include <p18f4321.h>

#pragma config OSC = INTIO2
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOR = OFF

#define R0 PORTAbits.RA0
#define R1 PORTAbits.RA1
#define R2 PORTAbits.RA2

#define G0 PORTAbits.RA3
#define G1 PORTAbits.RA4
#define G2 PORTAbits.RA5

#define B0 PORTAbits.RE0
#define B1 PORTAbits.RE1
#define B2 PORTAbits.RE2

#define CS_0 PORTBbits.RB0
#define CS_1 PORTBbits.RB1
#define CS_2 PORTBbits.RB2
#define CS_3 PORTBbits.RB3
#define CS_4 PORTBbits.RB4
#define CS_5 PORTBbits.RB5
#define CS_6 PORTBbits.RB6
#define CS_7 PORTBbits.RB7

#define Clear_R PORTDbits.RD2
#define Clear_G PORTDbits.RD1
#define Clear_B PORTDbits.RD0

unsigned char row,column,level,SPI_data,count;
void Init(void);
void SPI_out(unsigned char Chip_Select, unsigned char SPI_data);
void chk_isr(void);
void My_HiPrio_Int(void);
void T0ISR(void);
void Test(void);
void cycle(void);
void Red(unsigned char Red_Select);
char common[2] = {0xFE,0xFD}; //,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
char SPI_init = 0xFF;
void LED(unsigned char face,unsigned char layer,unsigned char level,unsigned char color); 
void Clear_All(void);
void Delay(void);
void Wait_Half_Second(void);
void Wait_One_Second(void);
void Wait(int seconds);

void main() 
{
	Init();
	while(1)
	{
		Test();
	}
}

void Init(void)
{ 
	row = 0;
	column = 0;
	level = 0;
	count = 0;
	OSCCON=0x40; // Program oscillator to be at 1Mhz
	TRISC=0X00; // Setup port C with output
	SSPCON1=0x20; // SPI Master mode, clock = FOSC/4
	SSPSTAT=0x40; //
	T0CON=0x85; // Set Timer 0 in Timer mode
	INTCONbits.TMR0IE=1; // Enable Timer 0 interrupt
	INTCONbits.TMR0IF=0; // Clear Timer 0 Interrupt Flag
	INTCONbits.GIE=1; // Enable Global Interrupt
	TMR0H=0xF8; // Program Timer High byte
	TMR0L=0x5D; // Program Timer Low byte
	T0CONbits.TMR0ON=0; // Turn on Timer 0
	
	//CS_0 = 1;
	//CS_1 = 1;
	TRISA = 0x00;
	TRISB = 0x00;
	TRISE = 0x00;
	TRISD = 0x00;
	ADCON1 = 0xFF;
	SPI_out(0,0xFF);
	SPI_out(1,0xFF);
		//	LED(0,0,0,1);
		Clear_All();
}

void Delay(void)
{
	INTCONbits.TMR0IF = 1; // Clear Timer 0 Interrupt Flag
}	
	

/*
void Red(unsigned char Red_Select)
{
	switch (Red_Select)
	{
		case 0: CATHODEbit2=0;CATHODEbit1=0;CATHODEbit0=0; break;
		case 1: CATHODEbit2=0;CATHODEbit1=0;CATHODEbit0=1; break;
		case 2: CATHODEbit2=0;CATHODEbit1=1;CATHODEbit0=0; break;
		case 3: CATHODEbit2=0;CATHODEbit1=1;CATHODEbit0=1; break;
		case 4: CATHODEbit2=1;CATHODEbit1=0;CATHODEbit0=0; break;
		case 5: CATHODEbit2=1;CATHODEbit1=0;CATHODEbit0=1; break;
		case 6: CATHODEbit2=1;CATHODEbit1=1;CATHODEbit0=0; break;
		case 7: CATHODEbit2=1;CATHODEbit1=1;CATHODEbit0=1; break;
	}
}	
*/

void Clear_All(void)
{

	Wait(10);
	SPI_out(0,0xFF);
	SPI_out(1,0xFF);
	SPI_out(2,0xFF);
	SPI_out(3,0xFF);
	SPI_out(4,0xFF);
	SPI_out(5,0xFF);
	SPI_out(6,0xFF);
	SPI_out(7,0xFF);
	Clear_R = 1;
	Clear_G = 1;
	Clear_B = 1;
	Wait(10);
}	
	
void LED(unsigned char face,unsigned char layer,unsigned char level,unsigned char color)   			// Chooses what LED turns on by picking the corresponding x,y,z coordinate; and, what color to display
{
	char LED_Layer;
	SPI_out(0,0xFF);
	SPI_out(1,0xFF);
	SPI_out(2,0xFF);
	SPI_out(3,0xFF);
	SPI_out(4,0xFF);
	SPI_out(5,0xFF);
	SPI_out(6,0xFF);
	SPI_out(7,0xFF);
	

	/*switch(face)													
		{
			case 0: R2=0;R1=0;R0=0;break;
			case 1: R2=0;R1=0;R0=0;break;

		}*/
	switch(layer)
		{
			case 0: LED_Layer=0;break;
			case 1: LED_Layer=1;break;
			//case 2: LED_Layer = 2; break;
			//case 3: LED_Layer = 3; break;
		}
			
	SPI_out(level,common[face]);
					
	switch(color)
		{
		//  Color Assignment Code
		//	0 = off
		//	1 = red
		//	2 = green
		//	3 = yellow
		//	4 = blue
		//	5 = purple
		//	6 = cyan
		//	7 = white
			case 0: 
				switch(LED_Layer)
				{
					case 0: R2=0;R1=0;R0=0;break;
					case 1: R2=0;R1=0;R0=1;break; 
					case 2: R2=0;R1=1;R0=0;break; 
					case 3: R2=0;R1=1;R0=1;break; 
				}
				break;
			case 1: 
				switch(LED_Layer)
				{
					case 0: R2=0;R1=0;R0=0;Clear_R=0;Clear_G=1;Clear_B=1;break;
					case 1: R2=0;R1=0;R0=1;Clear_R=0;Clear_G=1;Clear_B=1;break; 
					case 2: R2=0;R1=1;R0=0;Clear_R=0;Clear_G=1;Clear_B=1;break; 
					case 3: R2=0;R1=1;R0=1;Clear_R=0;Clear_G=1;Clear_B=1;break;  
				}
				break;
			case 2: 
				switch(LED_Layer)
				{
					case 0: G2=0;G1=0;G0=0;Clear_R=1;Clear_G=0;Clear_B=1;break;
					case 1: G2=0;G1=0;G0=1;Clear_R=1;Clear_G=0;Clear_B=1;break; 
					case 2: G2=0;G1=1;G0=0;Clear_R=1;Clear_G=0;Clear_B=1;break; 
					case 3: G2=0;G1=1;G0=1;Clear_R=1;Clear_G=0;Clear_B=1;break;  
				}
				break;
			/*case 3: 
				switch(LED_Layer)
				{
					case 0: LEDR0 = 1; LEDG0 = 1; LEDB0 = 0; break;
					case 1: LEDR1 = 1; LEDG1 = 1; LEDB1 = 0; break;
					case 2: LEDR2 = 1; LEDG2 = 1; LEDB2 = 0; break;
					case 3: LEDR3 = 1; LEDG3 = 1; LEDB3 = 0; break;
				}
				break;
			case 4: 
				switch(LED_Layer)
				{
					case 0: LEDR0 = 0; LEDG0 = 0; LEDB0 = 1; break;
					case 1: LEDR1 = 0; LEDG1 = 0; LEDB1 = 1; break;
					case 2: LEDR2 = 0; LEDG2 = 0; LEDB2 = 1; break;
					case 3: LEDR3 = 0; LEDG3 = 0; LEDB3 = 1; break;
				}
				break;
			case 5: 
				switch(LED_Layer)
				{
					case 0: LEDR0 = 1; LEDG0 = 0; LEDB0 = 1; break;
					case 1: LEDR1 = 1; LEDG1 = 0; LEDB1 = 1; break;
					case 2: LEDR2 = 1; LEDG2 = 0; LEDB2 = 1; break;
					case 3: LEDR3 = 1; LEDG3 = 0; LEDB3 = 1; break;
				}
				break;
			case 6: 
				switch(LED_Layer)
				{
					case 0: LEDR0 = 0; LEDG0 = 1; LEDB0 = 1; break;
					case 1: LEDR1 = 0; LEDG1 = 1; LEDB1 = 1; break;
					case 2: LEDR2 = 0; LEDG2 = 1; LEDB2 = 1; break;
					case 3: LEDR3 = 0; LEDG3 = 1; LEDB3 = 1; break;
				}
				break;
			case 7: 
				switch(LED_Layer)
				{
					case 0: LEDR0 = 1; LEDG0 = 1; LEDB0 = 1; break;
					case 1: LEDR1 = 1; LEDG1 = 1; LEDB1 = 1; break;
					case 2: LEDR2 = 1; LEDG2 = 1; LEDB2 = 1; break;
					case 3: LEDR3 = 1; LEDG3 = 1; LEDB3 = 1; break;
				}
				break;
				*/
		}
}		

	
void Test(void)
{
	
	LED(0,0,0,1);
	Clear_All();
	LED(0,0,1,1);
	Clear_All();
	LED(0,1,0,1);
	Clear_All();
	LED(0,1,1,1);
	Clear_All();
	LED(1,0,0,1);
	Clear_All();
	LED(1,0,1,1);
	Clear_All();
	LED(1,1,0,1);
	Clear_All();
	LED(1,1,1,1);
	Clear_All();
	
	
	LED(0,0,0,2);
	Clear_All();
	LED(0,0,1,2);
	Clear_All();
	LED(0,1,0,2);
	Clear_All();
	LED(0,1,1,2);
	Clear_All();
	LED(1,0,0,2);
	Clear_All();
	LED(1,0,1,2);
	Clear_All();
	LED(1,1,0,2);
	Clear_All();
	LED(1,1,1,2);
	Clear_All();
	
}
	


#pragma code My_HiPriority_Int=0x08
void My_HiPrio_Int(void)
{
	_asm
	GOTO chk_isr // When Priority occurs, go to chk_isr routine
	_endasm
}
	#pragma code
	// Check ISR routine
	#pragma interrupt chk_isr

void chk_isr(void)
{
	T0ISR(); // Go to Timer 0 ISR
}

void SPI_out(unsigned char Chip_Select, unsigned char SPI_data)
{
	unsigned char Select;
	switch (Chip_Select)
	{
		case 0: CS_0 = 0; // set the chip select of the D/A chip to be low
 				SSPBUF = SPI_data; // output the first byte to the SPI bus
				while (SSPSTATbits.BF == 0); // wait for status done
				CS_0 = 1; // raise chip select high
				break;
		case 1: CS_1 = 0; // set the chip select of the D/A chip to be low
 				SSPBUF = SPI_data; // output the first byte to the SPI bus
				while (SSPSTATbits.BF == 0); // wait for status done
				CS_1 = 1; // raise chip select high
				break;	
		case 2: CS_2 = 0; // set the chip select of the D/A chip to be low
 				SSPBUF = SPI_data; // output the first byte to the SPI bus
				while (SSPSTATbits.BF == 0); // wait for status done
				CS_2 = 1; // raise chip select high
				break;
		case 3: CS_3 = 0; // set the chip select of the D/A chip to be low
 				SSPBUF = SPI_data; // output the first byte to the SPI bus
				while (SSPSTATbits.BF == 0); // wait for status done
				CS_3 = 1; // raise chip select high
				break;
		case 4: CS_4 = 0; // set the chip select of the D/A chip to be low
 				SSPBUF = SPI_data; // output the first byte to the SPI bus
				while (SSPSTATbits.BF == 0); // wait for status done
				CS_4 = 1; // raise chip select high
				break;
		case 5: CS_5 = 0; // set the chip select of the D/A chip to be low
 				SSPBUF = SPI_data; // output the first byte to the SPI bus
				while (SSPSTATbits.BF == 0); // wait for status done
				CS_5 = 1; // raise chip select high
				break;
		case 6: CS_6 = 0; // set the chip select of the D/A chip to be low
 				SSPBUF = SPI_data; // output the first byte to the SPI bus
				while (SSPSTATbits.BF == 0); // wait for status done
				CS_6 = 1; // raise chip select high
				break;
		case 7: CS_7 = 0; // set the chip select of the D/A chip to be low
 				SSPBUF = SPI_data; // output the first byte to the SPI bus
				while (SSPSTATbits.BF == 0); // wait for status done
				CS_7 = 1; // raise chip select high
				break;																															
	}			
}

void T0ISR(void)
{	
	INTCONbits.TMR0IF = 0; // Clear Timer 0 Interrupt Flag
	TMR0H=0x80; // Program Timer High byte
	TMR0L=0x00; // Program Timer Low byte
}

void Wait(int seconds)
{
	int x;
	for (x=seconds;x>0;x--)
	{
		Wait_One_Second();
	}
}		

void Wait_Half_Second(void)
{
	int j;
		for ( j=0;j<200;j++ )  //6000 for 1 second
		{
		}
}

void Wait_One_Second(void)
{
	Wait_Half_Second();
	
	Wait_Half_Second();
	
}