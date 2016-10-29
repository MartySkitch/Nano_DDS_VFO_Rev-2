/*
 *
 *
 *    FollowPWM.h
 *
 *    PWM follower, that adjusts a PWM output according to an A/D input.
 *
 *    License: GNU General Public License Version 3.0.
 *    
 *    Copyright (C) 2014 by Matthew K. Roberts, KK5JY. All rights reserved.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *    
 *    This program is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *    
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see: http://www.gnu.org/licenses/
 *
 *
 */

#ifndef __FOLLOW_PWM_H
#define __FOLLOW_PWM_H

#include "Arduino.h"

//
//  A PWM follower class - adjusts a PWM output according to an A/D
//       input, probably connected to a high-Z potentiometer.
//
class FollowPWM {
	private:
		const byte m_Output, m_Input;
		const uint16_t MaxAD;
		const uint16_t MaxPWM;
		const uint16_t Divisor;
		byte m_LastPWM;

	public:
		//
		//  ctor - specify the output (PWM) pin, and the input (A/D) pin
		//
		FollowPWM(byte output, byte input) :
			m_Output(output), m_Input(input), MaxAD(1023), MaxPWM(255), Divisor(MaxAD / MaxPWM) {
			m_LastPWM = 0;
			pinMode(output, OUTPUT);
			digitalWrite(output, m_LastPWM);
			pinMode(input, INPUT_PULLUP);
		}

		//
		//  Process() - update the PWM value by reading the A/D
		//
		void Process() {
			uint16_t ad = analogRead(m_Input);
			ad /= Divisor;
			if (m_LastPWM != ad) {
				m_LastPWM = ad;
				analogWrite(m_Output, ad);
			}
		}
};

#endif // __FOLLOW_PWM_H
