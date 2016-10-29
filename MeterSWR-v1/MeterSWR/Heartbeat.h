/*
 *
 *
 *    Heartbeat.h
 *
 *    License: GNU General Public License Version 3.0.
 *    
 *    Copyright (C) 2015-2016 by Matthew K. Roberts, KK5JY.
 *    All rights reserved.
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

#ifndef __HEARTBEAT_LED_H
	#ifdef HEARTBEAT_LED
		#if HEARTBEAT_LED > 0
			// new static globals
			static unsigned long lastHeartbeat = 0UL;	// heartbeat LED flash timer
			static bool heartbeatState = false;			// the LED state (so we don't have to do I/O to read it)

			// import the elapsed-time math
			#include "Elapsed.h"

			//
			//  SETUP - initializes the output pin
			//
			#define HeartbeatSetup() { pinMode(HEARTBEAT_LED, OUTPUT); }

			//
			//  LOOP - the per-loop call, which does the timing and I/O
			//
			#define HeartbeatLoop() { \
				if ((!heartbeatState) && (Elapsed(now, lastHeartbeat) >= 900)) { \
					lastHeartbeat = now; \
					digitalWrite(HEARTBEAT_LED, HIGH); \
					heartbeatState = true; \
				} else if (heartbeatState && (Elapsed(now, lastHeartbeat) >= 100)) { \
					digitalWrite(HEARTBEAT_LED, LOW); \
					heartbeatState = false; \
				} \
			}
		#else
			// empty macros if no pin assigned
			#define HeartbeatSetup() { }
			#define HeartbeatLoop() { }
		#endif
	#else
		// empty macros if HEARTBEAT_LED not defined
		#define HeartbeatSetup() { }
		#define HeartbeatLoop() { }
	#endif
#endif // __HEARTBEAT_LED_H

