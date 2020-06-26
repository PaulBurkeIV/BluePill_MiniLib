/*
 * ProfPWM.c
 * Profiled PWM adds transition rate control to the PWM function.
 * Where PWM is used as a position control, for example controlling servo position,
 * the rate corresponds to speed.
 * Where the PWM is controlling speed, as in a PWM motor drive, the rate
 * corresonds to acceleration.
 */

#ifdef USE_PROFPWM

#include "Platform.h"
#include "AppIODefs.h"
#include "Globals.h"
#include "ProfiledPWM.h"

/*
Each channel corresponds with a structure t_ProfPWMChannel defining:
The timer (TIMx) providing the PWM timebase (TIM1, TIM2, TIM3 or TIM4)
The timer channel on TIMx (1, 2, 3 or 4)
The maximum PWM value
The minimum PWM value
The current position value
The position setpoint
The rate at which the PWM changes from current to setpoint value
A pointer to an optional (possibly non- linear) transitional function

An array of N_PROFPWMS structures is created, which is processed by calling ProfilePWM().
This can be called from SysTickHook(), the main loop, or elswhere if you wish.
*/


t_ProfPWMChannel ProfPWM[N_PROFPWMS];

/*
Timebase initialisation:
++++++++++++++++++++++++
void ProfPWM_BaseInit(t_PWMBase PWM, uint32_t Freq, uint32_t Resolution)

PWM is an STM32 timer with PWM function. The Blue Pill has 4 such timers, TIM1, TIM2, TIM3 and TIM4,
giving a total of 14 channels (TIM4 has only 2 channels available).
Freq is the output frequency in Hz, and Resolution is the PWM resolution in counts.
The actual output frequency will depend on the combination of clock speed
(72MHz or 36MHz for the Blue Pill), Freq and Resolution.
For example, 72MHz/ 50Hz /1000 counts is 1440, so the frequency will be 50Hz
within the crystal accuracy.
On the other hand, 20kHz and 1024 count resolution will result in a divider of 3.75,
which will be truncated to 3, resulting in an output frequency of 23.4kHz.
*/

void ProfPWM_BaseInit(t_PWMBase PWM, uint32_t Freq, uint32_t Resolution)
{
InitPWMTimebase(PWM, Freq, Resolution);
}

/*
Pin initialisation:
+++++++++++++++++++
void ProfPWM_PinInit(GPIO_TypeDef* Port, uint8_t Pin, uint8_t Mode, bool OType, uint8_t Pull)

This is just a wrapper for the standard IO pin initialisation InitIOBit().
It's up to you that you initialise the right pins for your timer!
*/

void ProfPWM_PinInit(GPIO_TypeDef* Port, uint8_t Pin, uint8_t Mode, bool OType, uint8_t Pull)
{
InitIOBit(Port, Pin, Mode,OType, Pull);
}

/*
Channel initialisation:
+++++++++++++++++++++++
void ProfPWM_ChannelInit(uint8_t Channel, t_PWMBase PWM, uint16_t PhysChann, uint16_t Min, uint16_t Max, uint16_t InitVal, bool Inv)

Channel is the entry in the array t_ProfPWMChannel structures (0 to N_PROFPWMS-1)
PWM is the TIMx for this channel (TIM1 to TIM4)
PhysChann is the timer compare channel on this TIMx (1 to 4)
Min is the minimum value that the output should take (0 to Resolution)
Max is the maximum value that the output should take (0 to Resolution)
InitVal is the value that the output is initialised to (0 to Resolution)
Inv inverts the output if true. It can be used if the output drives an inverting buffer.

Call this function after timebase initialisation.
*/

void ProfPWM_ChannelInit(uint8_t Channel, t_PWMBase PWM, uint16_t PhysChann, uint16_t Min, uint16_t Max, uint16_t InitVal, bool Inv)
{
ProfPWM[Channel].PWM = PWM;
ProfPWM[Channel].PhysChann = PhysChann;
ProfPWM[Channel].Max = Max << PWM_RATE;
ProfPWM[Channel].Min = Min << PWM_RATE;
ProfPWM[Channel].Current = InitVal << PWM_RATE;
InitPWMChannel(PWM, PhysChann, InitVal, Inv);
}

/*
PWM output set
++++++++++++++
void ProfPWM_Set(uint8_t Channel, uint16_t Value, uint32_t Rate)

Channel is the entry in the array t_ProfPWMChannel structures (0 to N_PROFPWMS-1)
Value is the desired final PWM setting after profiled transition
Rate is the rate of change. It is scaled by 2^PWM_RATE (defaut 2^8).
*/

void ProfPWM_Set(uint8_t Channel, uint16_t Value, uint32_t Rate)
{
ProfPWM[Channel].Rate = Rate;
ProfPWM[Channel].Setpoint = Value << PWM_RATE;
}

/*
Install PWM Profile Function
++++++++++++++++++++++++++++
void ProfPWM_Function(uint8_t Channel, uint16_t (*Profile)(uint16_t))

Channel is the entry in the array t_ProfPWMChannel structures (0 to N_PROFPWMS-1)

(*Profile)(uint16_t)) is a pointer to a user function which takes the current value
and returns a modified int16_t.

The default transition profile is linear. If a non- linear profile is desired,
this can be provided using a custom function which receives the current value
and returns a modified value. The function can be a lookup table with or without
interpolation, or a mathematical function, or indeed vary depending on input vaues.
But beware that it is called at every PWM update, so avoid anything that could
cause the program to hang, for example waiting on inputs, very complex mathematical
functions, or excessive looping.

Note also that the profile may have no effect if the rate is faster than whatever
is on the output (e.g. a servo) can physically change.
*/

void ProfPWM_Function(uint8_t Channel, uint16_t (*Profile)(uint16_t))
{
ProfPWM[Channel].Profile = Profile;
}

/*
Read current PWM value
++++++++++++++++++++++
uint16_t ProfPWM_Read( uint8_t Channel)

Returns the current output setting. May be useful to find where you are in a slow profile!
*/

uint16_t ProfPWM_Read( uint8_t Channel)
{
return ProfPWM[Channel].Current >> PWM_RATE;
}

/*
Stop transition
++++++++++++++++
void ProfPWM_Halt(uint8_t Channel, uint16_t Value)

Channel is the entry in the array t_ProfPWMChannel structures (0 to N_PROFPWMS-1)

Sets the PWM output immediately to Value.
e.g.
ProfPWM_Halt(0, ProfPWM_Read(0))
will freeze a servo at its current position.

ProfPWM_Halt(1, 0)
will emergency- stop a motor output.
*/

void ProfPWM_Halt(uint8_t Channel, uint16_t Value)
{
ProfPWM[Channel].Current = ProfPWM[Channel].Setpoint = Value << PWM_RATE;
ProfPWM[Channel].Rate = 0;
}

/*
Update Profiled PWM array
+++++++++++++++++++++++++
void ProfilePWM(void)

This is called periodically to update the profile values. It is typically
called from the SysTickHook() function.
*/

void ProfilePWM(void)
{
uint8_t Channel;
for( Channel = 0; Channel < N_PROFPWMS; Channel++)
 {
 if(  ProfPWM[Channel].Current > ProfPWM[Channel].Setpoint)
  {
  ProfPWM[Channel].Current -= ProfPWM[Channel].Rate;
  if( ProfPWM[Channel].Current < ProfPWM[Channel].Min)
   {
   ProfPWM[Channel].Current = ProfPWM[Channel].Min;
   }
  }
 else if( ProfPWM[Channel].Current < ProfPWM[Channel].Setpoint )
  {
  ProfPWM[Channel].Current += ProfPWM[Channel].Rate;
  if( ProfPWM[Channel].Current > ProfPWM[Channel].Max)
   {
   ProfPWM[Channel].Current = ProfPWM[Channel].Max;
   }
  }
 if(1) //!ProfPWM[Channel].Profile)
  {
  SetPWM(ProfPWM[Channel].PWM, ProfPWM[Channel].PhysChann, ProfPWM[Channel].Current >> PWM_RATE);
  }
 else
  {
  SetPWM(ProfPWM[Channel].PWM, ProfPWM[Channel].PhysChann, ProfPWM[Channel].Profile(ProfPWM[Channel].Current));
  }
 }
}
#endif //USE_PROFPWM
