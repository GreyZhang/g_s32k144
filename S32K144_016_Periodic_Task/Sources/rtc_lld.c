#include "rtc_lld.h"
#include "printf.h"

rtc_timedate_t current_time;

void rtc_lld_init(void)
{
    /* Initialize RTC instance
     *  - See RTC configuration component for options
     */
    RTC_DRV_Init(RTCTIMER1, &rtcTimer1_Config0);
    /* Configure RTC Time Seconds Interrupt */
    RTC_DRV_ConfigureSecondsInt(RTCTIMER1, &rtcTimer1_SecIntConfig0);
    /* Set the time and date */
    RTC_DRV_SetTimeDate(RTCTIMER1, &rtcTimer1_StartTime0);

    /* Start the RTC counter */
    RTC_DRV_StartCounter(RTCTIMER1);
}

void rtc_lld_step(void)
{
    static uint8_t i = 0U;
    /* Get current time */
    RTC_DRV_GetCurrentTimeDate(RTCTIMER1, &current_time);
    printf("%d/%d/%d/%d:%d:%d\n", current_time.year, current_time.month, current_time.day, \
           current_time.hour, current_time.minutes, current_time.seconds);
    i++;
    if(i == 10U)
    {
        printf("set time to 2008/8/8/8:8:8\n");
        current_time.year = 2008;
        current_time.month = 8;
        current_time.day = 8;
        current_time.hour = 8;
        current_time.minutes = 8;
        current_time.seconds = 8;
        RTC_DRV_StopCounter(RTCTIMER1);
        RTC_DRV_SetTimeDate(RTCTIMER1, &current_time);
        RTC_DRV_StartCounter(RTCTIMER1);
        i = 0U;
    }
    else
    {
        /* no code */
    }
}
