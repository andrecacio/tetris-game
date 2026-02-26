#include "GLCD.h" 
#include "AsciiLib.h"

static uint8_t LCD_Code;

#define  ILI9320    0  
#define  ILI9325    1  
#define  ILI9328    2  
#define  ILI9331    3  
#define  SSD1298    4  
#define  SSD1289    5  
#define  ST7781     6  
#define  LGDP4531   7  
#define  SPFD5408B  8  
#define  R61505U    9  
#define  HX8346A    10 
#define  HX8347D    11 
#define  HX8347A    12 
#define  LGDP4535   13 
#define  SSD2119    14 

static void LCD_Configuration(void)
{
	LPC_GPIO0->FIODIR   |= 0x03f80000;
	LPC_GPIO0->FIOSET    = 0x03f80000;
}

static __attribute__((always_inline)) void LCD_Send (uint16_t byte) 
{
	LPC_GPIO2->FIODIR |= 0xFF;          
	LCD_DIR(1)		    				     
	LCD_EN(0)	                        
	LPC_GPIO2->FIOPIN =  byte;          
	LCD_LE(1)                          
	LCD_LE(0)							
	LPC_GPIO2->FIOPIN =  byte >> 8;     
}

static void wait_delay(int count)
{
	while(count--);
}

static __attribute__((always_inline)) uint16_t LCD_Read (void) 
{
	uint16_t value;
	LPC_GPIO2->FIODIR &= ~(0xFF);              
	LCD_DIR(0);		    				            
	LCD_EN(0);	                               
	wait_delay(30);							    						 
	value = LPC_GPIO2->FIOPIN0;                
	LCD_EN(1);	                               
	wait_delay(30);							    						 
	value = (value << 8) | LPC_GPIO2->FIOPIN0; 
	LCD_DIR(1);
	return  value;
}

static __attribute__((always_inline)) void LCD_WriteIndex(uint16_t index)
{
	LCD_CS(0);
	LCD_RS(0);
	LCD_RD(1);
	LCD_Send( index ); 
	wait_delay(22);	
	LCD_WR(0);  
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

static __attribute__((always_inline)) void LCD_WriteData(uint16_t data)
{				
	LCD_CS(0);
	LCD_RS(1);   
	LCD_Send( data );
	LCD_WR(0);     
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

static __attribute__((always_inline)) uint16_t LCD_ReadData(void)
{ 
	uint16_t value;
	LCD_CS(0);
	LCD_RS(1);
	LCD_WR(1);
	LCD_RD(0);
	value = LCD_Read();
	LCD_RD(1);
	LCD_CS(1);
	return value;
}

static __attribute__((always_inline)) void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{ 
	LCD_WriteIndex(LCD_Reg);         
	LCD_WriteData(LCD_RegValue);  
}

static __attribute__((always_inline)) uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	uint16_t LCD_RAM;
	LCD_WriteIndex(LCD_Reg);
	LCD_RAM = LCD_ReadData();      	
	return LCD_RAM;
}

static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
	uint16_t temp = Xpos;
			 Xpos = Ypos;
			 Ypos = ( MAX_X - 1 ) - temp;  
	#endif

  switch( LCD_Code )
  {
     default:		
          LCD_WriteReg(0x0020, Xpos );     
          LCD_WriteReg(0x0021, Ypos );     
	      break; 
     case SSD1298: 
     case SSD1289:   
	      LCD_WriteReg(0x004e, Xpos );      
          LCD_WriteReg(0x004f, Ypos );          
	      break;  
     case HX8346A: 
     case HX8347A: 
     case HX8347D: 
	      LCD_WriteReg(0x02, Xpos>>8 );                                     
	      LCD_WriteReg(0x03, Xpos );  
	      LCD_WriteReg(0x06, Ypos>>8 );                            
	      LCD_WriteReg(0x07, Ypos );    
	      break;      
     case SSD2119:	
	      break; 
  }
}

static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 

void LCD_Initialization(void)
{
	uint16_t DeviceCode;
	LCD_Configuration();
	delay_ms(100);
	DeviceCode = LCD_ReadReg(0x0000);		
	
	if( DeviceCode == 0x9325 || DeviceCode == 0x9328 )	
	{
		LCD_Code = ILI9325;
		LCD_WriteReg(0x00e7,0x0010);      
		LCD_WriteReg(0x0000,0x0001);  
		LCD_WriteReg(0x0001,0x0100);     
		LCD_WriteReg(0x0002,0x0700); 
		LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 
		LCD_WriteReg(0x0004,0x0000);                                   
		LCD_WriteReg(0x0008,0x0207);           
		LCD_WriteReg(0x0009,0x0000);         
		LCD_WriteReg(0x000a,0x0000);       
		LCD_WriteReg(0x000c,0x0001);    
		LCD_WriteReg(0x000d,0x0000);             
		LCD_WriteReg(0x000f,0x0000);
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);                                                              
		LCD_WriteReg(0x0013,0x0000);                 
		delay_ms(50); 
		LCD_WriteReg(0x0010,0x1590);   
		LCD_WriteReg(0x0011,0x0227);
		delay_ms(50); 
		LCD_WriteReg(0x0012,0x009c);                  
		delay_ms(50); 
		LCD_WriteReg(0x0013,0x1900);   
		LCD_WriteReg(0x0029,0x0023);
		LCD_WriteReg(0x002b,0x000e);
		delay_ms(50); 
		LCD_WriteReg(0x0020,0x0000);                                                    
		LCD_WriteReg(0x0021,0x0000);           
		delay_ms(50); 
		LCD_WriteReg(0x0030,0x0007); 
		LCD_WriteReg(0x0031,0x0707);   
		LCD_WriteReg(0x0032,0x0006);
		LCD_WriteReg(0x0035,0x0704);
		LCD_WriteReg(0x0036,0x1f04); 
		LCD_WriteReg(0x0037,0x0004);
		LCD_WriteReg(0x0038,0x0000);        
		LCD_WriteReg(0x0039,0x0706);     
		LCD_WriteReg(0x003c,0x0701);
		LCD_WriteReg(0x003d,0x000f);
		delay_ms(50); 
		LCD_WriteReg(0x0050,0x0000);        
		LCD_WriteReg(0x0051,0x00ef);   
		LCD_WriteReg(0x0052,0x0000);     
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0xa700);        
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006a,0x0000);
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);
		  
		LCD_WriteReg(0x0090,0x0010);     
		LCD_WriteReg(0x0092,0x0000);  
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0110);
		LCD_WriteReg(0x0097,0x0000);        
		LCD_WriteReg(0x0098,0x0000);  
		LCD_WriteReg(0x0007,0x0133);
		LCD_WriteReg(0x0020,0x0000);                                                       
		LCD_WriteReg(0x0021,0x0000); 
	}
    delay_ms(50); 
}


void LCD_Clear(uint16_t Color)
{
	uint32_t index;
	LCD_SetCursor(0,0); 
	LCD_WriteIndex(0x0022);
	for( index = 0; index < MAX_X * MAX_Y; index++ ) {
		LCD_WriteData(Color);
	}
}

static uint16_t LCD_BGR2RGB(uint16_t color)
{
	uint16_t  r, g, b, rgb;
	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
	rgb =  (b<<11) + (g<<5) + (r<<0);
	return( rgb );
}

uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
	uint16_t dummy;
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteIndex(0x0022);  
	switch( LCD_Code )
	{
        default:
             dummy = LCD_ReadData(); 
             dummy = LCD_ReadData(); 	
             return  LCD_BGR2RGB( dummy );
	}
}

void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y ) return;
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022,point);
}

void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
    short dx,dy,temp;
    if( x0 > x1 ) { temp = x1; x1 = x0; x0 = temp; }
    if( y0 > y1 ) { temp = y1; y1 = y0; y0 = temp; }
    dx = x1-x0; dy = y1-y0;
    
    if( dx == 0 ) {
        do { LCD_SetPoint(x0, y0, color); y0++; } while( y1 >= y0 );
        return; 
    }
    if( dy == 0 ) {
        do { LCD_SetPoint(x0, y0, color); x0++; } while( x1 >= x0 );
        return;
    }
    
    if( dx > dy ) {
        temp = 2 * dy - dx;
        while( x0 != x1 ) {
            LCD_SetPoint(x0,y0,color);
            x0++;
            if( temp > 0 ) { y0++; temp += 2 * dy - 2 * dx; }
            else { temp += 2 * dy; }
        }
        LCD_SetPoint(x0,y0,color);
    } else {
        temp = 2 * dx - dy;
        while( y0 != y1 ) {
            LCD_SetPoint(x0,y0,color);
            y0++;
            if( temp > 0 ) { x0++; temp += 2 * dy - 2 * dx; }
            else { temp += 2 * dy; }
        }
        LCD_SetPoint(x0,y0,color);
    }
} 

void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);
    for( i=0; i<16; i++ ) {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ ) {
            if( ((tmp_char >> (7 - j)) & 0x01) == 0x01 )
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );
            else
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );
        }
    }
}

void GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor)
{
    uint8_t TempChar;
    do {
        TempChar = *str++;  
        PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < MAX_X - 8 ) Xpos += 8;
        else if ( Ypos < MAX_Y - 16 ) { Xpos = 0; Ypos += 16; }   
        else { Xpos = 0; Ypos = 0; }    
    } while ( *str != 0 );
}