/**************************************************************************/
/*
    Driver for st7735 128x160 pixel TFT LCD displays.
    
    This driver uses a bit-banged SPI interface and a 16-bit RGB565
    colour palette.
*/
/* Private includes ----------------------------------------------------------*/
#include "st7735.h"
#include "spi.h"
#include "Font.h"

/* Private function prototypes  --------------------------------------------*/
static void lcd7735_sendbyte(uint8_t data);
static void lcd7735_send2byte(uint8_t msb, uint8_t lsb);
static void WriteCmd(uint8_t cmd);
static void WriteData(uint8_t data);
static void ST7735_WaitLastData(void);
static void Init_ILI9163V(void);
static void lcdSetOrientation(uint8_t orientation);
static void lcdFillRGB(uint16_t color);
static uint16_t lcdGetWidth (void);
static uint16_t lcdGetHeight (void);
static uint16_t FindColor (uint8_t color);
static void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color);

//************************************************************************//
#define BUFF_SIZE (ST7735_PANEL_WIDTH*ST7735_PANEL_HEIGHT)

//************************************************************************//
const uint8_t * GlobalFont = Arial_22x23;  
uint32_t Paint_Color = black;
uint32_t Back_Color = white;
uint8_t Lcd_buffer[BUFF_SIZE];

//************************************************************************//
static void lcd7735_sendbyte(uint8_t data) //передача в режиме 8 бит
{
  HAL_SPI_Transmit(&hspi1, &data,1,0x1);
	//while((SPI1->SR & SPI_SR_TXE) == RESET){}
	//SPI1->DR = data;
}

//************************************************************************//
static void lcd7735_send2byte(uint8_t msb, uint8_t lsb) //передача в режиме 16 бит
{
	uint8_t masData[]={lsb,msb};
	HAL_SPI_Transmit(&hspi1, masData, 1, 0x1);
	//lcd7735_sendbyte(masData[0]);
	//lcd7735_sendbyte(masData[1]);
}

//************************************************************************//
static void WriteCmd(uint8_t cmd) 
{  
	LCD_DC(0); //сброс линии DC
	lcd7735_sendbyte(cmd);
//	ST7735_WaitLastData();
}

//************************************************************************//
static void WriteData(uint8_t data)
{  
	LCD_DC(1);//установка вывод DC в единицу, т.к. передаем данные
	lcd7735_sendbyte (data);
//	ST7735_WaitLastData();
}

//************************************************************************//
static void ST7735_WaitLastData(void)
{
  while((SPI1->SR & SPI_SR_TXE) == RESET);
  while((SPI1->SR & SPI_SR_BSY) != RESET);
}

//************************************************************************//
static void Init_ILI9163V(void)
{
  //SPI1->CR1 |= SPI_CR1_SPE; //включение SPI модуля
    
	WriteCmd(SLPOUT);  //отключение спящего режима
	HAL_Delay(5);

	
	WriteCmd(SGAMMASET);  //выбор гамма-кривой для текущего отображения
	WriteData(0x04);

	WriteCmd(FRMCTR1);  //Set Frame Rate    
	WriteData(0x0b);
	WriteData(0x14);

	WriteCmd(PWCTR1);  //Power_Control 1     //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	WriteData(0x08);
	WriteData(0x05);

	WriteCmd(PWCTR2);  //Power_Control 2     //Set BT[2:0] for AVDD & VCL & VGH & VGL
	WriteData(0x02);

	WriteCmd(VMCTR1);  //Power_Control 3    //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	WriteData(0x44);
	WriteData(0x48);

	WriteCmd(VMOFCTR);  // Set VMF        
	WriteData(0xc2);

	WriteCmd(COLMODE);  // set color mode   Interface Pixel Format 
	WriteData(0x05); // 16-bit color

	WriteCmd(CASET);  //Set Column Address
	WriteData(0x00); // XS [15..8]
	WriteData(0x00); //XS [7..0]
	WriteData(0x00); //XE [15..8]
	WriteData(0x7F); //XE [7..0]

	WriteCmd(RASET);  //Set Page Address
	WriteData(0x00); // YS [15..8]
	WriteData(0x00);  //YS [7..0]
	WriteData(0x00); //YE [15..8]
	WriteData(0x9F); //YE [7..0]
	
	WriteCmd(MADCTL);	 // Memory Data Access Control   
	WriteData(0xC8);  //!

	WriteCmd(SOURCE);  // Source Driver Direction
	WriteData(0x00);

	WriteCmd(GAMMABIT); //Enable Gamma bit
	WriteData(0x01);
	
	WriteCmd(GMCTRP1); //Positive Gamma Correction Setting
	WriteData(0x3F);//p1
	WriteData(0x25);//p2
	WriteData(0x21);//p3
	WriteData(0x24);//p4
	WriteData(0x1d);//p5
	WriteData(0x0d);//p6
	WriteData(0x4c);//p7
	WriteData(0xB8);//p8
	WriteData(0x38);//p9
	WriteData(0x17);//p10
	WriteData(0x0f);//p11
	WriteData(0x08);//p12
	WriteData(0x04);//p13
	WriteData(0x02);//p14
	WriteData(0x00);//p15
	
	WriteCmd(GMCTRN1); //Negative Gamma Correction Setting
	WriteData(0x00);//p1
	WriteData(0x1a);//p2
	WriteData(0x1e);//p3
	WriteData(0x0B);//p4
	WriteData(0x12);//p5
	WriteData(0x12);//p6
	WriteData(0x33);//p7
	WriteData(0x47);//p8
	WriteData(0x47);//p9
	WriteData(0x08);//p10
	WriteData(0x20);//p11
	WriteData(0x27);//p12
	WriteData(0x3c);//p13
	WriteData(0x3d);//p14
	WriteData(0x3F);//p15

	WriteCmd(DISPON); // Display On
}

//************************************************************************//
static void lcdSetOrientation(uint8_t orientation)
{
  switch (orientation)
	{
		case 0:
			WriteCmd(MADCTL);  // Memory Data Access Control
			WriteData(0x60);         //горизонтальная ориентация 
	    break;    
		
		case 1:
			WriteCmd(MADCTL);  // Memory Data Access Control
			WriteData(0xC0);          // вертикальная ориентация, зеркальная
			break;
		
		case 2:
			WriteCmd(MADCTL);  // Memory Data Access Control
			WriteData(0x00);           // вертикальная ориентация
			break;
				
		default:
			break;
	}
}

//************************************************************************//
static void lcdFillRGB(uint16_t color)
{
  uint16_t x;
	uint16_t color2;
	
	color2=color >> 8;
  st7735SetAddrWindow(0, 0, lcdGetWidth() - 1, lcdGetHeight() - 1);
  WriteCmd(RAMWR);  // write to RAM
  for (x=0; x < ST7735_PANEL_WIDTH*ST7735_PANEL_HEIGHT; x++) 
	{
		WriteData(color2);    
		WriteData(color);    
  }
  WriteCmd(NOP);
}

//************************************************************************//
void lcdInit(void)
{
	LCD_CS(0);
	LCD_RST(0);
	HAL_Delay(5);	
  LCD_RST(1);
  HAL_Delay(5);

	Init_ILI9163V();	// Run LCD init sequence
  lcdSetOrientation(0); //установка ориентации дисплея
  lcdFillRGB(Back_Color); //цвет фона дисплея
}

//************************************************************************//
static uint16_t lcdGetWidth(void)
{
  return ST7735_PANEL_WIDTH;
}

//************************************************************************//
static uint16_t lcdGetHeight(void)
{
  return ST7735_PANEL_HEIGHT;
}

//************************************************************************//
static uint16_t FindColor (uint8_t color)
{
	if(color==black){return BLACK;}
	if(color==white){return WHITE;}	
	if(color==green){return GREEN;}	
	if(color==red){return RED;}	
	if(color==blue){return BLUE;}
	if(color==yellow){return YELLOW;}
	if(color==grey){return GRAY;}
	return 0;
}

//************************************************************************//
void ClearLcdMemory(void)
{
	int tmp=0;
	for(tmp=0; (tmp < BUFF_SIZE); tmp++)
	{
		Lcd_buffer[tmp]=0xFF;
	}
}

//************************************************************************//
void st7735SetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  WriteCmd(CASET);   // column addr set
  WriteData(0x00);
  WriteData(x0);          // XSTART
  WriteData(0x00);
  WriteData(x1);          // XEND

  WriteCmd(RASET);   // row addr set
  WriteData(0x00);
  WriteData(y0);          // YSTART
  WriteData(0x00);
  WriteData(y1);          // YEND
}

//************************************************************************//
void LCD_Refresh(void)
{  	
	int x, y;
	
  st7735SetAddrWindow(0, 0, lcdGetWidth() - 1, lcdGetHeight() - 1);
  WriteCmd(RAMWR);  // write to RAM
	
  for (x=0; x < (ST7735_PANEL_WIDTH*ST7735_PANEL_HEIGHT); x++) 
	{
		y = FindColor(Lcd_buffer[x]);
		WriteData(y >> 8);    
		WriteData(y);     
  }
  WriteCmd(NOP);
}

//************************************************************************//
void LCD_SetFont(const uint8_t * font, uint32_t color)
{
 	GlobalFont=font;
	Paint_Color = color;
}

//************************************************************************//
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	x = y*ST7735_PANEL_WIDTH+x;
	Lcd_buffer[x] = (uint8_t)color;	
	
}

//************************************************************************//
uint32_t LCD_FastShowChar(uint32_t x, uint32_t y, uint8_t num)
{        
  uint8_t tmp;
	uint16_t tmp2,tmp3,tmp4;
  uint32_t dy,i;
	uint32_t tmpMaxWidth = 0, maxWidth = 0;
	uint32_t symbolHeight, symbolLeghth, symbolByteWidth;
	 
   if ((x>ST7735_PANEL_WIDTH) || (y>ST7735_PANEL_HEIGHT))
	 {	return 0;	}

	if (num == ' ') // special case - " " symbol doesn't have any width
	{ return 10; }		  	
	if (num < ' ') 
	{	return 0;	}            					
	else 
	{	num=num-' ';	}

	symbolByteWidth = GlobalFont[0];	 	
	symbolHeight =  GlobalFont[2];
	symbolLeghth = (GlobalFont[0])*(GlobalFont[2]);
	//symbolLeghth =  GlobalFont[3];

 	for(dy=0; dy<symbolHeight; dy++) 
	{
		tmp4=y+dy;
		for(i=0; i<symbolByteWidth; i++)
		{
			tmp2=i*8;
			tmp3=x+i*8;
			tmp = GlobalFont[num*symbolLeghth + dy*symbolByteWidth+i]; 
			
			if (tmp&0x80) 
			{
				lcdDrawPixel(tmp3+0, tmp4, Paint_Color); 
				tmpMaxWidth = tmp2+1; 
			}
			if (tmp&0x40) 
			{
				lcdDrawPixel(tmp3+1, tmp4, Paint_Color); 
				tmpMaxWidth = tmp2+2; 
			}
			if (tmp&0x20) 
			{
				lcdDrawPixel(tmp3+2, tmp4, Paint_Color); 
				tmpMaxWidth = tmp2+3; 
			}
			if (tmp&0x10) 
			{
				lcdDrawPixel(tmp3+3, tmp4, Paint_Color); 
				tmpMaxWidth = tmp2+4; 
			}
			if (tmp&0x08) 
			{	
				lcdDrawPixel(tmp3+4, tmp4, Paint_Color); 
				tmpMaxWidth = tmp2+5; 
			}
			if (tmp&0x04) 
			{
				lcdDrawPixel(tmp3+5, tmp4, Paint_Color);
				tmpMaxWidth = tmp2+6; 
			}
			if (tmp&0x02) 
			{
				lcdDrawPixel(tmp3+6, tmp4, Paint_Color); 
				tmpMaxWidth = tmp2+7; 
			}
			if (tmp&0x01) 
			{
				lcdDrawPixel(tmp3+7, tmp4, Paint_Color); 
				tmpMaxWidth = tmp2+8; 
			}

			if (tmpMaxWidth > maxWidth) 
			{ maxWidth = tmpMaxWidth; }
		}
 	}
	return (maxWidth+maxWidth/8+1);
}

//************************************************************************//
uint32_t LCD_GetCharWidth(uint32_t y, uint8_t num)
{        
  uint8_t tmp;
	uint16_t tmp2;
  uint32_t dy,i;
	uint32_t tmpMaxWidth = 0, maxWidth = 0;
	uint32_t symbolHeight, symbolLeghth, symbolByteWidth;
	 
	if(y > ST7735_PANEL_HEIGHT)
	{	return 0;	}

	if (num == ' ') 	// special case - " " symbol doesn't have any width
	{ return 10; }		  
	if (num < ' ') 
	{	return 0;	}	            					
	else 
	{	num=num-' ';	}

	symbolByteWidth = GlobalFont[0];	 	
	symbolHeight =  GlobalFont[2];
	symbolLeghth = (GlobalFont[0])*(GlobalFont[2]);
	//symbolLeghth =  GlobalFont[3];

 	for(dy=0;dy<symbolHeight;dy++) 
	{
		for(i=0;i<symbolByteWidth;i++)
		{
			tmp2=i*8;
			tmp = GlobalFont[num*symbolLeghth + dy*symbolByteWidth+i]; 
			
			if (tmp&0x80) 
			{	tmpMaxWidth = tmp2+1; }
			if (tmp&0x40) 
			{	tmpMaxWidth = tmp2+2; }
			if (tmp&0x20) 
			{	tmpMaxWidth = tmp2+3; }
			if (tmp&0x10) 
			{	tmpMaxWidth = tmp2+4; }
			if (tmp&0x08) 
			{	tmpMaxWidth = tmp2+5; }
			if (tmp&0x04) 
			{	tmpMaxWidth = tmp2+6; }
			if (tmp&0x02) 
			{	tmpMaxWidth = tmp2+7; }
			if (tmp&0x01) 
			{	tmpMaxWidth = tmp2+8; }

			if (tmpMaxWidth > maxWidth) 
			{ maxWidth = tmpMaxWidth; }
		}
 	}
	return (maxWidth+maxWidth/8+1);
}

//************************************************************************//
void LCD_ShowString (uint16_t x, uint16_t y, char *p)
{         
	while(*p!='\0')	
	{       
		if(x>=ST7735_PANEL_WIDTH)
		{ 
			x=0; 
			y = y + GlobalFont[2]-1; 
		}
		if(y>=ST7735_PANEL_HEIGHT)
		{	y=x=0;	}
		x+=LCD_FastShowChar(x,y,*p);
		p++;
	}  
}

//************************************************************************//
void LCD_ShowStringSize(uint16_t x, uint16_t y, char *p, uint32_t size)
{         
	while(size--)	
	{       
		if(x>=ST7735_PANEL_WIDTH)
		{ x=0; y=y+GlobalFont[2]-1; }
		if(y>=ST7735_PANEL_HEIGHT)
		{	y=x=0;	}
		x+=LCD_FastShowChar(x,y,*p);
		p++;
	}  
}

//************************************************************************//
// pictures were converted by https://littlevgl.com/image-to-c-array
void LCD_DrawBMP(const char* buf, int x0, int y0, int w, int h)
{  	
	int x, y;
	
  st7735SetAddrWindow(x0+0, y0+0, x0+w-1, y0+h-1);
  WriteCmd(RAMWR);  // write to RAM
  for (x=0; x < (w*h*2); x++) 
	{
		y =  buf[x]; 
    WriteData(y);     
  }
  WriteCmd(NOP);
}


//************************************************************************//
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
  uint16_t x, pixels;

  if (x1 < x0) 
	{
		x = x1;
		x1 = x0;
		x0 = x;
  }

  if (x1 >= 160) // Check limits
	{	x1 = 159; }
  if (x0 >= 160) 
	{	x1 = 159; }
	
	for (pixels = x0; pixels < x1+1; pixels++) 
	{	lcdDrawPixel(pixels,y,color);	}
}

//************************************************************************//
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  uint16_t y, pixels;

	if (y1 < y0) 
	{
		y = y1;
		y1 = y0;
		y0 = y;
	}

  if (y1 >= 128) // Check limits
	{	y1 = 127; }
  if (y0 >= 128) 
	{ y0 = 127;	}
	
	for (pixels = y0; pixels < y1 + 1; pixels++) 
	{	lcdDrawPixel(x,pixels,color);	}
}

//************************************************************************//
void LcdDrawRectangle(uint16_t x0,uint16_t x1,uint16_t y0,uint16_t y1,uint16_t color)
{
	uint16_t x,y;
	
	if(x0>x1) { x=x0;	x0=x1; x1=x; }
	if(y0>y1) { y=y0;	y0=y1; y1=y; }
	if(x0>=160) { x0 = 159;}
	if(x1>=160) { x1 = 159;}
	if(y0>=128) { y0 = 127;}
	if(y1>=128) { y1 = 127;}
	
	for(x=x0;x<=x1;x++) 
	{
		for(y=y0;y<y1;y++) 
		{	lcdDrawPixel(x,y,color); }
	}
}

//************************************************************************//
void LcdDrawGraphSimple(uint32_t *buf, uint32_t color)
{
	uint32_t i;
	signed int tmp;
	for(i=0;i<22;i++) 
	{
		tmp=124-buf[i]/1;
		if(tmp < 0) 
		{ tmp = 0;}
		LcdDrawRectangle(4+7*i,10+7*i,124,tmp,color);
	}
}

//************************************************************************//
void LcdDrawGraph(uint32_t *bufLow, uint32_t *bufMiddle, uint32_t *bufHigh)
{
	uint32_t i;
	int32_t tmp;
	
	for(i=0;i<22;i++) 
	{
		tmp=124-bufMiddle[i]/8;
		if(tmp < 0) 
		{ tmp = 0;}
		LcdDrawRectangle(4+7*i,10+7*i,124, tmp, blue);
		
		tmp=124-bufLow[i]/8;
		if(tmp < 0) 
		{ tmp = 0;}
		LcdDrawRectangle(5+7*i,9+7*i, tmp, tmp+2, green);
		
		tmp=124-bufHigh[i]/8;
		if(tmp < 0) 
		{ tmp = 0;}
		LcdDrawRectangle(5+7*i,9+7*i, tmp, tmp+2, red);
	}
}

//************************************************************************//
void LcdDrawUvGraph(uint32_t Low,uint32_t Middle, uint32_t High)
{
	signed int tmp;
	
	tmp=124-Middle/8;
	if(tmp < 0) 
	{ tmp = 0;}
	LcdDrawRectangle(70,90,124,tmp,blue);
	
	tmp=124-Low/8;
	if(tmp < 0) 
	{ tmp = 0;}
	LcdDrawRectangle(70,90,tmp,tmp+2,green);
	
	tmp=124-High/8;
	if(tmp < 0) 
	{ tmp = 0;}
	LcdDrawRectangle(70,90,tmp,tmp+2,red);
}

//************************************************************************//
void LcdDrawASGraph(uint32_t left,uint32_t right)
{
	signed int tmp;
	
	tmp=123-left/4;
	if(tmp < 24) { tmp = 24;}
	LcdDrawRectangle(40,60,124,tmp,blue);
	
	tmp=123-right/4;
	if(tmp < 24) { tmp = 24;}
	LcdDrawRectangle(100,120,124,tmp,blue);
}

//************************************************************************//
void LcdDrawMgGraph(int *buf, int low, int high)
{
	signed int tmp;
	int i;
	
	for(i=0;i<6;i++) {
		tmp=123-buf[i]/3;
		if(tmp < 24) { tmp = 24;}
		LcdDrawRectangle(20+20*i,38+20*i,124,tmp,blue);
		
		tmp=124-high/3;
		if(tmp < 0) { tmp = 0;}
		LcdDrawRectangle(10,150,tmp,tmp+2,red);
		
		tmp=124-low/3;
		if(tmp < 0) { tmp = 0;}
		LcdDrawRectangle(10,150,tmp,tmp+2,green);
	}
}

//************************************************************************//
	
