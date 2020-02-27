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
    /* Get current time */
    RTC_DRV_GetCurrentTimeDate(RTCTIMER1, &current_time);
    printf("%d/%d/%d/%d:%d:%d\n", current_time.year, current_time.month, current_time.day, \
           current_time.hour, current_time.minutes, current_time.seconds);
}
