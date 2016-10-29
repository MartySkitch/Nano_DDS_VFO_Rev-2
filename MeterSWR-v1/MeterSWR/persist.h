/*
 *
 *
 *    persist.h
 *
 *    SWR Meter Firmware for Arduino
 *    Version 1.0
 *
 *    User settings persistence support (EEPROM support).
 *
 *    Note: this file is a simple refactoring of the main INO file, and
 *          cannot be built outside of a #include from that file.
 *
 *    License: GNU General Public License Version 3.0.
 *    
 *    Copyright (C) 2014-2016 by Matthew K. Roberts, KK5JY.
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

#ifndef __METERSWR_PERSIST_H
#define __METERSWR_PERSIST_H

#include "EEPROM.h"

// define the unique string for EEPROM detection
#define EEPROM_KEY ("METERSWR-01.00")

//
//  Binary DTO for meter settings
//
struct MeterBlock {
	float AlphaForward;
	float AlphaReflected;
};

//
//  write object to EEPROM
//
inline int write_eeprom(int pos, void *obj, int size) {
	char *cp = (char*)(obj);
	for (uint8_t i = 0; i != size; ++i) {
		EEPROM.write(pos + i, *cp++);
	}
	return pos + size;
}

//
//  read object from EEPROM
//
inline int read_eeprom(int pos, void *obj, int size) {
	char *cp = (char*)(obj);
	for (uint8_t i = 0; i != size; ++i) {
		*cp++ = EEPROM.read(pos + i);
	}
	return pos + size;
}

//
//  clear the EEPROM
//
inline void clear_eeprom() {
	for(uint16_t i = 0; i != E2END + 1; ++i) {
		EEPROM.write(i, 0);
	}
}

//
//  store EEPROM state
//
inline void save_eeprom() {
	// EEPROM pointer
	uint16_t pos = 0;

	// write out header
	{
		strcpy_P(ioBuffer, PSTR(EEPROM_KEY));
		uint8_t count = strlen(ioBuffer);
		pos = write_eeprom(pos, ioBuffer, count);
#ifdef DEBUG
		HostSerial.print(strcpy_P(ioBuffer, PSTR("#debug,header key length ")));
		HostSerial.print((int)(count));
		HostSerial.println();
#endif
	}

	// write state block
	{
		// copy
		MeterBlock mb;
		mb.AlphaForward = swr.AlphaForward();
		mb.AlphaReflected = swr.AlphaReflected();

		// write
		pos = write_eeprom(pos, &mb, sizeof(MeterBlock));
	}
}

//
//  restore EEPROM state
//
inline void load_eeprom(bool quiet = false) {
	uint16_t pos = 0;
	// read the header
	{
		char *header = strcpy_P(ioBuffer, PSTR(EEPROM_KEY));
		uint8_t count = strlen(header);
		for (uint8_t i = 0; i != count; ++i) {
			if (EEPROM.read(i) != ioBuffer[i]) {
#ifdef DEBUG
				HostSerial.print(strcpy_P(ioBuffer, PSTR("#debug,header mismatch @ ")));
				HostSerial.print((int)(i));
				HostSerial.print(strcpy_P(ioBuffer, PSTR(" contents: ")));
				for (uint8_t j = 0; j != count; ++j) {
					HostSerial.print((char)(EEPROM.read(j)));
				}
				HostSerial.println();
#endif
				// can't find the config block, give up
				if (!quiet)
					HostSerial.println(strcpy_P(ioBuffer, PSTR("#error,EEPROM header not found")));
				return;
			}
		}
		pos = count;
	}
	// read the state block
	{
		MeterBlock mb;
		pos = read_eeprom(pos, &mb, sizeof(MeterBlock));
		swr.AlphaForward(mb.AlphaForward); // this method will range-check
		swr.AlphaReflected(mb.AlphaReflected); // this method will range-check
	}
}

#endif
