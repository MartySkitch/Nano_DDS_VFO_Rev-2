/*
 *
 *
 *    SegmentSWR.h
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

#ifndef __SEGMENT_SWR_H
#define __SEGMENT_SWR_H

#include "Arduino.h"
#include <SoftwareSerial.h>

//
//  a 7-segment serial display
//
class SegmentSWR {
	private:
		SoftwareSerial m_Port;
		char buffer[5];
		
	public:
		SegmentSWR(const uint8_t rxpin, const uint8_t txpin) : m_Port(rxpin, txpin) {
			// nop
		}
		
		void Initialize() {
			m_Port.begin(9600);
			m_Port.print('\x76');
			delay(100);
			m_Port.print('\x76');
			m_Port.print('\x7A');
			m_Port.print('\xFF');
		}
		
		void Update(float value) {
			m_Port.print('\x77');
			m_Port.print('\x02');
			m_Port.print('\x79');
			m_Port.print('\x00');
			snprintf(buffer, sizeof(buffer), "%4d", (int)(value * 100));
			m_Port.print(buffer);
		}
};

#endif
