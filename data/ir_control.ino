#include <IRremote.h>
#include "MachineDefs.h"
 
IRrecv irrecv(RECV_PIN);
decode_results results;  

#ifdef PIONEER_DVD_REMOTE
/******************** PIONEER_DVD_REMOTE **********************/
#define CODE_BIG_PRINT 39D41DC6    //up
#define CODE_SMALL_PRINT 0xF50ACF30  //downd
#define CODE_LEFT_REEL_IN E0984BB6   //step
#define CODE_LEFT_REEL_OUT 0xf50aed12  //slow
#define CODE_RIGHT_REEL_IN 0xc53ad926   //rev
#define CODE_RIGHT_REEL_OUT 0xc53a59a6  //fwd
#define CODE_LEFT_CALIBRATION A32AB931  //prev
#define CODE_RIGHT_CALIBRATION 1D2FEFF6 //next
#define CODE_ENABLE_CONT_DRIVE 371A3C86  //return
#define CODE_DISABLE_CONT_DRIVE 0xf50af708 //enter
#define CODE_STOP 0xC53A19E6 //pause
#define CODE_RESUME 0xc53a7986 //play
#define CODE_1 0xf50a857a
#define CODE_2 0xf50a45ba
#define CODE_3 0xf50ac53a
#define CODE_4 0xF50A25DA
#define CODE_5 0xF50AA55A
#define CODE_6 0xF50A659A
#define CODE_7 0xF50AE51A
#define CODE_8 0xF50A15EA
#define CODE_9 0xF50A956A
#define CODE_0 0xF50A05FA

#elif defined NONAME_WHITE_REMOTE
/******************** NONAME_WHITE_REMOTE **********************/
#define CODE_BIG_PRINT 0xff629d //+
#define CODE_SMALL_PRINT 0xffe21d //-
#define CODE_LEFT_REEL_IN 0xfff20d //left
#define CODE_LEFT_REEL_OUT 0xffea15 //right
#define CODE_RIGHT_REEL_IN 0xffca35 //up
#define CODE_RIGHT_REEL_OUT 0xff18e7 //down
#define CODE_LEFT_CALIBRATION 0xff32cd //title
#define CODE_RIGHT_CALIBRATION 0xff0af5 //subtitle
#define CODE_ENABLE_CONT_DRIVE 0xff10ef //open/close
#define CODE_DISABLE_CONT_DRIVE 0xff00ff //standby
#define CODE_STOP 0xff827d //setup
#define CODE_RESUME 0xffa25d //center button of setup
#define CODE_1 0xff807f
#define CODE_2 0xffa05f
#define CODE_3 0xff906f 
#define CODE_4 0xff40bf
#define CODE_5 0xff609f
#define CODE_6 0xff50af 
#define CODE_7 0xffc03f
#define CODE_8 0xffe01f 
#define CODE_9 0xffd02f

#else
#error What remote?
#endif 


void setupIR() 
{
  irrecv.enableIRIn(); // Start the receiver  
}

void readIR()
{
  float lDist;
  bool fail = false;
  
  if (irrecv.decode(&results)) {         
    switch(results.value) {
#ifndef NO_REMOTE      
       case 0xF50A3DC2:  //power
         storePositionInEEPROM();
               
         break;         
       case CODE_BIG_PRINT:  //up
         printSize = 2;
         break;
       case CODE_SMALL_PRINT:  //down
         printSize = 0.5;
         break;
       case CODE_LEFT_REEL_IN: //left -
         manualLeft = -1;
         break;
       case CODE_LEFT_REEL_OUT: //left +
         manualLeft = 1;
         break;
       case CODE_RIGHT_REEL_IN: //right -
         manualRight = -1;
         break;
       case CODE_RIGHT_REEL_OUT: //right +
         manualRight = 1;
         break;
       case CODE_LEFT_CALIBRATION:  //prev - calibrate 200 mm from left
         currentLeftSteps = 200*stepsPerMM;
         testPen();         
         break;
       case CODE_RIGHT_CALIBRATION:  //next - calibrate 200 mm from right
         currentRightSteps = 200*stepsPerMM;              

         lDist = currentLeftSteps/stepsPerMM;
         disparity = (long)sqrt(lDist*lDist-200L*200L);

         SER_PRINT("Cal: lDist)");
         SER_PRINTLN(lDist);

         SER_PRINT("Cal: Disp=");
         SER_PRINTLN(disparity);

         break;   
       case CODE_DISABLE_CONT_DRIVE: 
         continousManualDrive = false;
         break;
       case CODE_ENABLE_CONT_DRIVE: 
         continousManualDrive = true;
         break;
       case CODE_STOP:  
         stopPressed = true;
#if  CODE_DISABLE_CONT_DRIVE == 0xBADC0DE
        //just disable continous drive when pressing stop. Re-enable with CODE_ENABLE_CONT_DRIVE again
         continousManualDrive = false;
#endif
         break;
       case CODE_RESUME:
          //resume print, or start new
          program = 1; //start print
          resumePlot = true;       
         break;
       case CODE_1: program = 1; currentPlot = 1; break;
       case CODE_2: program = 1; currentPlot = 2; break;
       case CODE_3: program = 1; currentPlot = 3; break;
       case CODE_4: program = 1; currentPlot = 4; break;
       case CODE_5: program = 1; currentPlot = 5; break;
       case CODE_6: program = 1; currentPlot = 6; break;
       case CODE_7: program = 1; currentPlot = 7; break;
       case CODE_8: program = 1; currentPlot = 8; break;
       case CODE_9: program = 1; currentPlot = 9; break;       
       default:
          fail=true;           
#endif //NO_REMOTE         
    }   
    if(fail) {
       SER_PRINT("???: ");
    }
    else {
       makePenNoise(1);
    }
    SER_PRINTLN2(results.value, HEX);      
    
    irrecv.resume(); // Receive the next value
  }  
}
