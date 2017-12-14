/**********************************************************************
*   File name: buttons.h
*   File owner: Milan Alden
*   Most recent editor: Milan Alden
*   This is the header file for buttons.c
***********************************************************************/

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "event_queue.h"

enum Button_event { Button_event_null = 0,
                    Button_event_press,
                    Num_button_Events };

enum Buttons { Button1 = 0, Button2, Num_buttons, No_button };

enum Button_state{ Button_null = 0,
                   Button_released,
                   Button_maybe_pressed,
                   Button_pressed,
                   Button_maybe_released,
                   Num_button_states };

typedef struct
{
    enum Buttons button_num;
    enum Button_state current_state;
    unsigned int press_time;
    unsigned int release_time;
} Button;

int is_button_pressed( Button *current_button, EventQueue *queue );
void set_button_interval_time( int button_interval_time );
void button_timer( Button *current_button, EventQueue *queue );

#endif /* BUTTONS_H_ */
