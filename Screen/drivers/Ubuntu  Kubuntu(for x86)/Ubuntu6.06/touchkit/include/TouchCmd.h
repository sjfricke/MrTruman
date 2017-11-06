/*=========================================================================*/
/* Command Set for Device to Host				           */	
/* The Packet Format Should be as				           */
/* Packet = SOP + CMD( 1 byte ) + DATA ( variable length ) + < CR >        */
/* Note : All of the DATA are in ASCIIZ format	                           */
/* the Packet from Host to Device is defoined as                           */
/* Packet = SOP + CMD( 1 bytes ) + < CR > + follow DATA                    */
/* Neverthless, the packet for point report is more special than above is  */
/* is defined as                                                           */
/* Packet = Sync( 1 byte ) + X1X2( 2 bytes ) + Y1Y2 ( 2 bytes )            */
/* Where ,                                                                 */ 
/*              Sync = 1 R R R R R R S                                     */
/*              S : Status for the point. 1 = TOUCH DOWN                   */ 
/*                                        0 = LIFT OFF                     */ 
/*              R : Reserved bits must be 0 for Frank                      */
/*              							   */
/*              X1: MSB of X axis  = RRRRXXXX                              */ 
/*              X2: LSB of X axis  = RXXXXXXX                              */
/*              Y1: MSB of Y axis  = RRRRYYYY                              */
/*              Y2: LSB of Y axis  = RYYYYYYY                              */
/*                                                                         */
/* File Name : TouchCmd.h                                                  */
/* Author    : Alex Chang                                                  */
/* Copyright (c) 2000 - 2001  eGalax Inc.		    	           */
/* All rights reserved.                                                    */
/*=========================================================================*/


#ifndef _TOUCHCMD_H
#define _TOUCHCMD_H

#define _SOP					0x0A
#define _CR					0
#define _SYNCBIT				0x80

#define _DOWN				0x01
#define POINT_TOUCH			0x81
#define POINT_NOTOUCH       0x80

#define CMD_ID              0x96   // ID for Weltrend case!
#define CHECK_SUM_BASE	0x55
#define D2H_BASE		    'A'
#define D2H_BASE1           'a'
#define D2H_BASE2           '0'

#define D2H_LOOPBACK		( D2H_BASE + 0 )
#define D2H_EEPROM		    ( D2H_BASE + 2 )
#define D2H_VERSION		    ( D2H_BASE + 3 )
#define D2H_MODEL		    ( D2H_BASE + 4 )
#define D2H_TYPE		    ( D2H_BASE + 5 )
#define D2H_THRESHOLD		( D2H_BASE + 7 )
#define D2H_ACC			    ( D2H_BASE + 9 )
#define D2H_GETDEBOUNCE		( D2H_BASE + 11 )
//#define D2H_THRESHOLD       ( D2H_BASE + 12 )
#define D2H_GAINCALDONE     ( D2H_BASE + 13 ) // Gain Calibration
#define D2H_HW4PTCALDONE    ( D2H_BASE + 14 ) // HW 4 points calibration
#define D2H_STRAYDONE       ( D2H_BASE + 15 )

#define D2H_ADVALUE         ( D2H_BASE + 16 ) // Raw AD Value
#define D2H_AGCLIMIT        ( D2H_BASE + 17 )

#define D2H_SUBAD           ( H2D_BASE + 18 ) // Report Raw AD - Stray AD
#define D2H_XYSUM           ( H2D_BASE + 19 ) // Report XY and SUM

#define D2H_STRAYTHRESHOLDDONE  ( D2H_BASE + 20 ) // Threshold Validation Done
#define D2H_SIGNAL              ( D2H_BASE + 21 ) // Singal Validation Done
#define D2H_SENSITIVITY         ( D2H_BASE + 22 ) // Sensitivity Validation done 
#define D2H_SETEEPROMOK			( D2H_BASE + 23 ) // Ack of H2D_SETEEPROM; format [SOP][2][CMD][Addr]
#define D2H_EEPROMFIXED			( D2H_BASE + 24 ) // Used in Avoid-Ctrl-Char Mode (format: [SOP][5][CMD][Addr][0:5][6:11][12:15])

/*=========================================================================*/
/*=========================================================================*/
#define H2D_BASE		        'A'
#define H2D_BASE1               'a'
#define H2D_BASE2               '0'

#define H2D_LOOPBACK	        ( H2D_BASE + 0 )
#define H2D_SETEEPROM	        ( H2D_BASE + 1 )
#define H2D_GETEEPROM	        ( H2D_BASE + 2 )
#define H2D_GETVERSION		    ( H2D_BASE + 3 )
#define H2D_GETMODEL		    ( H2D_BASE + 4 )
#define H2D_GETTYPE		        ( H2D_BASE + 5 )
#define H2D_SET_SETTHRESHOLD	( H2D_BASE + 6 )
#define H2D_GET_GETTHRESHOLD	( H2D_BASE + 7 )
#define H2D_SET_ACC		        ( H2D_BASE + 8 )
#define H2D_GET_ACC		        ( H2D_BASE + 9 )
#define H2D_SET_DEBOUNCE	    ( H2D_BASE + 10 )
#define H2D_GET_DEBOUNCE	    ( H2D_BASE + 11 )
#define H2D_GET_THRESHOLD       ( H2D_BASE + 12 )
#define H2D_SET_THRESHOLD       ( H2D_BASE + 13 )
#define H2D_GAINCAL             ( H2D_BASE + 14 )
#define H2D_HW4PTCAL            ( H2D_BASE + 15 )
#define H2D_STRAY               ( H2D_BASE + 16 )

#define H2D_FIXGAIN             ( H2D_BASE + 17 )   // Fix Gain
#define H2D_REPORTAD            ( H2D_BASE + 18 )   // Report raw AD Value

#define H2D_SETAGCLIMIT         ( H2D_BASE + 19 )   // first WORD: AGC LOW
#define H2D_GETAGCLIMIT         ( H2D_BASE + 20 )   // 2nd WORD : AGC HIGH

//#define H2D_REPORTSUBAD       ( H2D_BASE + 21 )   // Report Raw AD - Stray AD
#define H2D_REPORTXYSUM         ( H2D_BASE + 22 )   // Report XY and SUM
#define H2D_STRAYTHRESHOLD      ( H2D_BASE + 23 )   // Cmd to Learn the Stray Threshold
#define H2D_STRAYUPDATECOUNTER  ( H2D_BASE + 24 )
#define H2D_CNTER               ( H2D_BASE + 25 )   // Skip Counter, Auto Update Counter

#define H2D_SETCHANEL			( H2D_BASE1 + 1 )   // Channel Always On!!!
#define H2D_VALIDATE_SIGNAL     ( H2D_BASE1 + 2 )   // Validate the Gain Level
#define H2D_VALIDATE_SENSITIVITY ( H2D_BASE1 + 3 )  // Validate the Sensitivity
#define H2D_SWITCHFILTER        ( H2D_BASE1 + 4 )  // Format: [SOP][2][Cmd][zero for OFF]
#define H2D_AVOIDCTRLCHAR		( H2D_BASE1 + 5 ) // (for Linux, PS/2 only) 
#define H2D_FIRMWARERESET		( H2D_BASE1 + 6 ) // just sync parameters with EEPROM in fact


#endif
