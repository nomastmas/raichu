#ifndef TIMER_HPP_
#define TIMER_HPP_

#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/timers.h"

/// Typedef to clarify the time in milliseconds
typedef portTickType TIME_MS;

/// Enumeration to indicate timer type
typedef enum { TimerOneShot, TimerPeriodic } TimerType;

/**
 * Timer class to attach C-Functions to timers, but use C++ class
 * structure to easily control the timers.
 *
 * This is C++ wrapper around FreeRTOS timers.
 */
class Timer
{
    public:
	 	/**
	 	 * Constructor to create the timer.
	 	 * @param pFunction The pointer to C-Function
	 	 * @param t			The timer expiration in milliseconds
	 	 */
        Timer(tmrTIMER_CALLBACK pFunction, TIME_MS t, TimerType type);
        ~Timer(); ///< Destructor to delete the timer

        void start();					///< Starts the timer, and calls reset() if timer is already started.
        void stop();					///< Stops the timer
        void reset();					///< Resets(restarts) the timer
        void changePeriod(TIME_MS t);	///< Changes the timer's time
        bool isRunning();				///< @returns TRUE if the timer is active

        /**
         * @{ \name Timer functions to be used from within an ISR.
         * These will automatically call FreeRTOS YIELD function if required.
         */
        void startFromISR();					///< Restarts the timer from an ISR
        void stopFromISR();						///< Stops the timer from an ISR
        void resetFromISR();					///< Resets the timer from an ISR
        void changePeriodFromISR(TIME_MS t);    ///< Changes the timer's time from an ISR
        /** @} */

        /// @returns The FreeRTOS Timer Handle for this timer.
        inline xTimerHandle getTimerHandle() { return mTimerHandle; }

    private:
        xTimerHandle mTimerHandle;
};

#endif /* TIMER_HPP_ */
