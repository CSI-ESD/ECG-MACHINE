#include <msp430.h>
#include <stdbool.h>
#include <string.h>
#include "event_queue.h"
#include "buttons.h"
#include "displayLib.h"
#include "menuSimple.h"

int buttonpressedflag;
int button_sampling_rate = 2;
int sensors_sampling_rate = 1;
int ECGstate = 0;
/*
 * This int is a way of keeping track of what 'state' the screen is in - this way the scheduler can skip over unnesseary code
 *  Realistically this should be moved into its own function (which I can make) which will serve as an overlal controller for
 *  what state the board is in right now, being either startup, waveform or menu.
 *  Right now the numbers work as follows : 0 is startup/off (is the default but should be swiftly changed) 1 is waveform, 2 is menu.
 */
long int systemtimer = 0; //set this back to 0 before demoing
/* This counts how many times the timer has triggered and is a good way of keeping track of how long the system has been
 * running for. It's not only used for debugging, it's also used to decide when the system comes out of the default 'startup' screen
 * Nothing currently resets this value (although probably should). So technically this value will eventually go over the limit and risk crashing the system,
 * however this should be many cycles into the system, so this doesn't need to be implemented for the prototype phase.
 */



// these should probably be moved to the button header
Button button1;
Button button2;

EventQueue button1_q;
EventQueue button2_q;

int sample_button_timer = 1;
int sample_sensor_timer = 1;
int button1pushed = 0;
int button2pushed = 0;
Event e;

int button_process_event = 0;
#define BUTTON_PROCESS_EVENT_TIME 50


#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)   // Timer0 A0 1ms interrupt service routine
{

    systemtimer++; //this is
    //currently ticking far too often, it may pay to put it in its own timer that triggers less often
    sample_button_timer++;
    sample_sensor_timer++;
    if (sample_button_timer >= button_sampling_rate)
        {
        button_timer( &button1, &button1_q );
        button_timer( &button2, &button2_q );
            sample_button_timer = 0;
            button_process_event++;
            if( button_process_event >= BUTTON_PROCESS_EVENT_TIME )
            {
                button_process_event = 0;
                button1pushed = is_button_pressed( &button1, &button1_q );
                button2pushed = is_button_pressed( &button2, &button2_q );
            }
        }
    if (sample_sensor_timer >= sensors_sampling_rate)
            {
                //sensors
                sample_sensor_timer = 0;
            }


}


//Display buffer

/*
 *
 *  note to self move this to button header at some point*/

void initialise_button1()
{
    button1.button_num = Button1;
    button1.current_state = Button_released;
    button1.press_time = 0;
    button1.release_time = 0;
}

void initialise_button2()
{
    button2.button_num = Button2;
    button2.current_state = Button_released;
    button2.press_time = 0;
    button2.release_time = 0;
}

/*
 *
 *  note to self move this to button header at some point*/



void startuphandling()
{
    if(systemtimer <= 10000)
    {
        writeFormattedText(startupMessage1, strlen(startupMessage1), 0, 0, 12, true);
    }
    if(systemtimer >= 10000 && systemtimer <= 20000)
    {
        writeFormattedText(startupMessage2, strlen(startupMessage2), 0, 0, 12, true);
    }
    if(systemtimer >= 20000 && systemtimer <= 25000)
    {
        writeFormattedText(helpMessageButtons, strlen(helpMessageButtons), 0, 0, 12, true);
    }
    if(systemtimer >= 25000 && systemtimer <= 30000)
    {
        writeFormattedText(helpMessageButtons2, strlen(helpMessageButtons2), 0, 0, 12, true);
    }
    if((systemtimer >= 30000) || button1pushed == 1 || button2pushed == 1 )
    {
        ECGstate = 2;       //current it defaults to going to the menu for the prototype however should probabaly go to the waveform mode by default.
    }
}


int MenuCurrentSelection = 0;
enum Buttons buttonPressMask = No_button;

void menuflowhandling()
{

     if ((button1pushed == 1) && (button2pushed == 0)) // scroll down one section
     {
         buttonPressMask = Button1;
     }
     else if ((button2pushed == 1) && (button1pushed == 0)) // 'enter' key
     {

         buttonPressMask = Button2;                   /*Buttons.Button2;*/
     }
     else if((button2pushed == 1 )&&(button1pushed == 1)) // this needs to be here to 'overwrite' it being set to button 2
     {

         buttonPressMask = Num_buttons;     //only happens if bothbuttons are pressed together
     }
     else
     {
         buttonPressMask = No_button;
     }
     updateMenuBoxes( buttonPressMask );
}


int main(void) {

    fullInit(); //initlization hardware

        initialise_button1();
        initialise_button2();

        set_button_interval_time( button_sampling_rate );
        int firstimemenu = 0;

   while(1)
   {

       // this startup phase stuff should maybe moved to its own function to keep main clean. Needs feedback from group
       if(ECGstate == 0) //if in startup mode, this is only run once
       {
           initDisplayBuffer(0xFF);
           startuphandling();
           firstimemenu = 1;
       }



       if (ECGstate == 1)
       {  // if in 'Waveform mode'
           // send sensor data to waveform creating function
           // send waveform to display function
           // send number to screen directly via writeText or call a function that does so
           firstimemenu = 1;
       }

       if (ECGstate == 2) // menu mode
       {
           if(firstimemenu == 1){
               initialiseMenuBoxes(3);
               firstimemenu = 0;
           }
           if(button1pushed == 1 || button2pushed == 1){
               menuflowhandling();
           }

       }
       outputDisplayBuffer();

   }


return 0;
}
