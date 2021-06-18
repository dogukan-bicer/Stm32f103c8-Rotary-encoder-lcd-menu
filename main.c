#include "stm32f10x.h" 
#include "systick_time.h"
#include "gp_drive.h" 
#include "SPI_drive.h"
#include "n5110_drive.h"


/*
Pin kurulumu

SPI - 1
--> 
PA0 -->RST
PA1 --> DC
PA2 -->Ekran isigi
PA4 --> SS
PA5 --> SCLK
PA7 --> MOSI

3.3V --> Vcc
G --> Grd

PA9 -->mavi led
PA11 -->yesil led (blink)
PA12 -->kirmizi led

PA8 -->clk encoder
PA10 -->data encoder

PA6 -->Analog giris

*/


unsigned char n5110_buffer[6][84];
char numara[5];
char degerler[5];
char encoder[5];
char blink[5];
char pwm[4];
char analog[5];
int imlec=0;
uint16_t sayici=0;
uint16_t PinDT=0;
uint16_t deger=0;
uint16_t sayac = 80;
uint16_t durum = 0;
uint16_t sondurum = 0;
uint32_t zaman = 0;
uint32_t eskiZaman=0;
uint8_t buton = 0;
uint8_t ledbuton = 0;
uint8_t islem = 0;
uint8_t sonislem = 0;
uint8_t altmenu = 0;
uint8_t azalt = 0;
uint8_t artir = 0;
uint16_t butonsay = 0;
uint16_t eskibutonsay = 0;
uint16_t yenibutonsay = 0;
uint16_t butonsure= 0;
uint8_t tus = 0;
uint8_t pwmilk = 0;
uint16_t analogveri = 0;
int dongu=0;
int secim = 0;
int LEDMAVI=0;
int LEDKIRMIZI=0;

void systick_kurulum()
 {
	 SysTick->LOAD =72000000/8000;//her milisaniye 0 a denk
	 SysTick->VAL=0;
	 SysTick->CTRL = SysTick->CTRL = (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
 }

void timer_ayarlari(){
	
  RCC->APB2ENR|=(1<<11);// timer aktif
	
  TIM1->CCMR1 &= ~(3 << 8); // kanal 2 aktif
	
	TIM1->CCMR1|= (6<<12); //capture/compare regiser pa_9 için ayarlandi

  TIM1->CCER |= (1 << 4);  //capture/compare ENABLE KANAL2 CIKIS AYARLANDI
	
	TIM1->BDTR |= (1<<15);   //Break and dead-time register. ana cikis aktif
  
	TIM1->ARR=3600;//2khz
  TIM1->PSC=9; //PRESCALER
  TIM1->CCR2=0; //DUTY CYCLE %10  //////////////////////////////
 
	TIM1->CR1|=1; //TIMER BASLADI

}

int butonanahtar(){
		if(GPIOA->IDR & (1<<3)) /// Checking status of PIN ! portA  A9 pini
		{
			tus=!tus;
		}
		while(GPIOA->IDR & (1<<3))	
		{
		 DelayMs(10);
		}
		return tus;
	}


	
	
uint32_t millis() {
	DelayMs(1);
	zaman++;
	return zaman;
}

  ///////////////////////////////////////////////////////////////////////////////////////////BUTON		
	/*	deger=GPIOA->IDR & (1<<3); //deger=GPIOA->IDR & 0B0000000000001000;
		if(deger==8){
		buton=!buton;
		butonsay++;
		}
		deger=GPIOA->IDR & (1<<3); //deger=GPIOA->IDR & 0B0000000000001000;
		while(deger==8)	{		// deger 0 oldugunda cikicak
			DelayMs(10);
			
			deger=GPIOA->IDR & (1<<3);
		}		
		basmasuresi=millis()-eskiZaman;  */
		
		

  ///////////////////////////////////////////////////////////////////////////////////////////BUTON


void int2char(int num, char str[])
{
char lstr[30];
int cnt = 0;
int div = 10;
int j = 0;

	
	
while( num >= div)
{
	lstr[cnt] = num % div + 0x30;
	num /= 10;
	cnt++;
}
	lstr[cnt] = num + 0x30;
for(j= cnt ; j >=0;j--)
{
	str[cnt-j] = lstr[j];
}

}

  ///////////////////////////////////////////////////////////////////////////////////////////Geçen zaman
	uint32_t calismazamani(){
    if(millis()-eskiZaman >15)
		{
		eskiZaman = millis();
		sayici++;
		int2char(sayici,numara);
		}
		return sayici;
	}
	  ///////////////////////////////////////////////////////////////////////////////////////////Geçen zaman

uint16_t butonsuresi (){
		dongu++;
		DelayMs(10);
  	islem = GPIOA->IDR & (1<<3); //deger=GPIOA->IDR & 0B0000000000001000; 
		if (islem!=sonislem){
		deger=GPIOA->IDR & (1<<3); //deger=GPIOA->IDR & 0B0000000000001000;			
		  if(deger==0)
	    {
       butonsure=dongu;
			 butonsay=butonsure-eskibutonsay;
		  }
		  else
		   {
			 eskibutonsay=dongu;
		   }
		 }
		sonislem = GPIOA->IDR & (1<<3); //deger=GPIOA->IDR & 0B0000000000001000;	
		 return butonsay;
}
/*
int butontoggle (){
	deger=GPIOA->IDR & (1<<3); //deger=GPIOA->IDR & 0B0000000000001000;			
		  if(deger==8)
	    {
			 buton=!buton;
       DelayMs(1);
		  }
	 return buton;
   }  */


 ///////////////////////////////////////////////////////////////////////////////////////////Encoder kontrol
 int encoderkontrol (){
		durum=GPIOA->IDR & 0B0000000100000000; //a8 CLK;	256=1
 if(durum!=sondurum){
	 PinDT=GPIOA->IDR & 0B0000010000000000; //a9 DT	1024=1
	   if(PinDT==1024){
		 PinDT=256;
		 } 
   if(PinDT!=sondurum){ 
		artir=!artir;
    if(artir==0)	{
			sayac++;
     }	 
   }
   else {
		azalt=!azalt;
    if(azalt==0)	{
			sayac--;
     }		
	 }
 }
    sondurum=durum;
    return sayac;
 }
  ///////////////////////////////////////////////////////////////////////////////////////////Encoder kontrol	
   
 
 void Delays(int time) /// Random delay function
{
	int t;
	for(;time>0;time--)
	{
	 for(t=0;t<100000;t++)
		{}
	}
}
 
 
int main(void)
{
systick_init();
n5110_init(1);
//n5110_clear();
//////////////////////////////////////////////////////////////////////////////////ADC kurulumu		
			RCC_APBENR |= 0x4;
	   // GPIOA6
	    GPIOA->CRL &= 0xF0FFFFFF; /// A3 pini resetlendi
	    GPIOA->CRL |= 0x00000000; /// A3 pini girisi push pull
	    
	    RCC->APB2ENR |= 0x201;
			ADC1->CR2 = 0;
			ADC1->SQR3 = 6;
			ADC1->CR2 |= 1;
			DelayMs(100);
			ADC1->CR2 |= 1;
			ADC1->CR2 |= 2;
//////////////////////////////////////////////////////////////////////////////////ADC kurulumu	
	RCC->APB2ENR |= (1<<2);  //  GPIOA Clock aktif
	RCC->APB2ENR |= (1<<4);  //  GPIOC Clock aktif
	GPIOA->CRL &= 0xFFFF0FFF; /// A3 pini resetlendi
	GPIOA->CRL |= 0x00008000; /// A3 pini girisi push pull
	GPIOA->CRH &= 0xFFFFF0F0; /// A9 ve A8 pini resetlendi
	GPIOA->CRH |= 0x00000808; /// A9 ve A8 pini girisi push pull
  GPIOA->CRL &= 0xFFFFF0FF; /// A9 ve A8 pini resetlendi
	GPIOA->CRL |= 0x00000300; /// A9 ve A8 pini girisi push pull
	GPIOA->CRH &= 0xFFF00F0F; /// A9 ve A8 pini resetlendi
	GPIOA->CRH |= 0x00033030; /// A9 ve A8 pini girisi push pull

/////////////////////////////////////////////////////////////////////////////////////////////////////////External interrupt	
	/*	__disable_irq();
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PA; ///   A8 deki interrupt aktif edildi
	EXTI->IMR |= EXTI_IMR_MR3; ///  EXTI8 aktif
	EXTI->RTSR |= EXTI_RTSR_TR3; /// interrupt rising edge olarak ayarlandi
	NVIC_EnableIRQ(EXTI3_IRQn); // Genel interrupt 8 fonksiyonu calistirildi
	__enable_irq(); */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	/*		__disable_irq();
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI2_PA; ///   A8 deki interrupt aktif edildi
	EXTI->IMR |= EXTI_IMR_MR10; ///  EXTI8 aktif
	EXTI->RTSR |= EXTI_RTSR_TR10; /// interrupt rising edge olarak ayarlandi
	NVIC_EnableIRQ(EXTI15_10_IRQn); // Genel interrupt 8 fonksiyonu calistirildi
	__enable_irq(); */
/////////////////////////////////////////////////////////////////////////////////////////////////////////External interrupt		
  sondurum=GPIOA->IDR & 0B0000000100000000; //a8 CLK;
	RCC->APB2ENR|=(1<<2)|1;//ADC CLOCK AKTIF aport
 GPIOA->ODR  |= 0x0004;
 GPIOA->ODR  |= 0x0800;
 GPIOA->ODR  |= 0x1000;
 GPIOA->ODR  |= 0x0200;
 ///////////////////////////////////////////////////////////////////////////////////////////Açilis
	update_str_buffer(2, 5,"  Ho$geldiniz  ",n5110_buffer);
	print_buffer(n5110_buffer);
	DelayMs(1500);
 ///////////////////////////////////////////////////////////////////////////////////////////Açilis
	while(1)
	{ 
	//	GPIOA->ODR  = ~0x0800;
	  clear_buffer(n5110_buffer);
 /*  while(butonsay<15){
		update_str_buffer(3, 0,"cikmak icin basili tutun",n5110_buffer);
		print_buffer(n5110_buffer);
		DelayMs(35);
		butonsuresi(); 
	 }  */

	  encoderkontrol();
		


  ///////////////////////////////////////////////////////////////////////////////////////////MENÜ
		imlec=(sayac%8)+1;
    calismazamani();
		if (imlec<6){
		update_str_buffer(imlec, 0,">",n5110_buffer);
		update_str_buffer(0, 2,"------MEN#------",n5110_buffer);
		update_str_buffer(1, 7,"Led ayar|   ",n5110_buffer);
		update_str_buffer(2, 7,"Blink         ",n5110_buffer);
		update_str_buffer(3, 7,"PWM Sinyali",n5110_buffer);
    update_str_buffer(4, 7,"Saya~",n5110_buffer);
    update_str_buffer(5, 7,"Yard|m",n5110_buffer);
		}
		else{
		  if(imlec==6) {
			altmenu=1;}
			else{
			altmenu=imlec-5;}
		update_str_buffer(altmenu, 0,">",n5110_buffer);
		update_str_buffer(0, 2,"------MEN#------",n5110_buffer);
		update_str_buffer(1, 7,"Encoder test",n5110_buffer);
		update_str_buffer(2, 7,"Analog",n5110_buffer);
		update_str_buffer(3, 7,"Resetle",n5110_buffer);			
		}
		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		deger=GPIOA->IDR & (1<<3); //deger=GPIOA->IDR & 0B0000000000001000; 
		if(imlec==1 && deger!=0){
		 while(butonsay<3){
		  clear_buffer(n5110_buffer);
		  encoderkontrol();
		 
		  secim=(sayac%2);
		  update_str_buffer(0, 4,"---Led ayar|---",n5110_buffer);
		  update_str_buffer(2, 0,"Led se~in",n5110_buffer);
      update_str_buffer(4, 10,"Mavi",n5110_buffer);
		  update_str_buffer(4, 40,"K|rm|z|",n5110_buffer);
		   if (secim==0)
		    {
			   update_str_buffer(4, 3,">",n5110_buffer);
		    }
		    else if(secim==1)
		    {
			   update_str_buffer(4, 33,">",n5110_buffer);
		    }
				
        Delays(15);
      if(GPIOA->IDR & (1<<3) && secim==1 ) //kirmizi led
			 {
          LEDKIRMIZI=!LEDKIRMIZI;
				 
			 }
		  if(LEDKIRMIZI==1)//mavi led
				      { 
					 Delays(2);
				   update_str_buffer(5, 40,"A~|k",n5110_buffer);
           GPIOA->ODR  &= ~0x1000; //a12 kirmizi
				      }					
				 else{
					 GPIOA->ODR  |= 0x1000;  //a12 kirmizi
					} 
		if(GPIOA->IDR & (1<<3) && secim==0) //mavi led
			{
          LEDMAVI=!LEDMAVI;
				
      }				
		if(LEDMAVI==1)
				{
					Delays(2);
          update_str_buffer(5, 10,"A~|k",n5110_buffer);				
         	GPIOA->ODR  &= ~0x0200; //A9 mavi		
				} 
			else{
				  GPIOA->ODR  |= 0x0200; //A9 mavi
			  }  
	  print_buffer(n5110_buffer);			   			
		butonsuresi();
  }
		    LEDKIRMIZI=0;
	      LEDMAVI=0;
		 		butonsay=0;
	      secim=0;	
	      sayac=80;
	      GPIOA->ODR  |= 0x0200; //A9 mavi
	      GPIOA->ODR  |= 0x1000; //A9 mavi
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		
		if(imlec==2 && deger!=0){
		while(butonsay<5){
		encoderkontrol();
		secim=10*((sayac%9)+1);
		clear_buffer(n5110_buffer);	
		update_str_buffer(0, 4,"-----Blink------",n5110_buffer);
		update_str_buffer(2, 0,"Blink s+resi(ms)",n5110_buffer);
		int2char(secim,blink);	
		update_str_buffer(3, 0,blink,n5110_buffer);
		DelayMs(secim);
		GPIOA->ODR  ^= 0x0800;
		DelayMs(secim);
				print_buffer(n5110_buffer);
		    butonsuresi();	
        secim=0;			
		 }
		 butonsay=0;
		 sayac=81;
     GPIOA->ODR  |= 0x0800;		 
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		if(imlec==3 && deger!=0){
			GPIOA->CRH &=~(1<<6);
      GPIOA->CRH |=(1<<7)|(3<<4);
		   while(butonsay<10){
		    clear_buffer(n5110_buffer);	
		    timer_ayarlari();
		    encoderkontrol(); 
		    secim=360*(sayac%11);
		    pwmilk=100-(10*(sayac%11));
		    update_str_buffer(0, 4,"--PWM Sinyali--",n5110_buffer);
	    	update_str_buffer(2, 0,"Frekans(Hz)=2khz",n5110_buffer);
	    	update_str_buffer(3, 0,"Duty cycle=",n5110_buffer);
    		int2char(pwmilk,pwm);
        update_str_buffer(3, 58,pwm,n5110_buffer);	
				 if (secim==3600)
				 {
					 GPIOA->CRH |=(1<<6);
           GPIOA->CRH  &=~(1<<7)|(3<<4);
					 update_str_buffer(3, 55," 0 ",n5110_buffer);
					 update_str_buffer(3, 68," %",n5110_buffer);
				 }
				 else if(secim==0)
				 {
					 GPIOA->CRH &=~(1<<6);
           GPIOA->CRH |=(1<<7)|(3<<4);
					 update_str_buffer(3, 58,"100",n5110_buffer);
					 update_str_buffer(3, 73,"%",n5110_buffer);
				 }
				 else
				 {
					 GPIOA->CRH &=~(1<<6);
           GPIOA->CRH |=(1<<7)|(3<<4); 
					update_str_buffer(3, 68," %",n5110_buffer);
				 }
				  TIM1->CCR2=secim;///pwm gönder
				  print_buffer(n5110_buffer);
		      DelayMs(35);
		      butonsuresi();			
		 }
		GPIOA->CRH |=(1<<6);
    GPIOA->CRH  &=~(1<<7)|(3<<4);
		butonsay=0;
		sayac=82;
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		if(imlec==4 && deger!=0){
    clear_buffer(n5110_buffer);	
		while(butonsay<10){
		update_str_buffer(0, 4,"-----Saya~-----",n5110_buffer);
		update_str_buffer(2, 0,"Cal|$ma s+resi",n5110_buffer);
		calismazamani();
		update_str_buffer(3, 0,numara,n5110_buffer);
    update_str_buffer(3, 40,"sn",n5110_buffer);	
     		print_buffer(n5110_buffer);
		    DelayMs(35);
		    butonsuresi();			
		 }
		butonsay=0;
    sayac=83;		 
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		if(imlec==5 && deger!=0){
		clear_buffer(n5110_buffer);
		while(butonsay<10){
		encoderkontrol();
		secim=(sayac%2);
			if (secim==0){
		update_str_buffer(0, 4,"-----Yard|m-----",n5110_buffer);
		update_str_buffer(1, 0,"Menu &gesini se~ mek i~in encoder| kullan|n.~|kmak i~in d+!meye bas|n .   ",n5110_buffer);
			}
      else if(secim==1)	{
			update_str_buffer(0, 4,"-----Yard|m-----",n5110_buffer);	
			update_str_buffer(1, 0,"                  Do!ukan Bi~er    Versiyon:1.0                         ",n5110_buffer);	
			}
				print_buffer(n5110_buffer);
		    DelayMs(35);
		    butonsuresi();				
		 }
		butonsay=0;
		sayac=84;
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		if(imlec==6 && deger!=0){
		while(butonsay<10){	
		clear_buffer(n5110_buffer);
		encoderkontrol();
		update_str_buffer(0, 4,"--Encoder test--",n5110_buffer);
		update_str_buffer(2, 0,"Encoder de!eri=",n5110_buffer);
		int2char(sayac,encoder);
		update_str_buffer(3, 0,encoder,n5110_buffer);	
		if(sayac<100){
		update_str_buffer(3, 10,"  ",n5110_buffer);	
		}
		else if(sayac<10){
		update_str_buffer(3, 5,"   ",n5110_buffer);		
		}
    update_str_buffer(4, 0,"Buton durumu=",n5110_buffer);
		  if (deger==0)
		  {
			update_str_buffer(5, 0,"kapal|",n5110_buffer);
		  }
		  else
		  {
			update_str_buffer(5, 0,"a~|k",n5110_buffer);
		  }	
		    print_buffer(n5110_buffer);
		    DelayMs(35);
		    butonsuresi();	
     DelayMs(35);			
		 }
		butonsay=0;
		sayac=85;
		 }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////		 
		if(imlec==7 && deger!=0){ 
		clear_buffer(n5110_buffer);
			while(butonsay<5){
		ADC1->SQR1 &= ~(0xf << 20);
    ADC1->SQR1 |= (0x1 << 20);			
     ADC1->SMPR2 &= ~(0x7);
     ADC1->SMPR2 |= (0x1);	//7.5 döngü
    //12mhz: 7.5+12.5 =20 hz ->3.3us(mikro Saniye) 
    ADC1->CR2 &= ~(1 << 11); // right alignment		
    ADC1->CR2 |= 2|1; //adc açik ve surekli
    ADC1->CR2 |= (1<<2);// kalibrasyon	
    while(ADC1->CR2 & (1<<2));

    ADC1->CR2 |=1; //adc açik	
				
    analogveri = ADC1->DR;				
		update_str_buffer(0, 4,"----Analog----",n5110_buffer);
		update_str_buffer(2, 0,"  Analog okuma ",n5110_buffer);
    int2char(analogveri,analog);
		update_str_buffer(3, 30,analog,n5110_buffer);
		if(analogveri<10){
		update_str_buffer(3, 35,"   ",n5110_buffer);	
		}				
		 else if(analogveri<100)
		 {
			update_str_buffer(3, 40,"   ",n5110_buffer);
		 }
		 else if(analogveri<1000)
		 {
			update_str_buffer(3, 45,"   ",n5110_buffer);
		 }
		print_buffer(n5110_buffer);
		DelayMs(100);
		butonsuresi();
	  }
		butonsay=0;
		sayac=86;
	  }	 
//////////////////////////////////////////////////////////////////////////////////////////////////////////		
			if(imlec==8 && deger!=0){
		 while(butonsay<10){	
		clear_buffer(n5110_buffer);	
		encoderkontrol();
		butonsuresi();
		secim=(sayac%4);
		update_str_buffer(0, 4,"----Resetle----",n5110_buffer);
		update_str_buffer(2, 7,"Yeniden Ba$lat?",n5110_buffer);
    update_str_buffer(4, 15,"Hay|r",n5110_buffer);
		update_str_buffer(4, 47,"Evet",n5110_buffer);
		  if (secim==2 || secim==3)
		  {
			 update_str_buffer(4, 8,">",n5110_buffer);
		  }
		  else if (secim==0 || secim==1)
		  {
			 update_str_buffer(4, 40,">",n5110_buffer);
		  }	
    		print_buffer(n5110_buffer);

				Delays(10);
      	deger=GPIOA->IDR & (1<<3); //deger=GPIOA->IDR & 0B0000000000001000; 
        clear_buffer(n5110_buffer);			
		    if(deger==8 && (secim==0 || secim==1)){
				  update_str_buffer(2, 3,"Resetleniyor...",n5110_buffer);
				  print_buffer(n5110_buffer);
				  DelayMs(1000);
				  //nvic_sys_reset();	
				  butonsay=20;
				  SCB->AIRCR = ((u32)0x05FA0000) | (u32)0x04;
				 }	
         else if(deger==8 && (secim==2 || secim==3)){
				  butonsay=20;
				 }				 
		  } 
		sayac=87;    
		butonsay=0;
		DelayMs(100);
		}
  ///////////////////////////////////////////////////////////////////////////////////////////MENÜ
		/*if (deger==0)
		{
			update_str_buffer(4, 55,"kapali",n5110_buffer);
		}
		else
		{
			update_str_buffer(4, 55,"acik",n5110_buffer);
		}*/
	///////////////////////////////////////////////////////////////////////Encoder Test	
	/*	    PinCLK=GPIOA->IDR & 0B0000000100000000; //a8 CLK;	
			   PinDT=GPIOA->IDR & 0B0000010000000000; //a10 DT;
	       if(PinCLK==256){
					PinCLK=1; 
				 }
				 else{
				  PinCLK=0;}
				 if(PinDT==1024){
					PinDT=1; 
				 }
				 else{
				  PinDT=0;}
		     GPIOA->ODR |= 0x0004;
				 DelayMs(20);

					

		
		int2char(PinCLK,clk);
		update_str_buffer(0, 60,clk,n5110_buffer);
	
		int2char(PinDT,dt);
		update_str_buffer(2, 60,dt,n5110_buffer);
 ///////////////////////////////////////////////////////////////////////Encoder Test		*/
 			
		print_buffer(n5110_buffer);
		DelayMs(40);
    GPIOA->ODR |=0x0004;
	}
	
 }




/*void EXTI3_IRQHandler() // A8 için Interrupt Handler fonksiyonu
{
	EXTI->PR |=1;

}*/ 


/*int EXTI15_10_IRQHandler(int16_t sondurum) // A10 için Interrupt Handler fonksiyonu
{
	EXTI->PR |=1;
  sayac=5;
	return sondurum;
} */

