#include "../timer.hpp"

Timer::Timer(tmrTIMER_CALLBACK pFunction, TIME_MS t, TimerType type)
{
	// xTimerCreate() copies the pointer for the name, so the tName variable
	// cannot simply be on the stack, it should be global or static
	static signed char tName[] = "Tmr";

	mTimerHandle = xTimerCreate(tName, OS_MS(t),
								type == TimerOneShot ? pdFALSE : pdTRUE,
								0,
								pFunction);
}
Timer::~Timer()
{
	xTimerDelete(mTimerHandle, 0);
}

void Timer::start()
{
	xTimerStart(mTimerHandle, 0);
}
void Timer::stop()
{
	xTimerStop(mTimerHandle, 0);
}
void Timer::reset()
{
	xTimerReset(mTimerHandle, 0);
}
void Timer::changePeriod(TIME_MS t)
{
	xTimerChangePeriod(mTimerHandle, t, 0);
}
bool Timer::isRunning()
{
	return (pdFALSE != xTimerIsTimerActive(mTimerHandle) ? true : false);
}

#define callTimerFunctionFromIsr(f) \
		portBASE_TYPE higherPrTaskWoken = 0;	\
		f(mTimerHandle, &higherPrTaskWoken);	\
		if(higherPrTaskWoken)					\
			portYIELD_FROM_ISR()

void Timer::startFromISR()
{
	callTimerFunctionFromIsr(xTimerStartFromISR);
}
void Timer::stopFromISR()
{
	callTimerFunctionFromIsr(xTimerStopFromISR);
}
void Timer::resetFromISR()
{
	callTimerFunctionFromIsr(xTimerResetFromISR);
}
void Timer::changePeriodFromISR(TIME_MS t)
{
	portBASE_TYPE higherPrTaskWoken = 0;
	xTimerChangePeriodFromISR(mTimerHandle, t, &higherPrTaskWoken);
	if( higherPrTaskWoken ) {
		portYIELD_FROM_ISR();
	}
}
