#include <p18f4525.h>
#include <delays.h>

//設定微處理器相關結構位元
#pragma config	OSC=HS, BOREN=OFF, PWRT=OFF, WDT=OFF, LVP=OFF
#define OSC_CLOCK 16

#define Data_Pin			PORTEbits.RE2
#define Data_Pin_Direction	TRISEbits.TRISE2

//unsigned char DHT22array[5]={0};
unsigned char T_byte1,T_byte2,RH_byte1,RH_byte2,CheckSum;
char Time_out=0;

void delay_ms(long A);

void start_signal(void);
void DHT22delay(char A);
unsigned char DHT22_value(void);
char DHT22_response(void);

void main()  
{
	char i,j,k,Time_out=0;
	TRISA=0x00;
	TRISB=0x00;
	TRISE=0x00;
	TRISD=0x00;
	PORTA=0x00;
	PORTE=0x00;			
	PORTD=0x80;
	ADCON1=0x0F;
	
	while(1)
	{
		start_signal();
		if(DHT22_response())
		{
			RH_byte1 = DHT22_value();                 // read RH byte1
			RH_byte2 = DHT22_value();                 // read RH byte2
			T_byte1 = DHT22_value();                  // read T byte1
			T_byte2 = DHT22_value();                  // read T byte2
			CheckSum = DHT22_value();                 // read checksum
		}
		Nop();
		Nop();
	}	
}
//*****************延遲指定毫秒程式**********************
void delay_ms(long A) 
{
//This function is only good for OSC_CLOCK higher then 4MHz
	int i,us2TCY;
	us2TCY=(10*OSC_CLOCK)>>2;
	for(i=0;i<A;i++)Delay100TCYx(us2TCY);
}

void DHT22delay(char A) 
{
	char i;
	for(i=0;i<A;i++)Nop();
}

unsigned char DHT22_value(void)
{
	unsigned char data=0;
	char j,k=0;
	if(Time_out)
		return 0xFF;
	for(j = 0; j < 8; j++)
	{
		k=0;
		while(!Data_Pin)
		{
			k++;
			if(k>100) {Time_out=1; return 0xFF;}
			Nop();
		}
		DHT22delay(5);//28
		if(!Data_Pin)
		{
			data<<=1;               // Clear bit (7 - i)
		}	
		else
		{
			data<<=1;
			data+=1;                 // Set bit (7 - i)
			while(Data_Pin)
			{
				k++;
				if(k>100) {Time_out=1; return 0xFF;}
				Nop();
			}
		}
	}
	return data;
}

char DHT22_response(void)
{
	DHT22delay(8);//40
	if(!Data_Pin)                     // Read and test if connection pin is low
	{
		DHT22delay(18);//80
		if(Data_Pin)                    // Read and test if connection pin is high
		{
			DHT22delay(10);//48
			return 1;
		}
	}
}

void start_signal(void)
{
	Data_Pin_Direction = 0;              // Configure connection pin as output
	Data_Pin = 0;                        // Connection pin output low
	delay_ms(25);
	Data_Pin = 1;                        // Connection pin output high
	DHT22delay(5);//28
	Data_Pin_Direction = 1;              // Configure connection pin as input
}
