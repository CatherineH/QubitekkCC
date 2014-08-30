//print the strings sent to the LCD on the screen
#define ECHOLCD 0
//the number of milliseconds in the switch debounce
#define SWITCHDEBOUNCE 10
//the amount of time to wait between selecting the channel and reading the number
#define FPGA_READ_DELAY 1
//defined the refresh cycle
//assuming a 29.4 MHz clock and a target frequency of 10 Hz, this should be:
#define TIMESCALE 2940000
//The amount of time to wait for LCD commands
#define WAITTIME 1
//The amount of time to hold the LCD "enable" pin high
#define ENABLEDOWN 0
///serial interface variables
#define BINBUFSIZE  15
#define BOUTBUFSIZE 15
#define  CH_ESCAPE	27
#define  B_BAUDRATE	19200L

//The following are global variables for the CC
//The size of the coincidence window, can be values of 0, 1, 2, 3, 4, 5, 6, or 7
int window;
//The amount of time in count enable mode before the clear signal is sent to the counters
//This number is in seconds, with a lower resolution of a milisecond
float dwelltime;
//A marker for the highlighted menu option
int menuoption;
//Count enable mode; 1 for count enable, 0 for free-running mode
int enable;
//gate enable mode; 1 for gate enable (the gate is anded with the signal) or 0 for off
int gateenable;
//the amount of time in miliseconds that the "Counts cleared" or "Count Enable" messages appear on the screen
int screenholdoff;
//The following "text" strings are used to store the characters for the LCD screen
char text0[21];
char text2[21];
char text3[21];
char text1[21];
//Serialin is the buffer from the incoming serial connection
char serialin[17];
//serialout is the buffer to the outgoing serial connection
char serialoutput[15];
//the first chunk of the SCPI commands
char serialfirst[5];
//the second chunk of the SCPI commands
char serialsecond[5];
//a placeholder pointer for searching strings
char * loc;
//the following are where the count values are stored in memory
unsigned long chan0counts;
unsigned long chan1counts;
unsigned long coinccounts;
//helper numbers for formatting numbers on the LCD
unsigned long ones;
unsigned long thousands;
unsigned long millions;
unsigned long billions;


/*** LCD Functions */

void wake_up()
{
   long t0;

   //send command 0x30 to LCD - Wakeup
   BitWrPortI(PFDR, &PFDRShadow, 0, 6); // Bit 0
   BitWrPortI(PFDR, &PFDRShadow, 0, 4); // Bit 1
   BitWrPortI(PBDR, &PBDRShadow, 0, 7); // Bit 2
   BitWrPortI(PEDR, &PEDRShadow, 0, 6); // Bit 3
   BitWrPortI(PEDR, &PEDRShadow, 1, 4); // Bit 4
   BitWrPortI(PEDR, &PEDRShadow, 1, 1); // Bit 5
   BitWrPortI(PGDR, &PGDRShadow, 0, 7); // Bit 6
   BitWrPortI(PGDR, &PGDRShadow, 0, 5); // Bit 7

   //Pull D/I Port Low to Send Instruction
   BitWrPortI(PEDR, &PEDRShadow, 0, 7);

   //Pulse E Port High Then Low to Execute
   BitWrPortI(PFDR, &PFDRShadow, 1, 7);

   //wait for 10 milliseconds
   t0 = MS_TIMER+WAITTIME;
   while ((long)MS_TIMER - t0 < 0)
      /* do nothing*/;

   BitWrPortI(PFDR, &PFDRShadow, 0, 7);
}

void function_set_8bit()
{
   long t0;
   //send command 0x30 to LCD - Wakeup
   BitWrPortI(PFDR, &PFDRShadow, 0, 6); // Bit 0
   BitWrPortI(PFDR, &PFDRShadow, 0, 4); // Bit 1
   BitWrPortI(PBDR, &PBDRShadow, 0, 7); // Bit 2
   BitWrPortI(PEDR, &PEDRShadow, 0, 6); // Bit 3
   BitWrPortI(PEDR, &PEDRShadow, 0, 4); // Bit 4
   BitWrPortI(PEDR, &PEDRShadow, 1, 1); // Bit 5
   BitWrPortI(PGDR, &PGDRShadow, 0, 7); // Bit 6
   BitWrPortI(PGDR, &PGDRShadow, 0, 5); // Bit 7

   //Pull D/I Port Low to Send Instruction
   BitWrPortI(PEDR, &PEDRShadow, 0, 7);

   //Pulse E Port High Then Low to Execute
   BitWrPortI(PFDR, &PFDRShadow, 1, 7);

   //wait for 10 milliseconds
   t0 = MS_TIMER+WAITTIME;
   while ((long)MS_TIMER - t0 < 0)
      /* do nothing*/;

   BitWrPortI(PFDR, &PFDRShadow, 0, 7);
}

void command(char c)
{
	//send a command to the LCD in 4-bit mode
	long t0;
   BitWrPortI(PEDR, &PEDRShadow, ((c>>4)& 0x01), 4); // Bit 4
   BitWrPortI(PEDR, &PEDRShadow, ((c>>5)& 0x01), 1); // Bit 5
   BitWrPortI(PGDR, &PGDRShadow, ((c>>6)& 0x01), 7); // Bit 6
   BitWrPortI(PGDR, &PGDRShadow, ((c>>7)& 0x01), 5); // Bit 7
   //Pull D/I Port Low to Send Instruction
   BitWrPortI(PEDR, &PEDRShadow, 0, 7);

   //Pull Read/Write Port Low to Write Instruction
   BitWrPortI(PFDR, &PFDRShadow, 0, 5);

   //Pulse E Port High Then Low to Execute
   BitWrPortI(PFDR, &PFDRShadow, 1, 7);

   //wait for 10 milliseconds
   t0 = MS_TIMER+WAITTIME;
   while ((long)MS_TIMER - t0 < 0){}
      /* do nothing*/;

   BitWrPortI(PFDR, &PFDRShadow, 0, 7);

   //wait for 10 milliseconds
   t0 = MS_TIMER+ENABLEDOWN;
   while ((long)MS_TIMER - t0 < 0){}

   BitWrPortI(PEDR, &PEDRShadow, ((c>>0)& 0x01), 4); // Bit 4
   BitWrPortI(PEDR, &PEDRShadow, ((c>>1)& 0x01), 1); // Bit 5
   BitWrPortI(PGDR, &PGDRShadow, ((c>>2)& 0x01), 7); // Bit 6
   BitWrPortI(PGDR, &PGDRShadow, ((c>>3)& 0x01), 5); // Bit 7
   printf("Second command: %c: %d: %d%d%d%d\n",c,c,((c>>0)& 0x01),((c>>1)& 0x01),((c>>2)& 0x01),((c>>3)& 0x01));

   //Pull D/I Port Low to Send Instruction
   BitWrPortI(PEDR, &PEDRShadow, 0, 7);

   //Pull Read/Write Port Low to Write Instruction
   BitWrPortI(PFDR, &PFDRShadow, 0, 5);

   //Pulse E Port High Then Low to Execute
   BitWrPortI(PFDR, &PFDRShadow, 1, 7);

   //wait for 10 milliseconds
   t0 = MS_TIMER+WAITTIME;
   while ((long)MS_TIMER - t0 < 0)  {}
      /* do nothing*/;

   BitWrPortI(PFDR, &PFDRShadow, 0, 7);
   //wait for 10 milliseconds
   t0 = MS_TIMER+ENABLEDOWN;
   while ((long)MS_TIMER - t0 < 0){}
}
void data(char c)
{
	//send data to the LCD in four-bit mode
   long t0;
   BitWrPortI(PEDR, &PEDRShadow, ((c>>4)& 0x01), 4); // Bit 4
   BitWrPortI(PEDR, &PEDRShadow, ((c>>5)& 0x01), 1); // Bit 5
   BitWrPortI(PGDR, &PGDRShadow, ((c>>6)& 0x01), 7); // Bit 6
   BitWrPortI(PGDR, &PGDRShadow, ((c>>7)& 0x01), 5); // Bit 7
   //Pull D/I Port High to Send character
   BitWrPortI(PEDR, &PEDRShadow, 1, 7);

   //Pulse E Port High Then Low to Execute
   BitWrPortI(PFDR, &PFDRShadow, 1, 7);

   //wait for 10 milliseconds
   t0 = MS_TIMER+WAITTIME;
   while ((long)MS_TIMER - t0 < 0){}

   BitWrPortI(PFDR, &PFDRShadow, 0, 7);
   //wait for 10 milliseconds
   t0 = MS_TIMER+ENABLEDOWN;
   while ((long)MS_TIMER - t0 < 0){}

   BitWrPortI(PEDR, &PEDRShadow, ((c>>0)& 0x01), 4); // Bit 4
   BitWrPortI(PEDR, &PEDRShadow, ((c>>1)& 0x01), 1); // Bit 5
   BitWrPortI(PGDR, &PGDRShadow, ((c>>2)& 0x01), 7); // Bit 6
   BitWrPortI(PGDR, &PGDRShadow, ((c>>3)& 0x01), 5); // Bit 7
   //Pull D/I Port High to Send character
   BitWrPortI(PEDR, &PEDRShadow, 1, 7);

   //Pulse E Port High Then Low to Execute
   BitWrPortI(PFDR, &PFDRShadow, 1, 7);

   //wait for 10 milliseconds
   t0 = MS_TIMER+WAITTIME;
   while ((long)MS_TIMER - t0 < 0){}

   BitWrPortI(PFDR, &PFDRShadow, 0, 7);
   //wait for 10 milliseconds
   t0 = MS_TIMER+ENABLEDOWN;
   while ((long)MS_TIMER - t0 < 0){}
}

void Home()
{
	//go to the home location
	long t0;
   command(0x00);
   //wait for 10 milliseconds
   t0 = MS_TIMER+WAITTIME;
   while ((long)MS_TIMER - t0 < 0){}
}

void disp_pic()
{
	//send the text* strings to the LCD
   int disp_i;
   for (disp_i=0;disp_i<20;disp_i++)
   {
      data(text0[disp_i]);
   }
   for (disp_i=0;disp_i<20;disp_i++)
   {
      data(text2[disp_i]);
   }
   for (disp_i=0;disp_i<20;disp_i++)
   {
      data(text1[disp_i]);
   }
   for (disp_i=0;disp_i<20;disp_i++)
   {
      data(text3[disp_i]);
   }
}
//initializes the lines to the LCD on the Rabbit
void initOutputLCD()
{
   //D4-D7
   BitWrPortI(PEDDR, &PEDDRShadow,1,4);
   BitWrPortI(PEDDR, &PEDDRShadow,1,1);
   BitWrPortI(PGDDR, &PGDDRShadow,1,7);
   BitWrPortI(PGDDR, &PGDDRShadow,1,5);
   //D/I
   BitWrPortI(PEDDR, &PEDDRShadow,1,7);
   //DRW
   BitWrPortI(PFDDR, &PFDDRShadow,1,5);
   //DE
   BitWrPortI(PFDDR, &PFDDRShadow,1,7);
}
//runs through the LCD initialization commands
void initLCD()
{
   long initt0;

   initt0 = MS_TIMER+50;
   while ((long)MS_TIMER - initt0 < 0){}
   //Wakeup LCD - must be done three times
   wake_up();

   initt0 = MS_TIMER+5;
   while ((long)MS_TIMER - initt0 < 0){}

   wake_up();

   initt0 = MS_TIMER+5;
   while ((long)MS_TIMER - initt0 < 0){}

   wake_up();

   initt0 = MS_TIMER+5;
   while ((long)MS_TIMER - initt0 < 0){}
     //Wait 5 milliseconds
   function_set_8bit();
   command(0x28);
   command(0x0C);
   command(0x06);
   Home();
}

//display  menu
void coincidenceWindowMenu(int window, int gateEnable, int dwelltime, int menuoption, int rotator)
{
   sprintf(text0,"Enable to Select    ");
   //if the menu option is selected, put in the arrow character(126)
   if(menuoption ==2)
   {
   	//the rotator will cycle between 0 and 1 if the option is selected,
      //this control structure makes the value blink on and off
   	if(rotator)
      {
   		sprintf(text1,"%cCoin Window: %d ns  ",126,window);
      }
      else
      {
         sprintf(text1,"%cCoin Window:   ns  ",126);
      }
   }
   else
   {
   	sprintf(text1," Coin Window: %d ns  ",window);
   }
   if(menuoption ==1)
   {
   	if(rotator)
      {
   		sprintf(text2,"%cDwell Time: %3.2f s",126,dwelltime);
      }
      else
      {
         sprintf(text2,"%cDwell Time:    s",126);
      }
   }
   else
   {
   	sprintf(text2," Dwell Time: %2d s   ",dwelltime);
   }
   if(gateEnable)
   {
   	if(menuoption ==0)
      {
      	if(rotator)
         {
   			sprintf(text3,"%cGate Input Enabled ",126);
         }
         else
         {
         	sprintf(text3,"%cGate Input         ",126);
         }
      }
      else
      {
      	sprintf(text3," Gate Input Enabled ");
      }
   }
   else
   {
   	if(menuoption==0)
      {
      	if(rotator)
         {
   			sprintf(text3,"%cGate Input Disabled",126);
         }
         else
         {
         	sprintf(text3,"%cGate Input         ",126);
         }
      }
      else
      {
         sprintf(text3," Gate Input Disabled");
      }
   }
}
//clearNotice() and countEnableNotice() set the screen to put up a message when the clear or enable buttons are pressed
void clearNotice()
{
   sprintf(text0,"                    ");
   sprintf(text1,"                    ");
   sprintf(text2,"Counts Cleared      ");
   sprintf(text3,"                    ");
}
void countEnableNotice()
{
   sprintf(text0,"                    ");
   sprintf(text1,"                    ");
   sprintf(text2,"Count Enable        ");
   sprintf(text3,"                    ");
}
//this message will appear when the rabbit gets the kill signal from the LTC2954
void goodbyeNotice()
{
   sprintf(text0,"                    ");
   sprintf(text1,"                    ");
   sprintf(text2,"Goodbye!            ");
   sprintf(text3,"                    ");
}
/*** FPGA functions */
#define PORTA_AUX_IO
//initialize the rabbit pins to the FPGA
void initOutputFPGA()
{
	//Set the FPGA ports with the count numbers to input
   BitWrPortI(PBDDR, &PBDDRShadow, 0, 6);
   BitWrPortI(PBDDR, &PBDDRShadow, 0, 5);
   BitWrPortI(PBDDR, &PBDDRShadow, 0, 3);
   BitWrPortI(PFDDR, &PFDDRShadow, 0, 4);
   BitWrPortI(PGDDR, &PGDDRShadow, 0, 2);
   BitWrPortI(PGDDR, &PGDDRShadow, 0, 0);
   BitWrPortI(PDDDR, &PDDDRShadow, 0, 4);
   BitWrPortI(PDDDR, &PDDDRShadow, 0, 6);
   BitWrPortI(PDDDR, &PDDDRShadow, 0, 7);
   BitWrPortI(PFDDR, &PFDDRShadow, 0, 0);
   BitWrPortI(PBDDR, &PBDDRShadow, 0, 4);
   BitWrPortI(PBDDR, &PBDDRShadow, 0, 2);
   //set overflow to input
   BitWrPortI(PBDDR,&PBDDRShadow,0,5);
   //Set the channel ports to output
   BitWrPortI(PGDDR, &PGDDRShadow,1,3);
   BitWrPortI(PGDDR, &PGDDRShadow,1,1);
   //Set the count enable, and clear counts to output
   BitWrPortI(PDDDR, &PDDDRShadow,1,5);
   BitWrPortI(PDDDR, &PDDDRShadow,1,0);
   //Win 0 set to output
   BitWrPortI(PFDDR, &PFDDRShadow, 1, 1);

   //send clear counts and enable to 0
   BitWrPortI(PDDR, &PDDRShadow,0,5);
   BitWrPortI(PDDR, &PDDRShadow,0,0);
}
//read the pins in from the FPGA and convert to a decimal number
unsigned long readNumber()
{
   int fpga[21];
	unsigned long ifpga;
   unsigned long number;
   //check for overflow
   if(BitRdPortI(PBDR,5)==0)
   {
   	fpga[0] = BitRdPortI(PBDR, 6);
	   fpga[1] = BitRdPortI(PBDR, 5);
	   fpga[2] = BitRdPortI(PBDR, 3);
	   fpga[3] = BitRdPortI(PADR, 6);
	   fpga[4] = BitRdPortI(PADR, 4);
	   fpga[5] = BitRdPortI(PADR, 2);
	   fpga[6] = BitRdPortI(PCDR, 1);
	   fpga[7] = BitRdPortI(PFDR, 4);
	   fpga[8] = BitRdPortI(PGDR, 2);
	   fpga[9] = BitRdPortI(PGDR, 0);
	   fpga[10] = BitRdPortI(PDDR, 4);
	   fpga[11] = BitRdPortI(PDDR, 6);
	   fpga[12] = BitRdPortI(PDDR, 7);
	   fpga[13] = BitRdPortI(PADR, 0);
	   fpga[14] = BitRdPortI(PFDR, 0);
	   fpga[15] = BitRdPortI(PBDR, 4);
	   fpga[16] = BitRdPortI(PBDR, 2);
	   fpga[17] = BitRdPortI(PADR, 7);
	   fpga[18] = BitRdPortI(PADR, 5);
	   fpga[19] = BitRdPortI(PADR, 3);
	   fpga[20] = BitRdPortI(PADR, 1);
	   //printf("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n",fpga[0],fpga[1],fpga[2],fpga[3],fpga[4],fpga[5],fpga[6],fpga[7],fpga[8],fpga[9],fpga[10],fpga[11],fpga[12],fpga[13],fpga[14],fpga[15],fpga[16],fpga[17],fpga[18],fpga[19],fpga[20]);
	   number = 0;
	   for(ifpga=0;ifpga<21;ifpga++)
	   {
	      number += (1 << ifpga)*fpga[ifpga];
	   }
   }
   else
   {
   	number = -1;
   }
   return number;
}
//send the coincidence window to the FPGA
void sendWindow(int window)
{

   int wind0;
   int wind1;
   int wind2;

   wind0 = (window % 2);
   window = window/2;
   wind1 = (window % 2);
   window = window/2;
   wind2 = (window % 2);

   BitWrPortI(PFDR, &PFDRShadow,wind0,1);
   BitWrPortI(PCDR, &PCDRShadow,wind1,2);
   BitWrPortI(PCDR, &PCDRShadow,wind2,0);
}
//select the channel to read, 10 for chan1, 01 for chan2, 00 for coincidences
void selectChannel(int ch0, int ch1)
{
   BitWrPortI(PGDR, &PGDRShadow,ch0,3);
   BitWrPortI(PGDR, &PGDRShadow,ch1,1);
}
//send the clear signal to the FPGA
void clearCounts()
{
	long t0;
   BitWrPortI(PDDR,&PDDRShadow,1,0);
	t0 = MS_TIMER+10;
   while ((long)MS_TIMER - t0 < 0)
	BitWrPortI(PDDR,&PDDRShadow,0,0);
}
//turn on the gate enable signal to the FPGA
void gateEnableOn()
{
   BitWrPortI(PDDR,&PDDRShadow,1,5);
}
//turn off the gate enable signal to the FPGA
void gateEnableOff()
{
   BitWrPortI(PDDR,&PDDRShadow,0,5);
}


/*** Switches functions */
//initialize the switch pins on the rabbit
void initSwitches()
{
	//SW4
	BitWrPortI(PEDDR, &PEDDRShadow,0,5);
   //SW3
   BitWrPortI(PEDDR, &PEDDRShadow,0,2);
   //SW5
   BitWrPortI(PEDDR, &PEDDRShadow,0,0);
   //SW3
   BitWrPortI(PGDDR, &PGDDRShadow,0,6);
   //SW7
   BitWrPortI(PGDDR, &PGDDRShadow,0,4);
}
// SW5 is clear
void pressClear()
{
	costate{
   	waitfor(BitRdPortI(PEDR,0));
	   waitfor(DelayMs(SWITCHDEBOUNCE));          // Unpressed switch detected
   }

}
//read the channels from the FPGA and display the channel counts on the LCD
void readDisplay()
{
		//select each channel individually and read the numbers from the FPGA
     	selectChannel(0,0);
      coinccounts = readNumber();
      selectChannel(1,0);
      chan0counts = readNumber();
      selectChannel(0,1);
      chan1counts = readNumber();
      if(chan0counts ==0 || chan1counts ==0)
	   {
	      coinccounts = 0;
	   }

      //if the LCD isn't currently showing a clear counts or count enable message,
      //format the numbers
	   if(!screenholdoff)
	   {
      		//With the limited string functions available, to make the numbers easier to read with commas separating the thousands,
            //the numbers must be broken up into thousands, millions and billions.
            //these then have different formatting rules based on the upper value of the number
            //for example, if the number is less than a thousand, don't put commas in but space out the number so that it lines up on the LCD screen:
            if(chan0counts<0)
            {
            	sprintf(text0,"Channel1:   Overflow");
            }
            if(chan0counts<1000 && chan0counts>=0)
	         {
               sprintf(text0,"Channel1:%11lu",chan0counts);
	         }
            //if the number is between a thousand and a million, split the number up into the first three digits and the next three
            //put a comma between the numbers,
            //then space out the number so that it lines up on the display.
	         if(chan0counts>=1000 && chan0counts<1000000)
	         {
	            ones = (chan0counts % 1000);
	            thousands = chan0counts/1000;
	            thousands = (thousands % 1000);
	            sprintf(text0,"Channel1:%7lu,%03lu",thousands,ones);
	         }
            //and so on...
	         if(chan0counts>=1000000 && chan0counts<1000000000)
	         {
	            ones = (chan0counts % 1000);
	            thousands = chan0counts/1000;
	            thousands = (thousands % 1000);
	            millions = chan0counts/1000;
	            millions = millions/1000;
	            millions = (millions % 1000);
	            sprintf(text0,"Channel1:%3lu,%03lu,%03lu",millions,thousands,ones);
	         }
	         if(chan0counts>=1000000000)
	         {
	            ones = (chan0counts % 1000);
	            thousands = chan0counts/1000;
	            thousands = (thousands % 1000);
	            millions = chan0counts/1000;
	            millions = millions/1000;
	            millions = (millions % 1000);
	            billions = chan0counts/1000;
	            billions = billions/1000;
	            billions = billions/1000;
	            billions = (billions % 1000);
	            sprintf(text0,"Channel1:%lu,%03lu,%03lu,%03lu",billions,millions,thousands,ones);
	         }
            if(chan1counts<0)
            {
            	sprintf(text1,"Channel2:   Overflow");
            }
	         if(chan1counts<1000 && chan1counts>=0)
	         {
               sprintf(text1,"Channel2:%11lu",chan1counts);
	         }
	         if(chan1counts>=1000 && chan1counts<1000000)
	         {
	            ones = (chan1counts % 1000);
	            thousands = chan1counts/1000;
	            thousands = (thousands % 1000);
	            sprintf(text1,"Channel2:%7lu,%03lu",thousands,ones);
	         }
	         if(chan1counts>=1000000 && chan1counts<1000000000)
	         {
	            ones = (chan1counts % 1000);
               thousands = chan1counts/1000;
               thousands = (thousands % 1000);
	            millions = chan1counts/1000;
	            millions = millions/1000;
	            millions = (millions % 1000);
	            sprintf(text1,"Channel2:%3lu,%03lu,%03lu",millions,thousands,ones);
	         }
	         if(chan1counts>=1000000000)
	         {
	            ones = (chan1counts % 1000);
               thousands = chan1counts/1000;
               thousands = (thousands % 1000);
	            millions = chan1counts/1000;
	            millions = millions/1000;
	            millions = (millions % 1000);
	            billions = chan1counts/1000;
	            billions = billions/1000;
	            billions = billions/1000;
	            billions = (billions % 1000);
	            sprintf(text1,"Channel2:%lu,%03lu,%03lu,%03lu",billions,millions,thousands,ones);
	         }
            if(coinccounts<0)
            {
            	sprintf(text3,"Coincide:   Overflow");
            }
	         if(coinccounts<1000 && coinccounts>=0)
	         {
               sprintf(text3,"Coincide:%11lu",coinccounts);
	         }

	         if(coinccounts>=1000 && coinccounts<1000000)
	         {
	            ones = (coinccounts % 1000);
	            thousands = coinccounts/1000;
	            thousands = (thousands % 1000);
	            sprintf(text3,"Coincide:%7lu,%03lu",thousands,ones);
	         }
	         if(coinccounts>=1000000 && coinccounts<1000000000)
	         {
	            ones = (coinccounts % 1000);
	            thousands = coinccounts/1000;
	            thousands = (thousands % 1000);
	            millions = coinccounts/1000;
	            millions = millions/1000;
	            millions = (millions % 1000);
	            sprintf(text3,"Coincide:%3lu,%03lu,%03lu",millions,thousands,ones);
	         }
	         if(coinccounts>=1000000000)
	         {
	            ones = (coinccounts % 1000);
               thousands = coinccounts/1000;
               thousands = (thousands % 1000);
	            millions = coinccounts/1000;
	            millions = millions/1000;
	            millions = (millions % 1000);
	            billions = coinccounts/1000;
	            billions = billions/1000;
	            billions = billions/1000;
	            billions = (billions % 1000);
	            sprintf(text3,"Coincide:%lu,%03lu,%03lu,%03lu",billions,millions,thousands,ones);
	         }
            //if the gate enable is on, put a little "play" triangle on the third line
	         if(enable)
	         {
	            sprintf(text2,"%c%c                  ",124,62);

	         }
	         else
	         {
	            sprintf(text2,"                    ");
	         }
	         if(ECHOLCD)
	         {
	            printf("LCD:%s\n",text0);
	            printf("LCD:%s\n",text1);
	            printf("LCD:%s\n",text2);
	            printf("LCD:%s\n",text3);
	         }
	}
}

main(){
   //toggles for buttons
   int pressedenable;
   int presseddown;
   int pressedup;
   int pressedmenu;
   int pressedclear;
   //menuon is 0 when the menu is closed, 1 for when the menu is open
   int menuon;
   //selected is 0 when no menu option has been selected, 1 for when no menu option has been selected
   int selected;
   //the rotator variable allows the LCD menu items to blink
   int rotator;
   //the index of the characters read in from the serial connection
   int seri;
   //was the command from the serial input understood
   int serialparsed;
   //the input window from the serial connection
   int windowi;
   //the input dwell time from the serial connection
   float tmpdwell;
   //the character read in from the serial connection
   char c;
   //these are used to help with timing the clear signals
   unsigned long startTime;
   unsigned long stopTime;
   //initialize the POW_INT and POW_KILL pins
   //POW_INT is an input signal from the LTC2954 chip that informs of a power
   //button press
   BitWrPortI(PBDDR, &PBDDRShadow, 0, 7);
   //POW_KILL is an output signal to the LTC2954 that tells it to shut down
   BitWrPortI(PEDDR, &PEDDRShadow, 1, 6);
   //POW_KILL must be high
   BitWrPortI(PEDR, &PEDRShadow, 1, 6);


	printf("Initialize the connections\n");
	//initialize the LCD and FPGA connections
   initOutputLCD();
   initOutputFPGA();
   initSwitches();
   //send wake up commands to LCD
   printf("Initialize LCD\n");
   initLCD();
   //initalize all the button tangles to off
   pressedenable = 0;
   presseddown = 0;
   pressedup = 0;
   pressedmenu = 0;
   pressedclear = 0;
   //initialize the menu as closed
   menuon = 0;
   //the default value of the window is 1 ns
   window = 1;
   //count enable mode is initialized to off
   enable = 0;
   //the screenholdoff is off
   screenholdoff = 0;
   menuoption = 0;
   selected = 0;
   rotator = 0;
   //the default dwelltime is 2 s
   dwelltime = 2;
   chan0counts = 0;
   chan1counts = 0;
   coinccounts = 0;
   //open the serial connection
   serCopen(B_BAUDRATE);
   while (1)
   {
   	costate
      {
      	//check for POW_INT pin notice
      	if(!BitRdPortI(PEDR, 2))
         {
         	goodbyeNotice();
            disp_pic();
            DelayMs(100);
            //send the kill signal
            BitWrPortI(PEDR,&PEDRShadow,0,6);
         }
      }
   	costate
      {
      	//if more than 16 characters have been read from the serial connection
         //with no carriage return, start from the front of the buffer again
      	if(seri>=16)
         {
      		seri = 0;

         }
         while (seri <16) {
         	//this waitfor is used as a breather from the serial connection
            //so that it doesn't hang forever on waiting for the next character
         	waitfor(DelayMs(10));
            c = serCgetc();
            //if the character is a letter, number or punctuation, accept it
            if (c>31 &&c<124) {
            	 //put the character in the buffer
                serialin[seri] = c;
                //move the end of the string to the next character
                serialin[seri+1] = '\0';
                //increment the serial buffer index
                seri++;
            }
            if(c=='\n' ||c=='\r')// Exit on new line
            {
               serialin[seri] = '\0';
               //initialize serialparsed, a new command or query has been received,
               //next find out if it is correct
               serialparsed = 0;
               //set the index to 16 to break out of the loop
               seri =16;
            }
         }
         printf("Got command: %s\n",serialin);
         //if the first character of the string is a colon, start parsing it as a command
         if(serialin[0]==':')
         {
         	 //copy characters 1-5 to serialfirst as the first part of the SCPI command
         	 memcpy(serialfirst,&serialin[0]+1,4);
             //make sure that serialfirst then ends
             serialfirst[4] = '\0';
             //set window via serial
         	if(strcmp(serialfirst,"WIND")==0)
	         {
            	//search for the second part of the string, looking for a colon
            	loc = strchr(serialin,32);
               //if the second semicolon has been found
               if(loc != NULL)
               {
               	//acknowledge that the command was valid
               	serialparsed = 1;
                  //copy characters 1-2 after the second colon to serialsecond
	               memcpy(serialsecond,loc+1,2);
                  //attempt to parse this string as an integer
	               windowi = atoi(serialsecond);
                  //if the window is valid, set the window to the value
	               if(windowi<8 && windowi>=0)
	               {
	                    window = windowi;
	               }
               }
	         }
	         //set dwell time via serial
            //same as for window, look for the second colon
	         if(strcmp(serialfirst,"DWEL")==0)
	         {
               loc = strchr(serialin,32);
               if(loc != NULL)
               {
               	serialparsed = 1;
	               memcpy(serialsecond,loc+1,5);
	               tmpdwell = atof(serialsecond);

                  if(tmpdwell>=0)
	               {
	                  dwelltime = tmpdwell;
	               }
               }
	         }
            //read the :GATE:ON and :GATE:OFF commands
	         if(strcmp(serialfirst,"GATE")==0)
	         {
            	memcpy(serialin,&serialin[0]+1,7);
            	loc = strchr(serialin,':');
               if(loc !=NULL)
               {
               	memcpy(serialsecond,loc+1,3);
                  if(serialsecond[0]=='O' && serialsecond[1]=='N')
                  {
	               	serialparsed = 1;
	               	gateenable = 1;
                  }
                  else {
                  	if(serialsecond[0]=='O' && serialsecond[1]=='F' && serialsecond[2]=='F')
                     {
                        serialparsed = 1;
	               		gateenable = 0;
                     }
                  }
               }
	         }
            //read the :COUN:ON and :COUN:OFF commands
            if(strcmp(serialfirst,"COUN")==0)
	         {
            	memcpy(serialin,&serialin[0]+1,7);
            	loc = strchr(serialin,':');
               if(loc !=NULL)
               {
               	memcpy(serialsecond,loc+1,3);
                  if(serialsecond[0]=='O' && serialsecond[1]=='N')
                  {
	               	serialparsed = 1;
	               	enable = 1;
                  }
                  else {
                  	if(serialsecond[0]=='O' && serialsecond[1]=='F' && serialsecond[2]=='F')
                     {
                        serialparsed = 1;
	               		enable = 0;
                     }
                  }
               }
	         }

         }
         else
         {
         	//if the string didn't start with a colon, attempt to parse it as a query
            memcpy(serialfirst,&serialin[0],4);
            serialfirst[4] = '\0';
            if(strcmp(serialfirst,"WIND")==0)
	         {
               sprintf(serialoutput,"%d ns\n",window);
	            serCputs(serialoutput);
	            serialparsed = 1;
	         }
	         if(strcmp(serialfirst,"DWEL")==0)
	         {
	            sprintf(serialoutput,"%d s\n",dwelltime);
	            serCputs(serialoutput);
	            serialparsed = 1;
	         }
	         if(strcmp(serialfirst,"GATE")==0)
	         {
	            if(gateenable)
	            {
	               sprintf(serialoutput,"1\n");
	            }
	            else
	            {
	                sprintf(serialoutput,"0\n");
	            }
	             serCputs(serialoutput);
	             serialparsed = 1;
	         }
	         if(strcmp(serialfirst,"COUN")==0)
	         {
            	//like the commands, this query has two parts, separated by a colon
            	loc = strchr(serialin,':');
               if(loc==NULL)
               {
                  if(enable)
               	{
	                  sprintf(serialoutput,"1\n");
	               }
	               else
	               {
	                   sprintf(serialoutput,"0\n");
	               }
	                serCputs(serialoutput);
	                serialparsed = 1;
               }
               memcpy(serialsecond,loc+1,3);
               if(serialsecond[0]=='C' && serialsecond[1]=='1')
	            {
	                sprintf(serialoutput,"%lu\n",chan0counts);
	                serCputs(serialoutput);
	                serialparsed = 1;
	            }
	            if(serialsecond[0]=='C' && serialsecond[1]=='2')
	            {
	               sprintf(serialoutput,"%lu\n",chan1counts);
	               serCputs(serialoutput);
	               serialparsed = 1;
	            }
	            if(serialsecond[0]=='C' && serialsecond[1]=='O')
	            {
	                sprintf(serialoutput,"%lu\n",coinccounts);
	                serCputs(serialoutput);
	                serialparsed = 1;
	            }
	         }
            if(strcmp(serialfirst,"CLEA")==0)
         	{
	            serialparsed = 1;
	            clearCounts();
            }
            if(strcmp(serialin,"*IDN")==0)
	         {
               serCputs("Qubitekk,CC1,Rev. 1\n");
	            serialparsed = 1;
	         }
	         if(strcmp(serialin,"*RST")==0)
	         {
            	//set all values to default
	            gateenable =0;
	            window = 1;
	            dwelltime = 2;
               enable = 0;
               serialparsed = 1;
	         }
         }
         //if the command was not understood, reply with Unknown command
         if(serialparsed ==0 )
         {
         	serCputs("Unknown command\n");
            serialparsed = 1;
         }

      }
   	//put all the button press listeners in different costates
   	costate
      {
         if (!BitRdPortI(PEDR, 2))
	      {
            if(pressedenable)
            {
               if(menuon)
	            {
               	selected =0;
	               //if the menu is already on, pressing menu again will send the window
	               //value to the FPGA
	               sendWindow(window);
                  if(gateenable)
               	{
	                  gateEnableOn();
                  }
	               else{
	                  gateEnableOff();
                  }
	               //the display does not get turned off, the menuon value gets changed,
	               //and then a costate changes the display based on the menuon
	               menuon = 0;
               }
	            else
	            {
	               menuon = 1;
               }
               pressedenable = 0;
            }

	      }
         else
         {
            pressedenable = 1;
         }
         //release this costate so that it is not hanging on waiting for the button to operate
         waitfor(DelayMs(100));

      }
      costate
      {
        	   if(!BitRdPortI(PEDR,5))
            {
            	if(pressedup)
               {
               	printf("press up\n");
                  //the window should only increase if the menu is currently on
	               if(menuon)
	               {
                  	if(!selected)
                     {
                         menuoption++;
                         menuoption = (menuoption % 3);
                     }
                     else{
                     	if(menuoption==2)
                        {
	                  		window++;
	                  		//the window should not be bigger than 8
	                  		window = (window % 8);
                        }
                        if(menuoption==1)
                        {
                        	dwelltime++;
                           dwelltime = dwelltime - floor(dwelltime/60.0)*60;
                        }
                        if(menuoption==0)
                        {
                           gateenable++;
                           gateenable = (gateenable % 2);
                        }
                     }

	               }
                  pressedup = 0;
               }
            }
            else
            {
            	pressedup = 1;
            }

         waitfor(DelayMs(100));
      }
      costate
      {
         if(!BitRdPortI(PGDR,6))
      	{
            if(presseddown)
            {
               //the window should only increase if the menu is currently on
	               if(menuon)
	               {
                  	if(!selected)
                     {
                         menuoption--;
                         if(menuoption<0)
                         {
                         	menuoption = menuoption+3;
                         }
                     }
                     else{
                     	if(menuoption ==2)
                        {
	                  		window--;
                           if(window<0)
                           {
                           	window = window+8;
                           }
                        }
                        if(menuoption==1)
                        {
                        	dwelltime--;
                           if(dwelltime<0)
                           {
                           	dwelltime = dwelltime + 60;
                           }
                        }
                        if(menuoption==0)
                        {
                           gateenable--;
                           if(gateenable<0)
                           {
                           	gateenable = gateenable +2;
                           }
                        }
                     }

	               }
               presseddown = 0;
            }
         }
         else
         {
         	presseddown = 1;
         }
         waitfor(DelayMs(100));
      }
      costate
      {
      	//If the menu is off, then enable prevents the clear counts signal from being sent to the FPGA
         //If the menu is on, then enable sends the countEnable signal (which is actually the gateEnable signal)
         if (!BitRdPortI(PGDR, 4))
	      {
            if(menuon)
	         {
                if(selected)
                {
                	selected = 0;
                }
                else{
                	selected = 1;
                }
            }
	         else
	         {
	            if(enable)
	            {
	               enable = 0;
	            }
	            else
	            {
               	enable = 1;
	               countEnableNotice();
                  screenholdoff = 1;
                  //pause and show the count enable message for 0.5 s
	            	waitfor(DelayMs(500));
	            	screenholdoff = 0;
	            }

	         }
	      }
         waitfor(DelayMs(100));

      }
      costate
      {
          if (!BitRdPortI(PEDR, 0))
          {
          		if(pressedclear)
               {
                   clearCounts(); // send clear signal
	                clearNotice();
	                screenholdoff = 1;
                   //pause and show the counts cleared message for 0.5 s
	                waitfor(DelayMs(500));
	                screenholdoff = 0;
                   pressedclear = 0;
                }
          }
          else
          {
          	pressedclear = 1;
          }
          waitfor(DelayMs(100));
      }
      costate
      {
         if(menuon)
         {
         	if(selected)
            {
            	//if an option has been selected, make the value flash by changing the rotator
            	coincidenceWindowMenu(window,gateenable,dwelltime,menuoption,rotator);
               if(rotator)
               {
               	rotator = 0;
               }
               else
               {
                	rotator= 1;
               }
            }
            else
            {
            	//otherwise, if no option has been selected make the arrow flash by moving it offscreen (setting the menuoption to -1)
               //and back to screen (moving to the selected menu option)
            	if(rotator)
               {
                  coincidenceWindowMenu(window,gateenable,dwelltime,-1,1);
               	rotator = 0;
               }
               else
               {
                  coincidenceWindowMenu(window,gateenable,dwelltime,menuoption,1);
               	rotator = 1;
               }
            }
            //display the generated menu
            disp_pic();
         }
         else
         {
         	//start the counting sequence
         	startTime = MS_TIMER;
            if(enable)
            {
            	//if count enable mode is on, continuously read the screen until the dwelltime has passed
            	stopTime = startTime + (int)(dwelltime*1000);
               while(stopTime>MS_TIMER)
            	{
                  readDisplay();
                  disp_pic();
	            }

            }
            else
            {
            	//if not, read after 0.1 s and clear the display
               waitfor(DelayMs(100));
               readDisplay();
               disp_pic();
            }
            //if count enable is on, hold the screen for 2 seconds to hold the numbers
            if(enable)
            {
            	waitfor(DelaySec(2));
            }
            //clear the counts
            BitWrPortI(PDDR,&PDDRShadow,1,0);
            DelayMs(2);

            BitWrPortI(PDDR,&PDDRShadow,0,0);

         }

      }
   }

}