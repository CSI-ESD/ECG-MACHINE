/**********************************************************************
*   File name: buttons.c
*   File owner: Milan Alden
*   Most recent editor: Milan Alden
*   This is the implementation file for buttons.c
***********************************************************************/

#include <msp430.h>
#include "buttons.h"

static void button_released_enter( Button *current_button, EventQueue *queue );
static void button_released_timer( Button *current_button, EventQueue *queue );
static void button_maybe_pressed_enter( Button *current_button, EventQueue *queue );
static void button_maybe_pressed_timer( Button *current_button, EventQueue *queue );
static void button_pressed_enter( Button *current_button, EventQueue *queue );
static void button_pressed_timer( Button *current_button, EventQueue *queue);
static void enter_button_state( enum Button_state new_state, Button *current_button, EventQueue *queue );

#define MINIMUM_PRESS_TIME 100
#define MAXIMUM_RELEASE_TIME 100
#define BUTTON_HELD_TIME 200

#define TRUE 1
#define FALSE 0


int interval_time = 5;


int is_button_pressed( Button *current_button, EventQueue *queue )
{
    Event e;

    if( read_q(queue, &e))
    {
        if( e.event == Button_event_press)
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

/**********************************************************************
*   Function name: set_button_interval_time
*   Desc: set the interval time this function is called from,
*         allowing the button handler to time each button press
***********************************************************************/
void set_button_interval_time( int button_interval_time )
{
    interval_time = button_interval_time;
}

/**********************************************************************
*   Function name: current_button_pressed
*   Desc: Check if the current button has been pressed, returning true
*         if pressed and false if not pressed
***********************************************************************/
int current_button_pressed( Button *current_button )
{
    int button_pressed = FALSE;

    if( current_button->button_num == Button1 && (P1IN & BIT1) == 0 )
    {
        button_pressed = TRUE;
    }
    else if ( current_button->button_num == Button2 && (P4IN & BIT5) == 0 )
    {
        button_pressed = TRUE;
    }

    return button_pressed;
}
/**********************************************************************
*   Function name: button_released_enter
*   Desc: perform initialisation for the Button_released state
***********************************************************************/
void button_released_enter( Button *current_button, EventQueue *queue )
{
}

/**********************************************************************
*   Function name: button_released_timer
*   Desc: change state to Button_maybe_pressed if current button is
*         pressed
***********************************************************************/
void button_released_timer( Button *current_button, EventQueue *queue )
{
    if( current_button_pressed( current_button ) )
    {
        enter_button_state(Button_maybe_pressed, current_button, queue );
    }
}

/**********************************************************************
*   Function name: button_maybe_pressed_enter
*   Desc: perform initialisation for the Button_maybe_pressed state
***********************************************************************/
void button_maybe_pressed_enter( Button *current_button, EventQueue *queue )
{
    current_button->press_time = 0;
}

/**********************************************************************
*   Function name: button_maybe_pressed_timer
*   Desc: de-bounce button press, changing state to Button_released if
*         released and to Button_pressed if button remains pressed for
*         a certain amount of time
***********************************************************************/
void button_maybe_pressed_timer( Button *current_button, EventQueue *queue )
{
    const Event e = { Button_event_press, 0, 0, 0 };

    current_button->press_time++;
    if( !current_button_pressed( current_button ) )
    {
        if (current_button->press_time >= (MINIMUM_PRESS_TIME/interval_time ))
        {

            if (!write_q(queue, e))
            {
                //queue full
            }
        }
        enter_button_state(Button_released, current_button, queue);
    }
    else
    {
        if( current_button->press_time >= (BUTTON_HELD_TIME/interval_time))
        {
            enter_button_state(Button_pressed, current_button, queue );
        }
    }
}

/**********************************************************************
*   Function name: button_pressed_enter
*   Desc: perform initialisation for the button pressed state
***********************************************************************/
void button_pressed_enter( Button *current_button, EventQueue *queue )
{
    const Event e = { Button_event_press, 0, 0, 0 };

    current_button->press_time = 0;

    if (!write_q(queue, e))
    {
        //queue full
    }
}

/**********************************************************************
*   Function name: button_pressed_timer
*   Desc: increment button press after a set time, and change state to
*         Button_maybe_released if button is released
***********************************************************************/
void button_pressed_timer( Button *current_button, EventQueue *queue)
{
    current_button->press_time++;
    if( !current_button_pressed( current_button ) )
    {
        enter_button_state( Button_maybe_released, current_button, queue );
    }
    else
    {
        if( current_button->press_time >= (BUTTON_HELD_TIME/interval_time ))
        {
            enter_button_state( Button_pressed, current_button, queue );
        }
    }
}


/**********************************************************************
*   Function name: button_maybe_released_enter
*   Desc: perform initialisation for the button_maybe_released state
***********************************************************************/
void button_maybe_released_enter( Button *current_button, EventQueue *queue )
{
    current_button->release_time = 0;
}

/**********************************************************************
*   Function name: button_maybe_released_timer
*   Desc: de-bounce button release, changing state to Button_pressed if
*         pressed and to Button_released if button remains released for
*         a certain amount of time
***********************************************************************/
void button_maybe_released_timer( Button *current_button, EventQueue *queue)
{
    current_button->release_time++;

    if( !current_button_pressed( current_button ))
    {
        if (current_button->release_time >= (MAXIMUM_RELEASE_TIME/interval_time))
        {
            enter_button_state(Button_released, current_button, queue);
        }
    }
    else
    {
        enter_button_state(Button_pressed, current_button, queue);
    }
}

/**********************************************************************
*   Function name: enter_button_state
*   Desc: enter a new state (can be same state, re-initialising state
*         variables
***********************************************************************/
void enter_button_state( enum Button_state new_state, Button *current_button, EventQueue *queue )
{
    switch( new_state )
    {
        case Button_null:
            break;
        case Button_released:
            button_released_enter( current_button, queue );
            break;
        case Button_maybe_pressed:
            button_maybe_pressed_enter( current_button, queue );
            break;
        case Button_pressed:
            button_pressed_enter( current_button, queue );
            break;
        case Button_maybe_released:
            button_maybe_released_enter( current_button, queue );
        default:
            break;
    }

    current_button->current_state = new_state;
}

/**********************************************************************
*   Function name: button_timer
*   Desc: enter current state
***********************************************************************/
void button_timer( Button *current_button, EventQueue *queue )
{
    switch ( current_button->current_state )
    {
        case Button_null:
            break;
        case Button_released:
            button_released_timer( current_button, queue );
            break;
        case Button_maybe_pressed:
            button_maybe_pressed_timer( current_button, queue );
            break;
        case Button_pressed:
            button_pressed_timer( current_button, queue );
            break;
        case Button_maybe_released:
            button_maybe_released_timer( current_button, queue );
        default:
            break;
    }
}
