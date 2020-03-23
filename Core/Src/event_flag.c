/*******************************************************************************
 * Filename:			event_flag.c
 * Revised:				Date: 2020.02.20
 * Revision:			V001
 * Description:		    Event flag function
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "event_flag.h"
#include "software_timer.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define DEBOUNCE_DELAY	50

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/
volatile uint32_t eventFlags = 0;

/*******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************/
uint8_t debounceTimerId[4];

/*******************************************************************************
 * @fn      DebounceTimerCallback
 * @brief   Debounce timer callback
 * @paramz  softwareTimerId
 * @return  None
 ******************************************************************************/
void DebounceTimerCallback(uint8_t softwareTimerId)
{
	eventFlags |= (0x01 << softwareTimerId);
}

/*******************************************************************************
 * EVENT FLAG FUNCTIONS
 ******************************************************************************/
static void Button1PressedEventFlag(void);
static void Button2PressedEventFlag(void);
static void Button3PressedEventFlag(void);
static void Button4PressedEventFlag(void);

// Initial event flag jump table
static void (*EventFlags[])(void) =
{
	Button1PressedEventFlag,
	Button2PressedEventFlag,
	Button3PressedEventFlag,
	Button4PressedEventFlag,
};

/*******************************************************************************
 * @fn      Button1PressedEventFlag
 * @brief   Button 1 pressed event flag
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void Button1PressedEventFlag(void)
{
	if(HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin) == GPIO_PIN_RESET)
	{
		printf("Button 1 pressed\n");
	}
}

/*******************************************************************************
 * @fn      Button2PressedEventFlag
 * @brief   Button 2 pressed event flag
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void Button2PressedEventFlag(void)
{
	// Normally using delay solve the debounce problem
//	HAL_Delay(50);
	if(HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin) == GPIO_PIN_RESET)
	{
		printf("Button 2 pressed\n");
	}
}

/*******************************************************************************
 * @fn      Button3PressedEventFlag
 * @brief   Button 3 pressed event flag
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void Button3PressedEventFlag(void)
{
	if(HAL_GPIO_ReadPin(BUTTON3_GPIO_Port, BUTTON3_Pin) == GPIO_PIN_RESET)
	{
		printf("Button 3 pressed\n");
	}
}

/*******************************************************************************
 * @fn      Button4PressedEventFlag
 * @brief   Button 4 pressed event flag
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void Button4PressedEventFlag(void)
{
	if(HAL_GPIO_ReadPin(BUTTON4_GPIO_Port, BUTTON4_Pin) == GPIO_PIN_RESET)
	{
		printf("Button 4 pressed\n");
	}
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/
/*******************************************************************************
 * @fn		MainLoop
 * @brief	Main loop
 * @param	None
 ******************************************************************************/
// https://blog.csdn.net/liangsir_l/article/details/50707864
void MainLoop(void)
{
    uint8_t i = 0;

    // Enable software timer
    sSoftwareTimer.Enable();

    for(i = 0; i < 4; i++)
    {
  	  debounceTimerId[i] = sSoftwareTimer.Initialize(NULL, DebounceTimerCallback, NULL, TIMER_ONCE_TYPE);
    }

    for(;;)
    {
        if(eventFlags != 0)
        {
            for(i = 0; i < maximumEventFlag; i++)
            {
            	// Detected event flag
                if(((eventFlags >> i) & 0x01) == 0x01)
                {
                	// Call related function through jump table
                    (*EventFlags[i])();
                    // Clear event flag
                    eventFlags &= (~(0x01 << i));
                }
            }
        }
    }
}

/*******************************************************************************
 * INTERRUPT CALLBACK
 ******************************************************************************/
/*******************************************************************************
 * @fn      HAL_GPIO_EXTI_Callback
 * @brief   EXTI line detection callback
 * @param	gpioPin
 * @return	None
 ******************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t gpioPin)
{
	// No debounce, direct use event flags
//	switch(gpioPin)
//	{
//		case BUTTON1_Pin:
//			eventFlags |= (0x01 << button1PressedEventFlag);
//			break;
//		case BUTTON2_Pin:
//			eventFlags |= (0x01 << button2PressedEventFlag);
//			break;
//		case BUTTON3_Pin:
//			eventFlags |= (0x01 << button3PressedEventFlag);
//			break;
//		case BUTTON4_Pin:
//			eventFlags |= (0x01 << button4PressedEventFlag);
//			break;
//		default:
//			break;
//	}
	// Using timer solve the debounce problem
	switch(gpioPin)
	{
		case BUTTON1_Pin:
			sSoftwareTimer.Start(debounceTimerId[0], DEBOUNCE_DELAY);
			break;
		case BUTTON2_Pin:
			sSoftwareTimer.Start(debounceTimerId[1], DEBOUNCE_DELAY);
			break;
		case BUTTON3_Pin:
			sSoftwareTimer.Start(debounceTimerId[2], DEBOUNCE_DELAY);
			break;
		case BUTTON4_Pin:
			sSoftwareTimer.Start(debounceTimerId[3], DEBOUNCE_DELAY);
			break;
		default:
			break;
	}
}
