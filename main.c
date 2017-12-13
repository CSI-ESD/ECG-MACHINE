#include <msp430.h>
#include <stdbool.h>
#include "event_queue.h"
#include "buttons.h"
#include "displayLib.h"
#include "menuSimple.h"
#include <stdbool.h>

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
long int systemtimer = 75000; //set this back to 0 before demoing
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
#define BUTTON_PROCESS_EVENT_TIME 20




#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)   // Timer0 A0 1ms interrupt service routine
{
    systemtimer++; //this is currently ticking far too often, it may pay to put it in its own timer that triggers less often
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
    if(systemtimer <= 15000)
    {
        writeFormattedText(startupMessage1, strlen(startupMessage1), 0, 0, 12, true);
    }
    if(systemtimer >= 15000 && systemtimer <= 30000)
    {
        writeFormattedText(startupMessage2, strlen(startupMessage2), 0, 0, 12, true);
    }
    if(systemtimer >= 40000 && systemtimer <= 50000)
    {
        writeFormattedText(helpMessageButtons, strlen(helpMessageButtons), 0, 0, 12, true);
    }
    if(systemtimer >= 50000 && systemtimer <= 75000)
    {
        writeFormattedText(helpMessageButtons2, strlen(helpMessageButtons2), 0, 0, 12, true);
    }
    if(systemtimer >= 75000)
    {
        ECGstate = 2;       //current it defaults to going to the menu for the prototype however should probabaly go to the waveform mode by default.
    }
}

int MenuCurrentSelection = 0;
int asdf = 0;     //FOR THE LOVE OF GOD RENAME THIS ONCE DONE TESTIING
void menuflowhandling()
{
    initialiseMenuBoxes(3);
     if (button1pushed == 1) // scroll down one section
     {
         if(MenuCurrentSelection == 3)
         {  //handles what 'choice' is currently selected.
             MenuCurrentSelection = 0;  //textboxs  'wrap' around
             asdf = 0;
         }
         else
         {
             MenuCurrentSelection++;
             asdf++;
         }
     }
     if (button2pushed == 1)// 'enter' key
     {

     }

     writeTextBoxes(0); //needs to be configured
     populateTextBox(t3MenuSubOptions1[0], 2, 1, true);
}


int main(void) {

    fullInit(); //initlization hardware

        initialise_button1();
        initialise_button2();

        set_button_interval_time( button_sampling_rate );

   while(1)
   {
       initDisplayBuffer(0xFF);
       // this startup phase stuff should maybe moved to its own function to keep main clean. Needs feedback from group
       if(ECGstate == 0) //if in startup mode, this is only run once
       {
           startuphandling();
       }



       if (ECGstate == 1)
       {  // if in 'Waveform mode'
           // send sensor data to waveform creating function
           // send waveform to display function
           // send number to screen directly via writeText or call a function that does so
       }

       if (ECGstate == 2) // menu mode
       {
           menuflowhandling();
       }


       outputDisplayBuffer();

   }


return 0;
}



