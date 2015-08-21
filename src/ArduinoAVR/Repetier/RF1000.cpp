#if (ARDUINO >= 100)
 #include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
#else
 #include <WProgram.h>
#endif

//#include <avr/pgmspace.h>
#include "Repetier.h"
#include <Wire.h>


char			g_nMainDirectionX = 0;
char			g_nMainDirectionY = 0;
char			g_nMainDirectionZ = 0;
char			g_nBlockZ		  = 0;
char			g_nMainDirectionE = 0;
unsigned long	g_lastTime		  = 0;

#if FEATURE_HEAT_BED_Z_COMPENSATION
long			g_offsetZCompensationSteps = 0;
long			g_minZCompensationSteps	   = HEAT_BED_Z_COMPENSATION_MIN_STEPS;
long			g_maxZCompensationSteps	   = HEAT_BED_Z_COMPENSATION_MAX_STEPS;
long			g_diffZCompensationSteps   = HEAT_BED_Z_COMPENSATION_MAX_STEPS - HEAT_BED_Z_COMPENSATION_MIN_STEPS;
char			g_nHeatBedScanStatus	   = 0;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
char			g_nWorkPartScanStatus		= 0;
char			g_nWorkPartScanMode			= 0;
char			g_nActiveWorkPart			= 1;
#endif // FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
unsigned long	g_lastScanTime		  = 0;
unsigned long	g_scanStartTime		  = 0;
char			g_scanRetries		  = 0;
char			g_retryZScan		  = 0;

char			g_abortZScan		  = 0;
short			g_ZCompensationMatrix[COMPENSATION_MATRIX_MAX_X][COMPENSATION_MATRIX_MAX_Y];
unsigned char	g_uZMatrixMaxX		  = 0;
unsigned char	g_uZMatrixMaxY		  = 0;
long			g_nZScanZPosition	  = 0;
long			g_nLastZScanZPosition = 0;

short			g_nMaxPressureContact;
short			g_nMaxPressureRetry;
short			g_nMaxPressureIdle;
short			g_nMinPressureContact;
short			g_nMinPressureRetry;
short			g_nMinPressureIdle;
short			g_nFirstIdlePressure;
short			g_nCurrentIdlePressure;

char			g_nTempDirectionZ			 = 0;	// this is the current z-direction during operations like the bed scan or finding of the z-origin

// configurable scan parameters - the proper default values are set by restoreDefaultScanParameters()
long			g_nScanXStartSteps			 = 0;
long			g_nScanXStepSizeMm			 = 0;
long			g_nScanXStepSizeSteps		 = 0;
long			g_nScanXEndSteps			 = 0;
long			g_nScanXMaxPositionSteps	 = 0;
long			g_nScanYStartSteps			 = 0;
long			g_nScanYStepSizeMm			 = 0;
long			g_nScanYStepSizeSteps		 = 0;
long			g_nScanYEndSteps			 = 0;
long			g_nScanYMaxPositionSteps	 = 0;
short			g_nScanHeatBedUpFastSteps	 = 0;
short			g_nScanHeatBedUpSlowSteps	 = 0;
short			g_nScanHeatBedDownFastSteps	 = 0;
short			g_nScanHeatBedDownSlowSteps	 = 0;
long			g_nScanZMaxCompensationSteps = 0;
unsigned short	g_nScanFastStepDelay		 = 0;
unsigned short	g_nScanSlowStepDelay		 = 0;
unsigned short	g_nScanIdleDelay			 = 0;
unsigned short	g_nScanContactPressureDelta	 = 0;
unsigned short	g_nScanRetryPressureDelta	 = 0;
unsigned short	g_nScanIdlePressureDelta	 = 0;
short			g_nScanIdlePressureMin		 = 0;
short			g_nScanIdlePressureMax		 = 0;
char			g_nScanPressureReads		 = 0;
unsigned short	g_nScanPressureReadDelay	 = 0;
short			g_nScanPressureTolerance	 = 0;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

#if REMEMBER_PRESSURE
short			g_HeatBedPressure[COMPENSATION_MATRIX_MAX_X][COMPENSATION_MATRIX_MAX_Y];
#endif // REMEMBER_PRESSURE

long			g_staticZSteps				= 0;
char			g_debugLevel				= 0;
char			g_debugLog					= 0;
//short			g_debugCounter[10]			= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned long	g_uStopTime					= 0;
unsigned long	g_uBlockCommands			= 0;
short			g_debugInt16				= 0;
unsigned short	g_debugUInt16				= 0;
long			g_debugInt32				= 0;

// other configurable parameters
unsigned long	g_nManualZSteps				= DEFAULT_MANUAL_Z_STEPS;
unsigned long	g_nManualExtruderSteps		= DEFAULT_MANUAL_EXTRUDER_STEPS;

#if FEATURE_PAUSE_PRINTING
long			g_nPauseStepsX				= DEFAULT_PAUSE_STEPS_X;
long			g_nPauseStepsY				= DEFAULT_PAUSE_STEPS_Y;
long			g_nPauseStepsZ				= DEFAULT_PAUSE_STEPS_Z;
long			g_nPauseStepsExtruder		= DEFAULT_PAUSE_STEPS_EXTRUDER;
long			g_nContinueStepsX			= 0;
long			g_nContinueStepsY			= 0;
long			g_nContinueStepsZ			= 0;
long			g_nContinueStepsExtruder	= 0;
char			g_preparePause				= 0;
char			g_pausePrint				= 0;
char			g_printingPaused			= 0;
unsigned long	g_uPauseTime				= 0;
char			g_pauseBeepDone				= 0;
#endif // FEATURE_PAUSE_PRINTING

#if FEATURE_PARK
long			g_nParkPositionX			= PARK_POSITION_X;
long			g_nParkPositionY			= PARK_POSITION_Y;
long			g_nParkPositionZ			= PARK_POSITION_Z;
#endif // FEATURE_PARK

#if FEATURE_EMERGENCY_PAUSE
unsigned long	g_uLastPressureTime			= 0;
long			g_nPressureSum				= 0;
char			g_nPressureChecks			= 0;
#endif // FEATURE_EMERGENCY_PAUSE

#if FEATURE_EMERGENCY_Z_STOP
unsigned long	g_uLastZPressureTime		= 0;
long			g_nZPressureSum				= 0;
char			g_nZPressureChecks			= 0;
#endif // FEATURE_EMERGENCY_Z_STOP

#if FEATURE_FIND_Z_ORIGIN
char			g_nFindZOriginStatus		= 0;
long			g_nZOriginXPosition			= 0;
long			g_nZOriginYPosition			= 0;
long			g_nZOriginZPosition			= 0;
char			g_abortSearch				= 0;
#endif // FEATURE_FIND_Z_ORIGIN

#if DEBUG_HEAT_BED_Z_COMPENSATION
long			g_nNonCompensatedPositionSteps[3] = { 0, 0, 0 };
long			g_nTargetCompensationZ			  = 0;
long			g_nZCompensationUpdates			  = 0;
long			g_nDelta[2]						  = { 0, 0 };
long			g_nStepSize[2]					  = { 0, 0 };
long			g_nTempXFront					  = 0;
long			g_nTempXBack					  = 0;
long			g_nNeededZ						  = 0;
unsigned char	g_uIndex[4]						  = { 0, 0, 0, 0 };
short			g_nMatrix[4]					  = { 0, 0, 0, 0 };
long			g_nZDeltaMin					  = 100000;
long			g_nZDeltaMax					  = -100000;
long			g_nZCompensationUpdateTime		  = 0;
long			g_nZCompensationDelayMax		  = 0;
long			g_nTooFast						  = 0;
#endif // DEBUG_HEAT_BED_Z_COMPENSATION


void initRF1000( void )
{
	// initialize the two strain gauges
	Wire.begin();
	initStrainGauge();

#if FEATURE_CNC_MODE > 0
	switchOperatingMode( Printer::operatingMode );

	if( Printer::ZEndstopType != ENDSTOP_TYPE_SINGLE )
	{
		if( Printer::isZMinEndstopHit() || Printer::isZMaxEndstopHit() )
		{
			// a z-endstop is active at the moment of the startup of the firmware, but both z-endstops are within one circuit so we do not know which one is the pressed one
			// in this situation we do not allow any moving into z-direction before a z-homing has been performed
			Printer::ZEndstopUnknown = 1;
		}
	}
#else
	setupForPrinting();
#endif // FEATURE_CNC_MODE > 0

	return;

} // initRF1000


void initStrainGauge( void )
{
#if MOTHERBOARD == 12
	// configure DMS #1 (0x8C = single mode, 16 bits, gain = *1)
	Wire.beginTransmission( I2C_ADDRESS_STRAIN_GAUGE_1 );
	Wire.write( 0x8C );
	Wire.endTransmission();

	// configure DMS #2 (0x8C = single mode, 16 bits, gain = *1)
	Wire.beginTransmission( I2C_ADDRESS_STRAIN_GAUGE_2 );
	Wire.write( 0x8C );
	Wire.endTransmission();
#endif // MOTHERBOARD == 12

#if MOTHERBOARD == 13
	// configure DMS #1 (0x8C = single mode, 16 bits, gain = *1)
	Wire.beginTransmission( I2C_ADDRESS_STRAIN_GAUGE );
	Wire.write( 0x8C );
	Wire.endTransmission();
#endif // MOTHERBOARD == 13

	return;

} // initStrainGauge


short readStrainGauge( unsigned char uAddress )
{
	unsigned char	Register;
	short			Result;


	Wire.beginTransmission( uAddress );
	Wire.requestFrom( (uint8_t)uAddress, (uint8_t)3 );
        
	Result =  Wire.read();
    Result =  Result << 8;
	Result += Wire.read();
        
	Register = Wire.read();
	Wire.endTransmission();

	return Result;

} // readStrainGauge


#if FEATURE_HEAT_BED_Z_COMPENSATION
void startHeatBedScan( void )
{
	if( g_nHeatBedScanStatus )
	{
		// abort the heat bed scan
		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "startHeatBedScan(): the scan has been cancelled" ) );
		}
		g_abortZScan = 1;
	}
	else
	{
		if( PrintLine::linesCount )
		{
			// there is some printing in progress at the moment - do not start the heat bed scan in this case
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "startHeatBedScan(): the scan can not be started while the printing is in progress" ) );
			}
		}
		else
		{
			// start the heat bed scan
			g_nHeatBedScanStatus = 1;
			BEEP_START_HEAT_BED_SCAN

			// when the heat bed is scanned, the z-compensation must be disabled
			if( Printer::doHeatBedZCompensation )
			{
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "startHeatBedScan(): the z compensation has been disabled" ) );
				}
				resetZCompensation();
			}
		}
	}

	return;

} // startHeatBedScan


void scanHeatBed( void )
{
	static unsigned char	nIndexX;
	static unsigned char	nIndexY;
	static char				nIndexYDirection;
	static char				nRetry;
	static long				nX;
	static long				nY;
	static long				nZ;
	static long				nYDirection;
	static short			nContactPressure;
	char					nLastHeatBedScanStatus = g_nHeatBedScanStatus;
	short					nTempPressure;
	long					nTempPosition;


	// directions:
	// +x = to the right
	// -x = to the left
	// +y = heat bed moves to the front
	// -y = heat bed moves to the back
	// +z = heat bed moves down
	// -z = heat bed moves up

	if( g_abortZScan )
	{
		// the scan has been aborted
		g_abortZScan = 0;

		// avoid to crash the extruder against the heat bed during the following homing
		g_nZScanZPosition += moveZ( ZAXIS_STEPS_PER_MM *5 );

		// start at the home position
		Printer::homeAxis( true, true, true );

		// turn off the engines
		Printer::disableXStepper();
		Printer::disableYStepper();
		Printer::disableZStepper();

		// disable all heaters
		Extruder::setHeatedBedTemperature( 0, false );
		Extruder::setTemperatureForExtruder( 0, 0, false );

		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "scanHeatBed(): the scan has been aborted" ) );
		}

		UI_STATUS_UPD( UI_TEXT_HEAT_BED_SCAN_ABORTED );
		BEEP_ABORT_HEAT_BED_SCAN

		// restore the compensation values from the EEPROM
		if( loadCompensationMatrix( EEPROM_SECTOR_SIZE ) )
		{
			// there is no valid compensation matrix available
			initCompensationMatrix();
		}

		g_nHeatBedScanStatus  = 0;
		g_nZScanZPosition	  = 0;
		g_nLastZScanZPosition = 0;
		return;
	}

	// show that we are active
	previousMillisCmd = HAL::timeInMilliseconds();

	if( g_nHeatBedScanStatus )
	{
		UI_STATUS( UI_TEXT_HEAT_BED_SCAN );

		if( g_retryZScan )
		{
			// we have to retry to scan the current position
			g_nHeatBedScanStatus = 45;
			g_retryZScan		 = 0;
		}

		switch( g_nHeatBedScanStatus )
		{
			case 1:
			{
				g_scanStartTime    = HAL::timeInMilliseconds();
				g_abortZScan	   = 0;
				nContactPressure   = 0;
				g_nTempDirectionZ  = 0;

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanHeatBed(): the scan has been started" ) );
				}

				// clear all fields of the heat bed compensation matrix
				initCompensationMatrix();

				g_uZMatrixMaxX = 0;
				g_uZMatrixMaxY = 0;

				// output the currently used scan parameters
				outputScanParameters();

				g_nHeatBedScanStatus = 10;
				break;
			}
			case 10:
			{
				// start at the home position
				Printer::homeAxis( true, true, true );

				g_nHeatBedScanStatus = 15;
				g_lastScanTime		 = HAL::timeInMilliseconds();
				break;
			}
			case 15:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < HEAT_BED_SCAN_DELAY )
				{
					// do not check too often
					break;
				}

				if( testExtruderTemperature() )
				{
					// we did not reach the proper temperature
					g_lastScanTime = HAL::timeInMilliseconds();
					break;
				}
				g_nHeatBedScanStatus = 20;
				g_lastScanTime		 = HAL::timeInMilliseconds();
				break;
			}
			case 20:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < HEAT_BED_SCAN_DELAY )
				{
					// do not check too often
					break;
				}

				if( testHeatBedTemperature() )
				{
					// we did not reach the proper temperature
					g_lastScanTime = HAL::timeInMilliseconds();
					break;
				}
				g_nHeatBedScanStatus = 25;
				g_lastScanTime		 = HAL::timeInMilliseconds();
				break;
			}
			case 25:
			{
				// move to the first position
				Printer::enableZStepper();
				Printer::unsetAllSteppersDisabled();

				PrintLine::moveRelativeDistanceInSteps( g_nScanXStartSteps, 0, 0, 0, MAX_FEEDRATE_X, true, true );
				PrintLine::moveRelativeDistanceInSteps( 0, g_nScanYStartSteps, 0, 0, MAX_FEEDRATE_Y, true, true );

				g_nHeatBedScanStatus = 30;
				g_lastScanTime		 = HAL::timeInMilliseconds();
				break;
			}
			case 30:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < HEAT_BED_SCAN_DELAY )
				{
					// do not check too early
					break;
				}

				if( readIdlePressure( &g_nFirstIdlePressure ) )
				{
					// we were unable to determine the idle pressure
					break;
				}

				g_nHeatBedScanStatus = 35;
				g_lastScanTime		 = HAL::timeInMilliseconds();
				break;
			}
			case 35:
			{
				nX				 = g_nScanXStartSteps;
				nY				 = g_nScanYStartSteps;
				nZ				 = 0;
				nYDirection		 = g_nScanYStepSizeSteps;	// we start to move the heat bed from the back to the front
				nIndexYDirection = 1;
				nIndexX			 = 2;
				nIndexY			 = 2;

				g_nMinPressureContact = g_nFirstIdlePressure - g_nScanContactPressureDelta;
				g_nMaxPressureContact = g_nFirstIdlePressure + g_nScanContactPressureDelta;
				g_nMinPressureRetry	  = g_nFirstIdlePressure - g_nScanRetryPressureDelta;
				g_nMaxPressureRetry   = g_nFirstIdlePressure + g_nScanRetryPressureDelta;
				g_nMinPressureIdle	  = g_nFirstIdlePressure - g_nScanIdlePressureDelta;
				g_nMaxPressureIdle	  = g_nFirstIdlePressure + g_nScanIdlePressureDelta;

				// store also the version of this heat bed compensation matrix
#if REMEMBER_PRESSURE
				g_HeatBedPressure[0][0]		= EEPROM_FORMAT;
#endif // REMEMBER_PRESSURE

				g_ZCompensationMatrix[0][0] = EEPROM_FORMAT;

				g_nHeatBedScanStatus = 40;
				break;
			}
			case 39:
			{
				nTempPosition = nX + g_nScanXStepSizeSteps;
				if( nTempPosition > g_nScanXMaxPositionSteps )
				{
					// we end up here when the scan is complete
					g_nHeatBedScanStatus = 60;
					break;
				}

				// move to the next x-position
				PrintLine::moveRelativeDistanceInSteps( g_nScanXStepSizeSteps, 0, 0, 0, MAX_FEEDRATE_X, true, true );
				nX += g_nScanXStepSizeSteps;
				nIndexX ++;

				if( nIndexX > COMPENSATION_MATRIX_MAX_X )
				{
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "scanHeatBed(): the x-dimension of the compensation matrix became too big: " ), nIndexX );
					}
					g_abortZScan = 1;
					break;
				}

				if( nIndexX > g_uZMatrixMaxX )
				{
					g_uZMatrixMaxX = nIndexX;
				}

				if( nYDirection > 0 )
				{
					// we were moving from the front to the back during this column, so we have to move from the back to the front during the next column
					nYDirection		 = -g_nScanYStepSizeSteps;	// we start to move the heat bed from the back to the front
					nIndexYDirection = -1;
				}
				else
				{
					// we were moving from the back to the front during this column, so we have to move from the front to the back during the next column
					nYDirection		 = g_nScanYStepSizeSteps;	// we start to move the heat bed from the back to the front
					nIndexYDirection = 1;
				}

				g_nHeatBedScanStatus = 40;
				break;
			}
			case 40:
			{
				// safety checks
				if( nX <= g_nScanXMaxPositionSteps )
				{
					// remember also the exact x-position of this row/column
#if REMEMBER_PRESSURE
					g_HeatBedPressure[nIndexX][0]	  = nX;
#endif // REMEMBER_PRESSURE

					g_ZCompensationMatrix[nIndexX][0] = (short)((float)nX / XAXIS_STEPS_PER_MM);	// convert to mm

					g_nHeatBedScanStatus = 49;
					g_lastScanTime		 = HAL::timeInMilliseconds();
					break;
				}

				// we end up here when the scan is complete
				g_nHeatBedScanStatus = 60;
				break;
			}
			case 45:
			{
				// home the z-axis in order to find the starting point again
				Printer::homeAxis( false, false, true );

				g_scanRetries		 --;
				g_nZScanZPosition	 = 0;
				nZ					 = 0;
				g_nHeatBedScanStatus = 50;
				g_lastScanTime		 = HAL::timeInMilliseconds();
				break;
			}
			case 49:
			{
				g_scanRetries		 = HEAT_BED_SCAN_RETRIES;
				g_nHeatBedScanStatus = 50;
				break;
			}
			case 50:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < g_nScanIdleDelay )
				{
					// do not check too early
					break;
				}

				// scan this point
				if( testIdlePressure() )
				{
					// the current idle pressure is not plausible
					// break;
				}

				// we should consider that the idle presse can change slightly
				g_nMinPressureContact = g_nCurrentIdlePressure - g_nScanContactPressureDelta;
				g_nMaxPressureContact = g_nCurrentIdlePressure + g_nScanContactPressureDelta;
				g_nMinPressureRetry	  = g_nCurrentIdlePressure - g_nScanRetryPressureDelta;
				g_nMaxPressureRetry   = g_nCurrentIdlePressure + g_nScanRetryPressureDelta;
				g_nMinPressureIdle	  = g_nCurrentIdlePressure - g_nScanIdlePressureDelta;
				g_nMaxPressureIdle	  = g_nCurrentIdlePressure + g_nScanIdlePressureDelta;

				g_nHeatBedScanStatus = 51;
				break;
			}
			case 51:
			{
				// move fast to the surface
				nZ += moveZUpFast();

				g_nHeatBedScanStatus = 52;
				break;
			}
			case 52:
			{
				// move a little bit away from the surface
				nZ += moveZDownSlow();

				g_nHeatBedScanStatus = 53;
				break;
			}
			case 53:
			{
				// move slowly to the surface
				nZ += moveZUpSlow( &nTempPressure, &nRetry );
				nContactPressure = nTempPressure;

				g_nHeatBedScanStatus = 54;
				break;
			}
			case 54:
			{
#if DEBUG_HEAT_BED_SCAN
				if( Printer::debugInfo() )
				{
					Com::printF( PSTR( ";" ), nX );
					Com::printF( PSTR( ";" ), nY );
					Com::printF( PSTR( ";" ), nZ );
					Com::printF( PSTR( ";" ), nContactPressure );

					// output the non compensated position values
					Com::printF( PSTR( ";;" ), Printer::nonCompensatedPositionStepsX );
					Com::printF( PSTR( ";" ), Printer::nonCompensatedPositionStepsY );
					Com::printF( PSTR( ";" ), Printer::nonCompensatedPositionStepsZ );
					Com::printF( PSTR( ";" ), Printer::currentCompensationZ );

					Com::printFLN( PSTR( " " ) );
				}
#endif // DEBUG_HEAT_BED_SCAN

				// remember the z-position and the exact y-position of this row/column
				g_ZCompensationMatrix[nIndexX][nIndexY] = (short)nZ;
				g_ZCompensationMatrix[0][nIndexY]		= (short)((float)nY / YAXIS_STEPS_PER_MM);	// convert to mm

#if REMEMBER_PRESSURE
				// remember the pressure and the exact y-position of this row/column
				g_HeatBedPressure[nIndexX][nIndexY]		= nContactPressure;
				g_HeatBedPressure[0][nIndexY]			= nY;
#endif // REMEMBER_PRESSURE

				g_nHeatBedScanStatus = 55;
				break;
			}
			case 55:
			{
				// move away from the surface
				nZ += moveZDownFast();

				if( nYDirection > 0 )
				{
					nTempPosition = nY+nYDirection;

					if( nTempPosition > g_nScanYMaxPositionSteps )
					{
						// we have reached the end of this column
						g_nHeatBedScanStatus = 39;
						break;
					}
				}
				else
				{
					nTempPosition = nY+nYDirection;

					if( nTempPosition < g_nScanYStartSteps )
					{
						// we have reached the end of this column
						g_nHeatBedScanStatus = 39;
						break;
					}
				}

				// move to the next y-position
				PrintLine::moveRelativeDistanceInSteps( 0, nYDirection, 0, 0, MAX_FEEDRATE_Y, true, true );
				nY		+= nYDirection;
				nIndexY += nIndexYDirection;

				if( nIndexY > COMPENSATION_MATRIX_MAX_Y )
				{
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "scanWorkPart(): the y-dimension of the compensation matrix became too big: " ), nIndexY );
					}
					g_abortZScan = 1;
					break;
				}

				if( nIndexY > g_uZMatrixMaxY )
				{
					g_uZMatrixMaxY = nIndexY;
				}
		
				g_nHeatBedScanStatus = 49;
				g_lastScanTime		 = HAL::timeInMilliseconds();
				break;
			}
			case 60:
			{
				// avoid to crash the extruder against the heat bed during the following homing
				g_nZScanZPosition += moveZ( ZAXIS_STEPS_PER_MM *5 );

				// move back to the home position
				Printer::homeAxis( true, true, true);

				// turn off the engines
				Printer::disableXStepper();
				Printer::disableYStepper();
				Printer::disableZStepper();

				// disable all heaters
				Extruder::setHeatedBedTemperature( 0, false );
				Extruder::setTemperatureForExtruder( 0, 0, false );

				g_nHeatBedScanStatus = 65;
				break;
			}
			case 65:
			{
				if( Printer::debugInfo() )
				{
					// output the determined compensation
					Com::printFLN( PSTR( "scanHeatBed(): raw heat bed compensation matrix: " ) );
					outputCompensationMatrix();
				}

				g_nHeatBedScanStatus = 70;
				break;
			}
			case 70:
			{
				// output the determined pressure
				outputPressureMatrix();

				g_nHeatBedScanStatus = 75;
				break;
			}
			case 75:
			{
				if( Printer::debugInfo() )
				{
					// output the pure scan time
					Com::printF( PSTR( "scanHeatBed(): total scan time: " ), long((HAL::timeInMilliseconds() - g_scanStartTime) / 1000) );
					Com::printFLN( PSTR( " [s]" ) );
				}

				// prepare the heat bed compensation matrix for fast usage during the actual printing
				prepareCompensationMatrix();

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanHeatBed(): g_uZMatrixMaxY.1 = " ), (int)g_uZMatrixMaxY );
				}

				// convert the heat bed compensation matrix for fast usage during the actual printing
				convertCompensationMatrix();

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanHeatBed(): g_uZMatrixMaxY.2 = " ), (int)g_uZMatrixMaxY );

					// output the converted heat bed compensation matrix
					Com::printFLN( PSTR( "scanHeatBed(): converted heat bed compensation matrix: " ) );
					outputCompensationMatrix();
				}

				// save the determined values to the EEPROM
				if( saveCompensationMatrix( EEPROM_SECTOR_SIZE ) )
				{
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "scanHeatBed(): the heat bed compensation matrix could not be saved" ) );
					}
				}
				else
				{
					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "scanHeatBed(): the heat bed compensation matrix has been saved" ) );
					}
				}

				g_nHeatBedScanStatus = 80;
				g_lastScanTime		 = HAL::timeInMilliseconds();
				break;
			}
			case 80:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < HEAT_BED_SCAN_DELAY )
				{
					// do not check too early
					break;
				}

				// compare the idle pressure at the beginning and at the end
				readAveragePressure( &nTempPressure );

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanHeatBed(): idle pressure at start: " ), g_nFirstIdlePressure );
					Com::printFLN( PSTR( "scanHeatBed(): idle pressure at stop: " ), nTempPressure );
				}

				g_nHeatBedScanStatus = 100;
				break;
			}
			case 100:
			{
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanHeatBed(): the scan has been completed" ) );
				}
				UI_STATUS_UPD(UI_TEXT_HEAT_BED_SCAN_DONE);
				BEEP_STOP_HEAT_BED_SCAN

				g_nHeatBedScanStatus = 0;
				break;
			}
		}
	}

	return;

} // scanHeatBed


short testExtruderTemperature( void )
{
	if( Extruder::current->tempControl.targetTemperatureC > 40 )
	{
		// we have to wait until the target temperature is reached
		if( (Extruder::current->tempControl.currentTemperatureC + 2) < Extruder::current->tempControl.targetTemperatureC )
		{
			// wait until the extruder has reached its target temperature
			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "testExtruderTemperature(): heating: " ), Extruder::current->tempControl.currentTemperatureC, 1 );
				Com::printF( PSTR( " C / " ), Extruder::current->tempControl.targetTemperatureC, 1 );
				Com::printFLN( PSTR( " C" ) );
			}

			return -1;
		}
	}
	else
	{
		// we have to wait until the current temperatur is below something which would be too warm
		if( Extruder::current->tempControl.currentTemperatureC > 65 )
		{
			// wait until the extruder has reached its target temperature
			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "testExtruderTemperature(): cooling: " ),Extruder::current->tempControl.currentTemperatureC, 1 );
				Com::printFLN( PSTR( " C" ) );
			}

			return -1;
		}
	}

	// at this point we have reached the proper temperature
	return 0;

} // testExtruderTemperature


short testHeatBedTemperature( void )
{
#if HAVE_HEATED_BED
	if( heatedBedController.targetTemperatureC > 40 )
	{
		// we have to wait until the target temperature is reached
		if( (Extruder::getHeatedBedTemperature() + 2) < heatedBedController.targetTemperatureC )
		{
			// wait until the heat bed has reached its target temperature
			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "testHeatBedTemperature(): heating: " ), Extruder::getHeatedBedTemperature(), 1 );
				Com::printF( PSTR( " C / " ), heatedBedController.targetTemperatureC, 1 );
				Com::printFLN( PSTR( " C" ) );
			}

			return -1;
		}
	}
	else
	{
		// we have to wait until the current temperatur is below something which would be too warm
		if( Extruder::getHeatedBedTemperature() > 50 )
		{
			// wait until the heat bed has reached its target temperature
			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "testHeatBedTemperature(): cooling: " ), Extruder::getHeatedBedTemperature(), 1 );
				Com::printFLN( PSTR( " C" ) );
			}

			return -1;
		}
	}
#endif // HAVE_HEATED_BED

	// at this point we have reached the proper temperature
	return 0;

} // testHeatBedTemperature


void doHeatBedZCompensation( void )
{
	long			nCurrentPosition[3];
	unsigned char	nXLeftIndex;
	unsigned char	nXRightIndex;
	unsigned char	nYFrontIndex;
	unsigned char	nYBackIndex;
	long			nXLeftSteps;
	long			nXRightSteps;
	long			nYFrontSteps;
	long			nYBackSteps;
	long			nTemp;
	long			nDeltaX;
	long			nDeltaY;
	long			nDeltaZ;
	long			nStepSizeX;
	long			nStepSizeY;
	long			nNeededZCompensation;
	long			nTempXFront;
	long			nTempXBack;
	long			nTempZ;
	long			i;


	if( !Printer::doHeatBedZCompensation || g_printingPaused )
	{
		// there is nothing to do at the moment
		return;
	}

	HAL::forbidInterrupts();
	nCurrentPosition[X_AXIS] = Printer::nonCompensatedPositionStepsX;
	nCurrentPosition[Y_AXIS] = Printer::nonCompensatedPositionStepsY;
	nCurrentPosition[Z_AXIS] = Printer::nonCompensatedPositionStepsZ;
	HAL::allowInterrupts();

#if DEBUG_HEAT_BED_Z_COMPENSATION
	g_nNonCompensatedPositionSteps[X_AXIS] = nCurrentPosition[X_AXIS];
	g_nNonCompensatedPositionSteps[Y_AXIS] = nCurrentPosition[Y_AXIS];
	g_nNonCompensatedPositionSteps[Z_AXIS] = nCurrentPosition[Z_AXIS];
#endif // DEBUG_HEAT_BED_Z_COMPENSATION
	
	if( nCurrentPosition[Z_AXIS] > 0 )
	{
		// check whether we have to perform a compensation in z-direction
		if( nCurrentPosition[Z_AXIS] < g_maxZCompensationSteps )
		{
#if FEATURE_WATCHDOG
			HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

			// find the rectangle which covers the current position of the extruder
			nXLeftIndex = 1;
			nXLeftSteps = (long)((float)g_ZCompensationMatrix[1][0] * XAXIS_STEPS_PER_MM);
			for( i=1; i<g_uZMatrixMaxX; i++ )
			{
				nTemp = g_ZCompensationMatrix[i][0];
				nTemp = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
				if( nCurrentPosition[X_AXIS] <= nTemp )
				{
					nXRightIndex = i;
					nXRightSteps = nTemp;
					break;
				}
				nXLeftIndex = i;
				nXLeftSteps = nTemp;
			}
					
			nYFrontIndex = 1;
			nYFrontSteps = (long)((float)g_ZCompensationMatrix[0][1] * YAXIS_STEPS_PER_MM);
			for( i=1; i<g_uZMatrixMaxY; i++ )
			{
				nTemp = g_ZCompensationMatrix[0][i];
				nTemp = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
				if( nCurrentPosition[Y_AXIS] <= nTemp )
				{
					nYBackIndex = i;
					nYBackSteps = nTemp;
					break;
				}
				nYFrontIndex = i;
				nYFrontSteps = nTemp;
			}

			nDeltaX    = nCurrentPosition[X_AXIS] - nXLeftSteps;
			nDeltaY	   = nCurrentPosition[Y_AXIS] - nYFrontSteps;
			nStepSizeX = nXRightSteps - nXLeftSteps;
			nStepSizeY = nYBackSteps - nYFrontSteps;

			// we do a linear interpolation in order to find our exact place within the current rectangle
			nTempXFront = g_ZCompensationMatrix[nXLeftIndex][nYFrontIndex] +
						  (g_ZCompensationMatrix[nXRightIndex][nYFrontIndex] - g_ZCompensationMatrix[nXLeftIndex][nYFrontIndex]) * nDeltaX / nStepSizeX;
			nTempXBack  = g_ZCompensationMatrix[nXLeftIndex][nYBackIndex] +
						  (g_ZCompensationMatrix[nXRightIndex][nYBackIndex] - g_ZCompensationMatrix[nXLeftIndex][nYBackIndex]) * nDeltaX / nStepSizeX;
			nNeededZCompensation = nTempXFront +
								   (nTempXBack - nTempXFront) * nDeltaY / nStepSizeY;

#if DEBUG_HEAT_BED_Z_COMPENSATION
			g_nDelta[X_AXIS]	= nDeltaX;
			g_nDelta[Y_AXIS]	= nDeltaY;
			g_nStepSize[X_AXIS] = nStepSizeX;
			g_nStepSize[Y_AXIS] = nStepSizeY;
			g_nTempXFront		= nTempXFront;
			g_nTempXBack		= nTempXBack;
			g_nNeededZ			= nNeededZCompensation;
			g_uIndex[0]			= nXLeftIndex;
			g_uIndex[1]			= nXRightIndex;
			g_uIndex[2]			= nYFrontIndex;
			g_uIndex[3]			= nYBackIndex;
			g_nMatrix[0]		= g_ZCompensationMatrix[nXLeftIndex][nYFrontIndex];
			g_nMatrix[1]		= g_ZCompensationMatrix[nXRightIndex][nYFrontIndex];
			g_nMatrix[2]		= g_ZCompensationMatrix[nXLeftIndex][nYBackIndex];
			g_nMatrix[3]		= g_ZCompensationMatrix[nXRightIndex][nYBackIndex];
#endif // DEBUG_HEAT_BED_Z_COMPENSATION

			if( nCurrentPosition[Z_AXIS] <= g_minZCompensationSteps )
			{
				// the printer is very close to the surface - we shall print a layer of exactly the desired thickness
				nNeededZCompensation += g_staticZSteps;
			}
			else
			{
				// the printer is already a bit away from the surface - do the actual compensation
				nDeltaZ = g_maxZCompensationSteps - nCurrentPosition[Z_AXIS];
				nNeededZCompensation = g_offsetZCompensationSteps + 
									   (nNeededZCompensation - g_offsetZCompensationSteps) * nDeltaZ / (g_maxZCompensationSteps - g_minZCompensationSteps);
				nNeededZCompensation += g_staticZSteps;
			}
		}
		else
		{	
			// after the first layers, only the static offset to the surface must be compensated
			nNeededZCompensation = g_offsetZCompensationSteps + g_staticZSteps;
		}
	}
	else
	{
		// we do not perform a compensation in case the z-position from the G-code is 0 (because this would drive the extruder against the heat bed)
		nNeededZCompensation = g_staticZSteps;
	}

#if DEBUG_HEAT_BED_Z_COMPENSATION
	long	nZDelta = Printer::targetCompensationZ - nNeededZCompensation;

	if( nZDelta < g_nZDeltaMin )		g_nZDeltaMin = nZDelta;
	if( nZDelta > g_nZDeltaMax )		g_nZDeltaMax = nZDelta;

	g_nZCompensationUpdateTime = micros();

	if( Printer::targetCompensationZ != Printer::currentCompensationZ )
	{
		g_nTooFast ++;
	}
#endif // DEBUG_HEAT_BED_Z_COMPENSATION

	HAL::forbidInterrupts();
	Printer::targetCompensationZ = nNeededZCompensation;
	HAL::allowInterrupts();

#if DEBUG_HEAT_BED_Z_COMPENSATION
	g_nTargetCompensationZ  =  nNeededZCompensation;
	g_nZCompensationUpdates ++;
#endif // DEBUG_HEAT_BED_Z_COMPENSATION

	return;

} // doHeatBedZCompensation
#endif // FEATURE_HEAT_BED_Z_COMPENSATION


#if FEATURE_WORK_PART_Z_COMPENSATION
void startWorkPartScan( char nMode )
{
	if( g_nWorkPartScanStatus )
	{
		// abort the work part scan
		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "startWorkPartScan(): the scan has been cancelled" ) );
		}
		g_abortZScan = 1;
	}
	else
	{
		if( PrintLine::linesCount )
		{
			// there is some printing in progress at the moment - do not start the heat bed scan in this case
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "startWorkPartScan(): the scan can not be started while the milling is in progress" ) );
			}
		}
		else
		{
			// start the work part scan
			g_nWorkPartScanStatus = 1;
			g_nWorkPartScanMode	  = nMode;
			BEEP_START_WORK_PART_SCAN

			// when the work part is scanned, the z-compensation must be disabled
			if( Printer::doWorkPartZCompensation )
			{
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "startWorkPartScan(): the z compensation has been disabled" ) );
				}
				resetZCompensation();
			}
		}
	}

	return;

} // startWorkPartScan


void scanWorkPart( void )
{
	static unsigned char	nIndexX;
	static unsigned char	nIndexY;
	static char				nIndexYDirection;
	static char				nRetry;
	static long				nX;
	static long				nY;
	static long				nZ;
	static long				nYDirection;
	static short			nContactPressure;
	char					nLastWorkPartScanStatus = g_nWorkPartScanStatus;
	short					nTempPressure;
	long					nTempPosition;


	// directions:
	// +x = to the right
	// -x = to the left
	// +y = work part moves to the front
	// -y = work part moves to the back
	// +z = work part moves down
	// -z = work part moves up

	if( g_abortZScan )
	{
		// the scan has been aborted
		g_abortZScan = 0;

		// start at the home position
		if( g_nWorkPartScanMode )
		{
			// also the z-axis shall be homed
			Printer::homeAxis( true, true, true );
		}
		else
		{
			// the z-axis shall not be homed - in this case we must ensure that the tool does not crash against the limit stops at the front/left of the bed
			PrintLine::moveRelativeDistanceInSteps( 0, 0, WORK_PART_SCAN_Z_START_STEPS, 0, MAX_FEEDRATE_Z, true, true );
			Printer::homeAxis( true, true, false );
		}

		// turn off the engines
		Printer::disableXStepper();
		Printer::disableYStepper();
		Printer::disableZStepper();

		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "scanWorkPart(): the scan has been aborted" ) );
		}

		UI_STATUS_UPD( UI_TEXT_WORK_PART_SCAN_ABORTED );
		BEEP_ABORT_WORK_PART_SCAN

		// restore the compensation values from the EEPROM
		if( loadCompensationMatrix( 0 ) )
		{
			// there is no valid compensation matrix available
			initCompensationMatrix();
		}

		g_nWorkPartScanStatus = 0;
		g_nZScanZPosition	  = 0;
		g_nLastZScanZPosition = 0;
		return;
	}

	// show that we are active
	previousMillisCmd = HAL::timeInMilliseconds();

	if( g_nWorkPartScanStatus )
	{
		UI_STATUS( UI_TEXT_WORK_PART_SCAN );

		if( g_retryZScan )
		{
			// we have to retry to scan the current position
			g_nWorkPartScanStatus = 45;
			g_retryZScan		  = 0;
		}

		switch( g_nWorkPartScanStatus )
		{
			case 1:
			{
				g_scanStartTime    = HAL::timeInMilliseconds();
				g_abortZScan	   = 0;
				nContactPressure   = 0;
				g_nTempDirectionZ  = 0;

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): the scan has been started" ) );
				}

				// clear all fields of the work part compensation matrix
				initCompensationMatrix();

				g_uZMatrixMaxX = 0;
				g_uZMatrixMaxY = 0;

				// output the currently used scan parameters
				outputScanParameters();

				g_nWorkPartScanStatus = 10;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 1 -> 10" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 10:
			{
				// start at the home position
				if( g_nWorkPartScanMode )
				{
					// also the z-axis shall be homed
					Printer::homeAxis( true, true, true );
				}
				else
				{
					// the z-axis shall not be homed - in this case we must ensure that the tool does not crash against the limit stops at the front/left of the bed
					PrintLine::moveRelativeDistanceInSteps( 0, 0, WORK_PART_SCAN_Z_START_STEPS, 0, MAX_FEEDRATE_Z, true, true );
					Printer::homeAxis( true, true, false );
				}

				g_nWorkPartScanStatus = 25;
				g_lastScanTime		  = HAL::timeInMilliseconds();

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 10 -> 25" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 25:
			{
				// move to the first position
				Printer::enableZStepper();
				Printer::unsetAllSteppersDisabled();

				PrintLine::moveRelativeDistanceInSteps( g_nScanXStartSteps, 0, 0, 0, MAX_FEEDRATE_X, true, true );
				PrintLine::moveRelativeDistanceInSteps( 0, g_nScanYStartSteps, 0, 0, MAX_FEEDRATE_Y, true, true );

				g_nWorkPartScanStatus = 30;
				g_lastScanTime		  = HAL::timeInMilliseconds();

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 25 -> 30" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 30:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < WORK_PART_SCAN_DELAY )
				{
					// do not check too early
					break;
				}

				if( readIdlePressure( &g_nFirstIdlePressure ) )
				{
					// we were unable to determine the idle pressure
					break;
				}

				g_nMinPressureContact = g_nFirstIdlePressure - g_nScanContactPressureDelta;
				g_nMaxPressureContact = g_nFirstIdlePressure + g_nScanContactPressureDelta;
				g_nMinPressureRetry	  = g_nFirstIdlePressure - g_nScanRetryPressureDelta;
				g_nMaxPressureRetry   = g_nFirstIdlePressure + g_nScanRetryPressureDelta;
				g_nMinPressureIdle	  = g_nFirstIdlePressure - g_nScanIdlePressureDelta;
				g_nMaxPressureIdle	  = g_nFirstIdlePressure + g_nScanIdlePressureDelta;

				// prepare the direction of the z-axis (we have to move the milling bed up)
				prepareBedUp();
				g_nTempDirectionZ = -1;

				nX				 = g_nScanXStartSteps;
				nY				 = g_nScanYStartSteps;
				nZ				 = 0;
				nYDirection		 = g_nScanYStepSizeSteps;	// we start to move the heat bed from the back to the front
				nIndexYDirection = 1;
				nIndexX			 = 2;
				nIndexY			 = 2;

				// store also the version of this heat bed compensation matrix
#if REMEMBER_PRESSURE
				g_HeatBedPressure[0][0]		= EEPROM_FORMAT;
#endif // REMEMBER_PRESSURE

				g_ZCompensationMatrix[0][0] = EEPROM_FORMAT;

				g_nWorkPartScanStatus = 32;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 30 -> 32" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 32:
			{
				short	nCurrentPressure;


				// move the heat bed up until we detect the contact pressure
				g_lastScanTime = HAL::timeInMilliseconds();
				while( 1 )
				{
#if FEATURE_WATCHDOG
					HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

					nCurrentPressure = readStrainGauge( SCAN_STRAIN_GAUGE );

					if( nCurrentPressure > g_nMaxPressureContact || nCurrentPressure < g_nMinPressureContact )
					{
						// we have reached the target pressure
						g_nWorkPartScanStatus = 33;

#if DEBUG_WORK_PART_SCAN
						if( Printer::debugInfo() )
						{
							Com::printFLN( PSTR( "scanWorkPart(): 32 -> 33" ) );
						}
#endif // DEBUG_WORK_PART_SCAN
						return;
					}

					if( Printer::isZMinEndstopHit() )
					{
						if( Printer::debugErrors() )
						{
							Com::printFLN( PSTR( "scanWorkPart(): the z-min endstop has been reached" ) );
						}
						g_abortZScan = 1;
						return;
					}

					g_nZScanZPosition += moveZ( g_nScanHeatBedUpFastSteps );

					if( (HAL::timeInMilliseconds() - g_lastScanTime) > SEARCH_Z_ORIGIN_BREAKOUT_DELAY )
					{
						// do not stay within this loop forever
						return;
					}

					if( g_abortZScan )
					{
						break;
					}
				}

				// we should never end up here
				break;
			}
			case 33:
			{
				short	nCurrentPressure;


				// move the heat bed down again until we do not detect any contact anymore
				g_lastScanTime = HAL::timeInMilliseconds();
				while( 1 )
				{
#if FEATURE_WATCHDOG
					HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

					nCurrentPressure = readStrainGauge( SCAN_STRAIN_GAUGE );

					if( nCurrentPressure > g_nMinPressureContact && nCurrentPressure < g_nMaxPressureContact )
					{
						// we have reached the target pressure / we have found the z-origin
						if( Printer::debugErrors() )
						{
							Com::printFLN( PSTR( "scanWorkPart(): the z-origin has been determined" ) );
						}

						setZOrigin();

						// move away from the surface
						g_nZScanZPosition = nZ = moveZDownFast();
						g_nWorkPartScanStatus = 35;

						// ensure that we do not remember any previous z-position at this moment
						g_nLastZScanZPosition = 0;

#if DEBUG_WORK_PART_SCAN
						if( Printer::debugInfo() )
						{
							Com::printFLN( PSTR( "scanWorkPart(): 33 -> 35 > " ), nZ );
						}
#endif // DEBUG_WORK_PART_SCAN
						return;
					}

					if( Printer::isZMaxEndstopHit() )
					{
						if( Printer::debugErrors() )
						{
							Com::printFLN( PSTR( "scanWorkPart(): the z-max endstop has been reached" ) );
						}
						g_abortZScan = 1;
						return;
					}

					g_nZScanZPosition += moveZ( g_nScanHeatBedDownSlowSteps );

					if( (HAL::timeInMilliseconds() - g_lastScanTime) > SEARCH_Z_ORIGIN_BREAKOUT_DELAY )
					{
						// do not stay within this loop forever
						return;
					}

					if( g_abortZScan )
					{
						break;
					}
				}

				// we should never end up here
				break;
			}
			case 35:
			{
				g_nWorkPartScanStatus = 40;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 35 -> 40" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 39:
			{
				nTempPosition = nX + g_nScanXStepSizeSteps;
				if( nTempPosition > g_nScanXMaxPositionSteps )
				{
					// we end up here when the scan is complete
					g_nWorkPartScanStatus = 60;

#if DEBUG_WORK_PART_SCAN
					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "scanWorkPart(): 39 -> 60" ) );
					}
#endif // DEBUG_WORK_PART_SCAN
					break;
				}

				// move to the next x-position
				PrintLine::moveRelativeDistanceInSteps( g_nScanXStepSizeSteps, 0, 0, 0, MAX_FEEDRATE_X, true, true );
				nX += g_nScanXStepSizeSteps;
				nIndexX ++;

				if( nIndexX > COMPENSATION_MATRIX_MAX_X )
				{
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "scanWorkPart(): the x-dimension of the compensation matrix became too big: " ), nIndexX );
					}
					g_abortZScan = 1;
					break;
				}

				if( nIndexX > g_uZMatrixMaxX )
				{
					g_uZMatrixMaxX = nIndexX;
				}

				if( nYDirection > 0 )
				{
					// we were moving from the front to the back during this column, so we have to move from the back to the front during the next column
					nYDirection		 = -g_nScanYStepSizeSteps;	// we start to move the heat bed from the back to the front
					nIndexYDirection = -1;
				}
				else
				{
					// we were moving from the back to the front during this column, so we have to move from the front to the back during the next column
					nYDirection		 = g_nScanYStepSizeSteps;	// we start to move the heat bed from the back to the front
					nIndexYDirection = 1;
				}

				g_nWorkPartScanStatus = 40;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 39 -> 40" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 40:
			{
				// safety checks
				if( nX <= g_nScanXMaxPositionSteps )
				{
					// remember also the exact x-position of this row/column
#if REMEMBER_PRESSURE
					g_HeatBedPressure[nIndexX][0]	  = nX;
#endif // REMEMBER_PRESSURE

					g_ZCompensationMatrix[nIndexX][0] = (short)((float)nX / XAXIS_STEPS_PER_MM);	// convert to mm

					g_nWorkPartScanStatus = 49;
					g_lastScanTime		  = HAL::timeInMilliseconds();

#if DEBUG_WORK_PART_SCAN
					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "scanWorkPart(): 40 -> 49" ) );
					}
#endif // DEBUG_WORK_PART_SCAN
					break;
				}

				// we end up here when the scan is complete
				g_nWorkPartScanStatus = 60;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 40 -> 60" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 45:
			{
				// move away from the surface
				nZ += moveZ( g_nScanHeatBedDownFastSteps );
				g_nZScanZPosition = nZ;

				g_scanRetries		  --;
				g_nWorkPartScanStatus = 46;
				g_lastScanTime		  = HAL::timeInMilliseconds();

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 45 -> 46 > " ), nZ );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 46:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < WORK_PART_SCAN_DELAY )
				{
					// do not check too early
					break;
				}

				// try to determine the idle pressure again
				if( readIdlePressure( &g_nFirstIdlePressure ) )
				{
					// we were unable to determine the idle pressure
					break;
				}

				g_nMinPressureContact = g_nFirstIdlePressure - g_nScanContactPressureDelta;
				g_nMaxPressureContact = g_nFirstIdlePressure + g_nScanContactPressureDelta;
				g_nMinPressureRetry	  = g_nFirstIdlePressure - g_nScanRetryPressureDelta;
				g_nMaxPressureRetry   = g_nFirstIdlePressure + g_nScanRetryPressureDelta;
				g_nMinPressureIdle	  = g_nFirstIdlePressure - g_nScanIdlePressureDelta;
				g_nMaxPressureIdle	  = g_nFirstIdlePressure + g_nScanIdlePressureDelta;

				g_nWorkPartScanStatus = 50;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 46 -> 50" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 49:
			{
				g_scanRetries		  = WORK_PART_SCAN_RETRIES;
				g_nWorkPartScanStatus = 50;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 49 -> 50" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 50:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < g_nScanIdleDelay )
				{
					// do not check too early
					break;
				}

				// scan this point
				if( testIdlePressure() )
				{
					// the current idle pressure is not plausible
					// break;
				}

				// we should consider that the idle presse can change slightly
				g_nMinPressureContact = g_nCurrentIdlePressure - g_nScanContactPressureDelta;
				g_nMaxPressureContact = g_nCurrentIdlePressure + g_nScanContactPressureDelta;
				g_nMinPressureRetry	  = g_nCurrentIdlePressure - g_nScanRetryPressureDelta;
				g_nMaxPressureRetry   = g_nCurrentIdlePressure + g_nScanRetryPressureDelta;
				g_nMinPressureIdle	  = g_nCurrentIdlePressure - g_nScanIdlePressureDelta;
				g_nMaxPressureIdle	  = g_nCurrentIdlePressure + g_nScanIdlePressureDelta;

				g_nWorkPartScanStatus = 51;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 50 -> 51" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 51:
			{
				// move fast to the surface
				nZ += moveZUpFast();

				g_nWorkPartScanStatus = 52;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 51 -> 52 > " ), nZ );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 52:
			{
				// move a little bit away from the surface
				nZ += moveZDownSlow();

				g_nWorkPartScanStatus = 53;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 52 -> 53 > " ), nZ );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 53:
			{
				// move slowly to the surface
				nZ += moveZUpSlow( &nTempPressure, &nRetry );

				nContactPressure	  = nTempPressure;
				g_nWorkPartScanStatus = 54;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 53 -> 54 > " ), nZ );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 54:
			{
#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printF( PSTR( ";" ), nX );
					Com::printF( PSTR( ";" ), nY );
					Com::printF( PSTR( ";" ), nZ );
					Com::printF( PSTR( ";" ), nContactPressure );

					// output the non compensated position values
					Com::printF( PSTR( ";;" ), Printer::nonCompensatedPositionStepsX );
					Com::printF( PSTR( ";" ), Printer::nonCompensatedPositionStepsY );
					Com::printF( PSTR( ";" ), Printer::nonCompensatedPositionStepsZ );
					Com::printFLN( PSTR( ";" ), Printer::currentCompensationZ );
				}
#endif // DEBUG_WORK_PART_SCAN

				// remember the z-position and the exact y-position of this row/column
				g_ZCompensationMatrix[nIndexX][nIndexY] = (short)nZ;
				g_ZCompensationMatrix[0][nIndexY]		= (short)((float)nY / YAXIS_STEPS_PER_MM);	// convert to mm

#if REMEMBER_PRESSURE
				// remember the pressure and the exact y-position of this row/column
				g_HeatBedPressure[nIndexX][nIndexY]		= nContactPressure;
				g_HeatBedPressure[0][nIndexY]			= nY;
#endif // REMEMBER_PRESSURE

				g_nWorkPartScanStatus = 55;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 54 -> 55 > " ), nZ );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 55:
			{
				// move away from the surface
				nZ += moveZDownFast();

				if( nYDirection > 0 )
				{
					nTempPosition = nY+nYDirection;

					if( nTempPosition > g_nScanYMaxPositionSteps )
					{
						// we have reached the end of this column
						g_nWorkPartScanStatus = 39;

#if DEBUG_WORK_PART_SCAN
						if( Printer::debugInfo() )
						{
							Com::printFLN( PSTR( "scanWorkPart(): 55 -> 39" ) );
						}
#endif // DEBUG_WORK_PART_SCAN
						break;
					}
				}
				else
				{
					nTempPosition = nY+nYDirection;

					if( nTempPosition < g_nScanYStartSteps )
					{
						// we have reached the end of this column
						g_nWorkPartScanStatus = 39;

#if DEBUG_WORK_PART_SCAN
						if( Printer::debugInfo() )
						{
							Com::printFLN( PSTR( "scanWorkPart(): 55 -> 39" ) );
						}
#endif // DEBUG_WORK_PART_SCAN
						break;
					}
				}

				// move to the next y-position
				PrintLine::moveRelativeDistanceInSteps( 0, nYDirection, 0, 0, MAX_FEEDRATE_Y, true, true );
				nY		+= nYDirection;
				nIndexY += nIndexYDirection;

				if( nIndexY > COMPENSATION_MATRIX_MAX_Y )
				{
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "scanWorkPart(): the y-dimension of the compensation matrix became too big: " ), nIndexY );
					}
					g_abortZScan = 1;
					break;
				}

				if( nIndexY > g_uZMatrixMaxY )
				{
					g_uZMatrixMaxY = nIndexY;
				}
		
				g_nWorkPartScanStatus = 49;
				g_lastScanTime		  = HAL::timeInMilliseconds();

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 55 -> 49" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 60:
			{
				// move back to the home position
				if( g_nWorkPartScanMode )
				{
					// also the z-axis shall be homed
					Printer::homeAxis( true, true, true );
				}
				else
				{
					// the z-axis shall not be homed - in this case we must ensure that the tool does not crash against the limit stops at the front/left of the bed
					PrintLine::moveRelativeDistanceInSteps( 0, 0, WORK_PART_SCAN_Z_START_STEPS, 0, MAX_FEEDRATE_Z, true, true );
					Printer::homeAxis( true, true, false );
				}

				// turn off the engines
				Printer::disableXStepper();
				Printer::disableYStepper();
				Printer::disableZStepper();

				g_nWorkPartScanStatus = 65;

#if DEBUG_WORK_PART_SCAN
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): 60 -> 65" ) );
				}
#endif // DEBUG_WORK_PART_SCAN
				break;
			}
			case 65:
			{
				if( Printer::debugInfo() )
				{
					// output the determined compensation
					Com::printFLN( PSTR( "scanWorkPart(): raw work part compensation matrix: " ) );
					outputCompensationMatrix();
				}

				g_nWorkPartScanStatus = 70;
				break;
			}
			case 70:
			{
				// output the determined pressure
				outputPressureMatrix();

				g_nWorkPartScanStatus = 75;
				break;
			}
			case 75:
			{
				if( Printer::debugInfo() )
				{
					// output the pure scan time
					Com::printF( PSTR( "scanWorkPart(): total scan time: " ), long((HAL::timeInMilliseconds() - g_scanStartTime) / 1000) );
					Com::printFLN( PSTR( " [s]" ) );
				}

				// prepare the work part compensation matrix for fast usage during the actual milling
				prepareCompensationMatrix();

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): g_uZMatrixMaxY.1 = " ), (int)g_uZMatrixMaxY );
				}

				// convert the work part compensation matrix for fast usage during the actual printing
				convertCompensationMatrix();

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): g_uZMatrixMaxY.2 = " ), (int)g_uZMatrixMaxY );

					// output the converted work part compensation matrix
					Com::printFLN( PSTR( "scanWorkPart(): converted work part compensation matrix: " ) );
					outputCompensationMatrix();
				}

				// save the determined values to the EEPROM
				if( saveCompensationMatrix( EEPROM_SECTOR_SIZE + (unsigned int)(EEPROM_SECTOR_SIZE * g_nActiveWorkPart) ) )
				{
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "scanWorkPart(): the work part compensation matrix could not be saved" ) );
					}
				}
				else
				{
					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "scanWorkPart(): the work part compensation matrix has been saved > " ), g_nActiveWorkPart );
					}
				}

				g_nWorkPartScanStatus = 80;
				g_lastScanTime		  = HAL::timeInMilliseconds();
				break;
			}
			case 80:
			{
				if( (HAL::timeInMilliseconds() - g_lastScanTime) < WORK_PART_SCAN_DELAY )
				{
					// do not check too early
					break;
				}

				// compare the idle pressure at the beginning and at the end
				readAveragePressure( &nTempPressure );

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): idle pressure at start: " ), g_nFirstIdlePressure );
					Com::printFLN( PSTR( "scanWorkPart(): idle pressure at stop: " ), nTempPressure );
				}

				g_nWorkPartScanStatus = 100;
				break;
			}
			case 100:
			{
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "scanWorkPart(): the scan has been completed" ) );
				}
				UI_STATUS_UPD(UI_TEXT_WORK_PART_SCAN_DONE);
				BEEP_STOP_WORK_PART_SCAN

				g_nWorkPartScanStatus = 0;
				break;
			}
		}
	}

	return;

} // scanWorkPart


void doWorkPartZCompensation( void )
{
	long			nCurrentPosition[3];
	unsigned char	nXLeftIndex;
	unsigned char	nXRightIndex;
	unsigned char	nYFrontIndex;
	unsigned char	nYBackIndex;
	long			nXLeftSteps;
	long			nXRightSteps;
	long			nYFrontSteps;
	long			nYBackSteps;
	long			nTemp;
	long			nDeltaX;
	long			nDeltaY;
	long			nDeltaZ;
	long			nStepSizeX;
	long			nStepSizeY;
	long			nNeededZCompensation;
	long			nTempXFront;
	long			nTempXBack;
	long			nTempZ;
	long			i;


	if( !Printer::doWorkPartZCompensation || g_printingPaused )
	{
		// there is nothing to do at the moment
		return;
	}

	HAL::forbidInterrupts();
	nCurrentPosition[X_AXIS] = Printer::nonCompensatedPositionStepsX;
	nCurrentPosition[Y_AXIS] = Printer::nonCompensatedPositionStepsY;
	nCurrentPosition[Z_AXIS] = Printer::nonCompensatedPositionStepsZ;
	HAL::allowInterrupts();

	if( nCurrentPosition[Z_AXIS] )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

		// find the rectangle which covers the current position of the miller
		nXLeftIndex = 1;
		nXLeftSteps = (long)((float)g_ZCompensationMatrix[1][0] * XAXIS_STEPS_PER_MM);
		for( i=1; i<g_uZMatrixMaxX; i++ )
		{
			nTemp = g_ZCompensationMatrix[i][0];
			nTemp = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
			if( nCurrentPosition[X_AXIS] <= nTemp )
			{
				nXRightIndex = i;
				nXRightSteps = nTemp;
				break;
			}
			nXLeftIndex = i;
			nXLeftSteps = nTemp;
		}
					
		nYFrontIndex = 1;
		nYFrontSteps = (long)((float)g_ZCompensationMatrix[0][1] * YAXIS_STEPS_PER_MM);
		for( i=1; i<g_uZMatrixMaxY; i++ )
		{
			nTemp = g_ZCompensationMatrix[0][i];
			nTemp = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
			if( nCurrentPosition[Y_AXIS] <= nTemp )
			{
				nYBackIndex = i;
				nYBackSteps = nTemp;
				break;
			}
			nYFrontIndex = i;
			nYFrontSteps = nTemp;
		}

		nDeltaX    = nCurrentPosition[X_AXIS] - nXLeftSteps;
		nDeltaY	   = nCurrentPosition[Y_AXIS] - nYFrontSteps;
		nStepSizeX = nXRightSteps - nXLeftSteps;
		nStepSizeY = nYBackSteps - nYFrontSteps;

		// we do a linear interpolation in order to find our exact place within the current rectangle
		nTempXFront = g_ZCompensationMatrix[nXLeftIndex][nYFrontIndex] +
					  (g_ZCompensationMatrix[nXRightIndex][nYFrontIndex] - g_ZCompensationMatrix[nXLeftIndex][nYFrontIndex]) * nDeltaX / nStepSizeX;
		nTempXBack  = g_ZCompensationMatrix[nXLeftIndex][nYBackIndex] +
					  (g_ZCompensationMatrix[nXRightIndex][nYBackIndex] - g_ZCompensationMatrix[nXLeftIndex][nYBackIndex]) * nDeltaX / nStepSizeX;
		nNeededZCompensation = nTempXFront +
							   (nTempXBack - nTempXFront) * nDeltaY / nStepSizeY;
					
		nNeededZCompensation += 
#if FEATURE_FIND_Z_ORIGIN
								Printer::staticCompensationZ +
#endif // FEATURE_FIND_Z_ORIGIN
								g_staticZSteps;
	}
	else
	{
		// we do not perform a compensation in case the z-position from the G-code is 0 (because this would drive the tool against the work part)
		nNeededZCompensation = g_staticZSteps;
	}

	HAL::forbidInterrupts();
	Printer::targetCompensationZ = nNeededZCompensation;
	HAL::allowInterrupts();

	return;

} // doWorkPartZCompensation


void determineStaticCompensationZ( void )
{
	long	nXLeftIndex;
	long	nXRightIndex;
	long	nYFrontIndex;
	long	nYBackIndex;
	long	nXLeftSteps;
	long	nXRightSteps;
	long	nYFrontSteps;
	long	nYBackSteps;
	long	nTemp;
	long	nDeltaX;
	long	nDeltaY;
	long	nDeltaZ;
	long	nStepSizeX;
	long	nStepSizeY;
	long	nTempXFront;
	long	nTempXBack;
	long	i;


	// find the rectangle which covers the current position of the miller
	nXLeftIndex = 1;
	nXLeftSteps = (long)((float)g_ZCompensationMatrix[1][0] * XAXIS_STEPS_PER_MM);
	for( i=1; i<g_uZMatrixMaxX; i++ )
	{
		nTemp = g_ZCompensationMatrix[i][0];
		nTemp = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
		if( Printer::nonCompensatedPositionStepsX <= nTemp )
		{
			nXRightIndex = i;
			nXRightSteps = nTemp;
			break;
		}
		nXLeftIndex = i;
		nXLeftSteps = nTemp;
	}
					
	nYFrontIndex = 1;
	nYFrontSteps = (long)((float)g_ZCompensationMatrix[0][1] * YAXIS_STEPS_PER_MM);
	for( i=1; i<g_uZMatrixMaxY; i++ )
	{
		nTemp = g_ZCompensationMatrix[0][i];
		nTemp = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
		if( Printer::nonCompensatedPositionStepsY <= nTemp )
		{
			nYBackIndex = i;
			nYBackSteps = nTemp;
			break;
		}
		nYFrontIndex = i;
		nYFrontSteps = nTemp;
	}

	nDeltaX    = Printer::nonCompensatedPositionStepsX - nXLeftSteps;
	nDeltaY	   = Printer::nonCompensatedPositionStepsY - nYFrontSteps;
	nStepSizeX = nXRightSteps - nXLeftSteps;
	nStepSizeY = nYBackSteps - nYFrontSteps;

	// we do a linear interpolation in order to find our exact place within the current rectangle
	nTempXFront = g_ZCompensationMatrix[nXLeftIndex][nYFrontIndex] +
			 	  (g_ZCompensationMatrix[nXRightIndex][nYFrontIndex] - g_ZCompensationMatrix[nXLeftIndex][nYFrontIndex]) * nDeltaX / nStepSizeX;
	nTempXBack  = g_ZCompensationMatrix[nXLeftIndex][nYBackIndex] +
				  (g_ZCompensationMatrix[nXRightIndex][nYBackIndex] - g_ZCompensationMatrix[nXLeftIndex][nYBackIndex]) * nDeltaX / nStepSizeX;
		
	Printer::staticCompensationZ = nTempXFront +
								   (nTempXBack - nTempXFront) * nDeltaY / nStepSizeY;
	
	g_debugLog = 1;
	return;

} // determineStaticCompensationZ

#endif // FEATURE_WORK_PART_Z_COMPENSATION


#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
short readIdlePressure( short* pnIdlePressure )
{
	short	nTempPressure;


	// determine the pressure when the heat bed is far away - wait until the measured pressure is rather stable
	nTempPressure	= 0;
	if( readAveragePressure( pnIdlePressure ) )
	{
		// we were unable to determine the pressure
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "readIdlePressure(): the pressure could not be determined" ) );
		}
		return -1;
	}

	while( abs( nTempPressure - *pnIdlePressure) > 5 )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

		if( Printer::debugInfo() )
		{
			Com::printF( PSTR( "readIdlePressure(): pressure calibration: " ), nTempPressure );
			Com::printFLN( PSTR( " / " ), *pnIdlePressure );
		}

		HAL::delayMilliseconds( 500 );

		nTempPressure = *pnIdlePressure;
		if( readAveragePressure( pnIdlePressure ) )
		{
			// we were unable to determine the pressure
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "readIdlePressure(): the pressure could not be determined" ) );
			}
			return -1;
		}
	}

	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "readIdlePressure(): idle pressure: " ), *pnIdlePressure );
	}

	if( *pnIdlePressure < g_nScanIdlePressureMin || *pnIdlePressure > g_nScanIdlePressureMax )
	{
		// the idle pressure is out of range
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "readIdlePressure(): the idle pressure is out of range" ) );
		}
		return -1;
	}

	// at this point we know the idle pressure
	return 0;

} // readIdlePressure


short testIdlePressure( void )
{
	short	nTempPressure;
	short	nTemp;


	if( readAveragePressure( &nTempPressure ) )
	{
		// some error has occurred
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "testIdlePressure(): the pressure could not be determined" ) );
		}
		return -1;
	}
	g_nCurrentIdlePressure = nTempPressure;
	return 0;

	nTemp = 0;
	while( 1 )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

		HAL::delayMilliseconds( g_nScanSlowStepDelay );
		if( readAveragePressure( &nTempPressure ) )
		{
			// some error has occurred
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "testIdlePressure(): the pressure could not be determined" ) );
			}
			return -1;
		}

		if( nTempPressure < g_nMaxPressureIdle && nTempPressure > g_nMinPressureIdle )
		{
			// we have reached the target pressure
			g_nCurrentIdlePressure = nTempPressure;
			return 0;
		}

		nTemp ++;
		if( nTemp > 3 )
		{
			// it does not make sense to try this forever
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "testIdlePressure(): the idle pressure went out of range: " ), nTempPressure );
			}
			g_abortZScan = 1;
			return -1;
		}
	}

	// we should never end up here
	g_abortZScan = 1;
	return -1;

} // testIdlePressure


short readAveragePressure( short* pnAveragePressure )
{
	short	i;
	short	nTempPressure;
	short	nMinPressure;
	short	nMaxPressure;
	long	nPressureSum;
	char	nTemp;


	nTemp = 0;
	while( 1 )
	{
		// we read the strain gauge multiple times and check the variance
		nPressureSum = 0;
		nMinPressure = 32000;
		nMaxPressure = -32000;
		for( i=0; i<g_nScanPressureReads; i++)
		{
#if FEATURE_WATCHDOG
			HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

			HAL::delayMilliseconds( g_nScanPressureReadDelay );
			nTempPressure =  readStrainGauge( SCAN_STRAIN_GAUGE );
			nPressureSum  += nTempPressure;
			if( nTempPressure < nMinPressure )	nMinPressure = nTempPressure;
			if( nTempPressure > nMaxPressure )	nMaxPressure = nTempPressure;
		}
		nTempPressure = (short)(nPressureSum / g_nScanPressureReads);

		if( (nMaxPressure - nMinPressure) < g_nScanPressureTolerance )
		{
			// we have good results
			*pnAveragePressure = nTempPressure;
			return 0;
		}

		nTemp ++;
		if( nTemp >= 5 )
		{
			// we are unable to receive stable values - do not hang here forever
			if( Printer::debugErrors() )
			{
				Com::printF( PSTR( "readAveragePressure(): the pressure is not constant: " ), nMinPressure );
				Com::printF( PSTR( " / " ), nTempPressure );
				Com::printFLN( PSTR( " / " ), nMaxPressure );
			}
			break;
		}
	
		// wait some extra amount of time in case our results were not constant enough
		HAL::delayMilliseconds( 100 );
		
		//runStandardTasks();
		Commands::checkForPeriodicalActions(); 
	}

	if( Printer::debugErrors() )
	{
		Com::printFLN( PSTR( "readAveragePressure(): the pressure is not plausible" ) );
	}
	g_abortZScan	   = 1;
	*pnAveragePressure = 0;
	return -1;

} // readAveragePressure


short moveZUpFast( void )
{
	short	nTempPressure;
	short	nZ = 0;
	short	nSteps;


	// move the heat bed up until we detect the contact pressure (fast speed)
	while( 1 )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

		HAL::delayMilliseconds( g_nScanFastStepDelay );
		if( readAveragePressure( &nTempPressure ) )
		{
			// some error has occurred
			break;
		}

		if( nTempPressure > g_nMaxPressureContact || nTempPressure < g_nMinPressureContact )
		{
			// we have reached the target pressure
			break;
		}

		nSteps			  =  moveZ( g_nScanHeatBedUpFastSteps );
		nZ				  += nSteps;
		g_nZScanZPosition += nSteps;

		runStandardTasks();

		if( g_abortZScan )
		{
			break;
		}

		if( g_nZScanZPosition < -g_nScanZMaxCompensationSteps || g_nZScanZPosition > g_nScanZMaxCompensationSteps )
		{
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "moveZUpFast(): the z position went out of range, retries = " ), (int)g_scanRetries );
			}
			
			if( g_scanRetries )	g_retryZScan = 1;
			else				g_abortZScan = 1;
			break;
		}
	}

	return nZ;

} // moveZUpFast


short moveZDownSlow( void )
{
	short	nTempPressure;
	short	nZ = 0;
	short	nSteps;


	// move the heat bed down until we detect the retry pressure (slow speed)
	while( 1 )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

		HAL::delayMilliseconds( g_nScanSlowStepDelay );
		if( readAveragePressure( &nTempPressure ) )
		{
			// some error has occurred
			break;
		}

		if( nTempPressure < g_nMaxPressureRetry && nTempPressure > g_nMinPressureRetry )
		{
			// we have reached the target pressure
			break;
		}

		nSteps			  =  moveZ( g_nScanHeatBedDownSlowSteps );
		nZ				  += nSteps;
		g_nZScanZPosition += nSteps;
		runStandardTasks();

		if( g_abortZScan )
		{
			break;
		}

		if( g_nZScanZPosition < -g_nScanZMaxCompensationSteps || g_nZScanZPosition > g_nScanZMaxCompensationSteps )
		{
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "moveZDownSlow(): the z position went out of range, retries = " ), g_scanRetries );
			}
			
			if( g_scanRetries )	g_retryZScan = 1;
			else				g_abortZScan = 1;
			break;
		}
		if( g_nLastZScanZPosition )
		{
			if( (g_nZScanZPosition > g_nLastZScanZPosition && (g_nZScanZPosition - g_nLastZScanZPosition) > g_nScanHeatBedDownFastSteps) ||
				(g_nZScanZPosition < g_nLastZScanZPosition && (g_nLastZScanZPosition - g_nZScanZPosition) > g_nScanHeatBedDownFastSteps) )
			{
				if( Printer::debugErrors() )
				{
					Com::printFLN( PSTR( "moveZDownSlow(): the z position delta went out of range, retries = " ), g_scanRetries );
				}
			
				if( g_scanRetries )	g_retryZScan = 1;
				else				g_abortZScan = 1;
				break;
			}
		}
	}

	return nZ;

} // moveZDownSlow


short moveZUpSlow( short* pnContactPressure, char* pnRetry )
{
	short	nTempPressure;
	short	nZ = 0;
	short	nSteps;


	// move the heat bed up until we detect the contact pressure (slow speed)
	while( 1 )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

		HAL::delayMilliseconds( g_nScanSlowStepDelay );
		if( readAveragePressure( &nTempPressure ) )
		{
			// some error has occurred
			break;
		}

		if( nTempPressure > g_nMaxPressureContact || nTempPressure < g_nMinPressureContact )
		{
			// we have found the proper pressure
			break;
		}

		nSteps			  =  moveZ( g_nScanHeatBedUpSlowSteps );
		nZ			  	  += nSteps;
		g_nZScanZPosition += nSteps;
		runStandardTasks();

		if( g_abortZScan )
		{
			break;
		}

		if( g_nZScanZPosition < -g_nScanZMaxCompensationSteps || g_nZScanZPosition > g_nScanZMaxCompensationSteps )
		{
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "moveZUpSlow(): the z position went out of range, retries = " ), g_scanRetries );
			}
			
			if( g_scanRetries )	g_retryZScan = 1;
			else				g_abortZScan = 1;
			break;
		}
	}

	*pnContactPressure = nTempPressure;
	return nZ;

} // moveZUpSlow


short moveZDownFast( void )
{
	short	nTempPressure;
	short	nZ = 0;
	short	nSteps;


	// move the heat bed down so that we won't hit it when we move to the next position
	g_nLastZScanZPosition = g_nZScanZPosition;
	HAL::delayMilliseconds( g_nScanFastStepDelay );

	nSteps			  =  moveZ( g_nScanHeatBedDownFastSteps );
	nZ				  += nSteps;
	g_nZScanZPosition += nSteps;
	runStandardTasks();

	if( readAveragePressure( &nTempPressure ) )
	{
		// some error has occurred
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "moveZDownFast(): the pressure could not be determined" ) );
		}
		g_abortZScan = 1;
		return nZ;
	}

	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "moveZDownFast(): " ), (int)nTempPressure );
	}
	return nZ;

} // moveZDownFast


int moveZ( int nSteps )
{
	int		i;
	int		nMaxLoops;
	char	bBreak;
	char	nRun = 1;
	

	// Warning: this function does not check any end stops
/*	if( g_nMainDirectionZ )
	{
		// this function must not move the z-axis in case the "main" interrupt (bresenhamStep()) is running at the moment
		return 0;
	}			
*/
	// choose the direction
	if( nSteps >= 0 )
	{
//		HAL::forbidInterrupts();
		nMaxLoops = nSteps;

		if( g_nTempDirectionZ != 1 )
		{
			prepareBedDown();

			HAL::delayMicroseconds( XYZ_DIRECTION_CHANGE_DELAY );
			g_nTempDirectionZ = 1;
		}
	}
	else
	{
//		HAL::forbidInterrupts();
		nMaxLoops = -nSteps;

		if( g_nTempDirectionZ != -1 )
		{
			prepareBedUp();

			HAL::delayMicroseconds( XYZ_DIRECTION_CHANGE_DELAY );
			g_nTempDirectionZ = -1;
		}
	}
	
	// perform the steps
	for( i=0; i<nMaxLoops; i++ )
	{
		bBreak = 0;
		nRun   --;

		if( !nRun )
		{
			// process the standard commands from time to time also while the moving in z-direction is in progress
			// runStandardTasks();
			nRun = 10;
		}

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
		if( g_abortZScan )
		{
			bBreak = 1;
		}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
		
#if FEATURE_FIND_Z_ORIGIN
		if( g_abortSearch )
		{
			bBreak = 1;
		}
#endif // FEATURE_FIND_Z_ORIGIN

		if( bBreak )
		{
			// do not continue here in case the current operation has been cancelled
			if( nSteps > 0 )	nSteps = nMaxLoops;
			else				nSteps = -nMaxLoops;
			break;
		}

#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

        HAL::delayMicroseconds( XYZ_STEPPER_HIGH_DELAY );
		startZStep( g_nTempDirectionZ );

        HAL::delayMicroseconds( XYZ_STEPPER_LOW_DELAY );
		endZStep();
	}

//	HAL::allowInterrupts();
	return nSteps;

} // moveZ


int moveExtruder( int nSteps )
{
	int		i;
	int		nMaxLoops;
	
	
	HAL::forbidInterrupts();
	Extruder::enable();
    HAL::delayMicroseconds(EXTRUDER_DIRECTION_CHANGE_DELAY);

	// choose the direction
	if( nSteps >= 0 )
	{
		nMaxLoops = nSteps;
		Extruder::setDirection(true);
	}
	else
	{
		nMaxLoops = -nSteps;
		Extruder::setDirection(false);
	}

	HAL::delayMicroseconds(EXTRUDER_DIRECTION_CHANGE_DELAY);

	// perform the steps
	for( i=0; i<nMaxLoops; i++ )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

        HAL::delayMicroseconds(EXTRUDER_STEPPER_HIGH_DELAY);
		Extruder::step();

        HAL::delayMicroseconds(EXTRUDER_STEPPER_LOW_DELAY);
		Extruder::unstep();
	}

	HAL::allowInterrupts();
	return nSteps;
	
} // moveExtruder


void restoreDefaultScanParameters( void )
{
#if FEATURE_CNC_MODE == 2
	if( Printer::operatingMode == OPERATING_MODE_PRINT )
	{
#if FEATURE_HEAT_BED_Z_COMPENSATION
		// we must restore the default scan parameters for the heat bed scan
		g_nScanXStartSteps			 = HEAT_BED_SCAN_X_START_STEPS;
		g_nScanXStepSizeMm			 = HEAT_BED_SCAN_X_STEP_SIZE_MM;
		g_nScanXStepSizeSteps		 = HEAT_BED_SCAN_X_STEP_SIZE_STEPS;
		g_nScanXEndSteps			 = HEAT_BED_SCAN_X_END_STEPS;
		g_nScanXMaxPositionSteps	 = HEAT_BED_SCAN_X_MAX_POSITION_STEPS;

		g_nScanYStartSteps			 = HEAT_BED_SCAN_Y_START_STEPS;
		g_nScanYStepSizeMm			 = HEAT_BED_SCAN_Y_STEP_SIZE_MM;
		g_nScanYStepSizeSteps		 = HEAT_BED_SCAN_Y_STEP_SIZE_STEPS;
		g_nScanYEndSteps			 = HEAT_BED_SCAN_Y_END_STEPS;
		g_nScanYMaxPositionSteps	 = HEAT_BED_SCAN_Y_MAX_POSITION_STEPS;

		g_nScanHeatBedUpFastSteps	 = HEAT_BED_SCAN_UP_FAST_STEPS;
		g_nScanHeatBedUpSlowSteps	 = HEAT_BED_SCAN_UP_SLOW_STEPS;
		g_nScanHeatBedDownFastSteps	 = HEAT_BED_SCAN_DOWN_FAST_STEPS;
		g_nScanHeatBedDownSlowSteps	 = HEAT_BED_SCAN_DOWN_SLOW_STEPS;
		g_nScanZMaxCompensationSteps = HEAT_BED_Z_COMPENSATION_MAX_STEPS;
		g_nScanFastStepDelay		 = HEAT_BED_SCAN_FAST_STEP_DELAY_MS;
		g_nScanSlowStepDelay		 = HEAT_BED_SCAN_SLOW_STEP_DELAY_MS;
		g_nScanIdleDelay			 = HEAT_BED_SCAN_IDLE_DELAY_MS;

		g_nScanContactPressureDelta	 = HEAT_BED_SCAN_CONTACT_PRESSURE_DELTA;
		g_nScanRetryPressureDelta	 = HEAT_BED_SCAN_RETRY_PRESSURE_DELTA;
		g_nScanIdlePressureDelta	 = HEAT_BED_SCAN_IDLE_PRESSURE_DELTA;
		g_nScanIdlePressureMin		 = HEAT_BED_SCAN_IDLE_PRESSURE_MIN;
		g_nScanIdlePressureMax		 = HEAT_BED_SCAN_IDLE_PRESSURE_MAX;

		g_nScanPressureReads		 = HEAT_BED_SCAN_PRESSURE_READS;
		g_nScanPressureTolerance	 = HEAT_BED_SCAN_PRESSURE_TOLERANCE;
		g_nScanPressureReadDelay	 = HEAT_BED_SCAN_PRESSURE_READ_DELAY_MS;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
	}
	else
	{
#if FEATURE_WORK_PART_Z_COMPENSATION
		// we must restore the default scan parameters for the work part scan
		g_nScanXStartSteps			 = WORK_PART_SCAN_X_START_STEPS;
		g_nScanXStepSizeMm			 = WORK_PART_SCAN_X_STEP_SIZE_MM;
		g_nScanXStepSizeSteps		 = WORK_PART_SCAN_X_STEP_SIZE_STEPS;
		g_nScanXEndSteps			 = WORK_PART_SCAN_X_END_STEPS;
		g_nScanXMaxPositionSteps	 = WORK_PART_SCAN_X_MAX_POSITION_STEPS;

		g_nScanYStartSteps			 = WORK_PART_SCAN_Y_START_STEPS;
		g_nScanYStepSizeMm			 = WORK_PART_SCAN_Y_STEP_SIZE_MM;
		g_nScanYStepSizeSteps		 = WORK_PART_SCAN_Y_STEP_SIZE_STEPS;
		g_nScanYEndSteps			 = WORK_PART_SCAN_Y_END_STEPS;
		g_nScanYMaxPositionSteps	 = WORK_PART_SCAN_Y_MAX_POSITION_STEPS;

		g_nScanHeatBedUpFastSteps	 = WORK_PART_SCAN_UP_FAST_STEPS;
		g_nScanHeatBedUpSlowSteps	 = WORK_PART_SCAN_UP_SLOW_STEPS;
		g_nScanHeatBedDownFastSteps	 = WORK_PART_SCAN_DOWN_FAST_STEPS;
		g_nScanHeatBedDownSlowSteps	 = WORK_PART_SCAN_DOWN_SLOW_STEPS;
		g_nScanZMaxCompensationSteps = WORK_PART_Z_COMPENSATION_MAX_STEPS;
		g_nScanFastStepDelay		 = WORK_PART_SCAN_FAST_STEP_DELAY_MS;
		g_nScanSlowStepDelay		 = WORK_PART_SCAN_SLOW_STEP_DELAY_MS;
		g_nScanIdleDelay			 = WORK_PART_SCAN_IDLE_DELAY_MS;

		g_nScanContactPressureDelta	 = WORK_PART_SCAN_CONTACT_PRESSURE_DELTA;
		g_nScanRetryPressureDelta	 = WORK_PART_SCAN_RETRY_PRESSURE_DELTA;
		g_nScanIdlePressureDelta	 = WORK_PART_SCAN_IDLE_PRESSURE_DELTA;
		g_nScanIdlePressureMin		 = WORK_PART_SCAN_IDLE_PRESSURE_MIN;
		g_nScanIdlePressureMax		 = WORK_PART_SCAN_IDLE_PRESSURE_MAX;

		g_nScanPressureReads		 = WORK_PART_SCAN_PRESSURE_READS;
		g_nScanPressureTolerance	 = WORK_PART_SCAN_PRESSURE_TOLERANCE;
		g_nScanPressureReadDelay	 = WORK_PART_SCAN_PRESSURE_READ_DELAY_MS;
#endif // FEATURE_WORK_PART_Z_COMPENSATION
	}
#else
#if FEATURE_HEAT_BED_Z_COMPENSATION
	// we must restore the default scan parameters for the heat bed scan
	g_nScanXStartSteps			 = HEAT_BED_SCAN_X_START_STEPS;
	g_nScanXStepSizeMm			 = HEAT_BED_SCAN_X_STEP_SIZE_MM;
	g_nScanXStepSizeSteps		 = HEAT_BED_SCAN_X_STEP_SIZE_STEPS;
	g_nScanXEndSteps			 = HEAT_BED_SCAN_X_END_STEPS;
	g_nScanXMaxPositionSteps	 = HEAT_BED_SCAN_X_MAX_POSITION_STEPS;

	g_nScanYStartSteps			 = HEAT_BED_SCAN_Y_START_STEPS;
	g_nScanYStepSizeMm			 = HEAT_BED_SCAN_Y_STEP_SIZE_MM;
	g_nScanYStepSizeSteps		 = HEAT_BED_SCAN_Y_STEP_SIZE_STEPS;
	g_nScanYEndSteps			 = HEAT_BED_SCAN_Y_END_STEPS;
	g_nScanYMaxPositionSteps	 = HEAT_BED_SCAN_Y_MAX_POSITION_STEPS;

	g_nScanHeatBedUpFastSteps	 = HEAT_BED_SCAN_UP_FAST_STEPS;
	g_nScanHeatBedUpSlowSteps	 = HEAT_BED_SCAN_UP_SLOW_STEPS;
	g_nScanHeatBedDownFastSteps	 = HEAT_BED_SCAN_DOWN_FAST_STEPS;
	g_nScanHeatBedDownSlowSteps	 = HEAT_BED_SCAN_DOWN_SLOW_STEPS;
	g_nScanZMaxCompensationSteps = HEAT_BED_Z_COMPENSATION_MAX_STEPS;
	g_nScanFastStepDelay		 = HEAT_BED_SCAN_FAST_STEP_DELAY_MS;
	g_nScanSlowStepDelay		 = HEAT_BED_SCAN_SLOW_STEP_DELAY_MS;
	g_nScanIdleDelay			 = HEAT_BED_SCAN_IDLE_DELAY_MS;

	g_nScanContactPressureDelta	 = HEAT_BED_SCAN_CONTACT_PRESSURE_DELTA;
	g_nScanRetryPressureDelta	 = HEAT_BED_SCAN_RETRY_PRESSURE_DELTA;
	g_nScanIdlePressureDelta	 = HEAT_BED_SCAN_IDLE_PRESSURE_DELTA;
	g_nScanIdlePressureMin		 = HEAT_BED_SCAN_IDLE_PRESSURE_MIN;
	g_nScanIdlePressureMax		 = HEAT_BED_SCAN_IDLE_PRESSURE_MAX;

	g_nScanPressureReads		 = HEAT_BED_SCAN_PRESSURE_READS;
	g_nScanPressureTolerance	 = HEAT_BED_SCAN_PRESSURE_TOLERANCE;
	g_nScanPressureReadDelay	 = HEAT_BED_SCAN_PRESSURE_READ_DELAY_MS;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
#endif // FEATURE_CNC_MODE == 2

	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "restoreDefaultScanParameters(): the default scan parameters have been restored" ) );
	}
	return;

} // restoreDefaultScanParameters


void outputScanParameters( void )
{
	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "outputScanParameters(): current scan parameters:" ) );

		Com::printF( PSTR( "" ), ZAXIS_STEPS_PER_MM );					Com::printFLN( PSTR( ";[steps];ZAXIS_STEPS_PER_MM" ) );

		Com::printF( PSTR( "" ), g_nScanXStartSteps );					Com::printFLN( PSTR( ";[steps];g_nScanXStartSteps" ) );
		Com::printF( PSTR( "" ), g_nScanXStepSizeSteps );				Com::printFLN( PSTR( ";[steps];g_nScanXStepSizeSteps" ) );
		Com::printF( PSTR( "" ), g_nScanXEndSteps );					Com::printFLN( PSTR( ";[steps];g_nScanXEndSteps" ) );
		Com::printF( PSTR( "" ), g_nScanXMaxPositionSteps );			Com::printFLN( PSTR( ";[steps];g_nScanXMaxPositionSteps" ) );

		Com::printF( PSTR( "" ), g_nScanYStartSteps );					Com::printFLN( PSTR( ";[steps];g_nScanYStartSteps" ) );
		Com::printF( PSTR( "" ), g_nScanYStepSizeSteps );				Com::printFLN( PSTR( ";[steps];g_nScanYStepSizeSteps" ) );
		Com::printF( PSTR( "" ), g_nScanYEndSteps );					Com::printFLN( PSTR( ";[steps];g_nScanYEndSteps" ) );
		Com::printF( PSTR( "" ), g_nScanYMaxPositionSteps );			Com::printFLN( PSTR( ";[steps];g_nScanYMaxPositionSteps" ) );

		Com::printF( PSTR( "" ), (int)g_nScanHeatBedUpFastSteps );		Com::printFLN( PSTR( ";[steps];g_nScanHeatBedUpFastSteps" ) );
		Com::printF( PSTR( "" ), (int)g_nScanHeatBedUpSlowSteps );		Com::printFLN( PSTR( ";[steps];g_nScanHeatBedUpSlowSteps" ) );
		Com::printF( PSTR( "" ), (int)g_nScanHeatBedDownFastSteps );	Com::printFLN( PSTR( ";[steps];g_nScanHeatBedDownFastSteps" ) );
		Com::printF( PSTR( "" ), (int)g_nScanHeatBedDownSlowSteps );	Com::printFLN( PSTR( ";[steps];g_nScanHeatBedDownSlowSteps" ) );
		Com::printF( PSTR( "" ), (int)g_nScanFastStepDelay );			Com::printFLN( PSTR( ";[ms];g_nScanFastStepDelay" ) );
		Com::printF( PSTR( "" ), (int)g_nScanSlowStepDelay );			Com::printFLN( PSTR( ";[ms];g_nScanSlowStepDelay" ) );
		Com::printF( PSTR( "" ), (int)g_nScanIdleDelay );				Com::printFLN( PSTR( ";[ms];g_nScanIdleDelay" ) );

		Com::printF( PSTR( "" ), (int)g_nScanContactPressureDelta );	Com::printFLN( PSTR( ";[digits];g_nScanContactPressureDelta" ) );
		Com::printF( PSTR( "" ), (int)g_nScanRetryPressureDelta );		Com::printFLN( PSTR( ";[digits];g_nScanRetryPressureDelta" ) );
		Com::printF( PSTR( "" ), (int)g_nScanIdlePressureDelta );		Com::printFLN( PSTR( ";[digits];g_nScanIdlePressureDelta" ) );

		Com::printF( PSTR( "" ), (int)g_nScanPressureReads );			Com::printFLN( PSTR( ";[-];g_nScanPressureReads" ) );
		Com::printF( PSTR( "" ), (int)g_nScanPressureTolerance );		Com::printFLN( PSTR( ";[digits];g_nScanPressureTolerance" ) );
		Com::printF( PSTR( "" ), (int)g_nScanPressureReadDelay );		Com::printFLN( PSTR( ";[ms];g_nScanPressureReadDelay" ) );
	}
	return;

} // outputScanParameters


void initCompensationMatrix( void )
{
	// clear all fields of the compensation matrix
	memset( g_ZCompensationMatrix, 0, COMPENSATION_MATRIX_MAX_X*COMPENSATION_MATRIX_MAX_Y*2 );
	return;

} // initCompensationMatrix


void outputCompensationMatrix( void )
{
	if( Printer::debugInfo() )
	{
		short	x;
		short	y;


//		Com::printFLN( PSTR( "z compensation matrix:" ) );
		Com::printFLN( PSTR( "front left ... front right" ) );
		Com::printFLN( PSTR( "...        ...         ..." ) );
		Com::printFLN( PSTR( "back left  ...  back right" ) );

#if FEATURE_HEAT_BED_Z_COMPENSATION
		g_offsetZCompensationSteps = -32000;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

		for( y=0; y<g_uZMatrixMaxY; y++ )
		{
			for( x=0; x<g_uZMatrixMaxX; x++ )
			{
				Com::printF( PSTR( ";" ), g_ZCompensationMatrix[x][y] );

#if FEATURE_HEAT_BED_Z_COMPENSATION
				if( x>0 && y>0 && g_ZCompensationMatrix[x][y] > g_offsetZCompensationSteps )
				{
					g_offsetZCompensationSteps = g_ZCompensationMatrix[x][y];
				}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
			}
			Com::printFLN( PSTR( " " ) );
		}

#if FEATURE_HEAT_BED_Z_COMPENSATION
		Com::printFLN( PSTR( "offset = " ), g_offsetZCompensationSteps );
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

		Com::printFLN( PSTR( "g_uZMatrixMaxX = " ), g_uZMatrixMaxX );
		Com::printFLN( PSTR( "g_uZMatrixMaxY = " ), g_uZMatrixMaxY );

#if FEATURE_WORK_PART_Z_COMPENSATION && FEATURE_CNC_MODE > 0
		if( Printer::operatingMode == OPERATING_MODE_CNC )
		{
			Com::printFLN( PSTR( "g_nActiveWorkPart = " ), g_nActiveWorkPart );
			Com::printF( PSTR( "scan start: x = " ), (float)g_nScanXStartSteps / XAXIS_STEPS_PER_MM );
			Com::printF( PSTR( ", y = " ), (float)g_nScanYStartSteps / YAXIS_STEPS_PER_MM );
			Com::printFLN( PSTR( " [mm]" ) );
			Com::printF( PSTR( "scan steps: x = " ), (float)g_nScanXStepSizeMm );
			Com::printF( PSTR( ", y = " ), (float)g_nScanYStepSizeMm );
			Com::printFLN( PSTR( " [mm]" ) );
			Com::printF( PSTR( "scan end: x = " ), (float)g_nScanXMaxPositionSteps / XAXIS_STEPS_PER_MM );
			Com::printF( PSTR( ", y = " ), (float)g_nScanYMaxPositionSteps / YAXIS_STEPS_PER_MM );
			Com::printFLN( PSTR( " [mm]" ) );
		}
#endif // FEATURE_WORK_PART_Z_COMPENSATION && FEATURE_CNC_MODE > 0
	}

	return;

} // outputCompensationMatrix


char prepareCompensationMatrix( void )
{
	short	x;
	short	y;


	// perform some safety checks first
	if( g_ZCompensationMatrix[0][0] != EEPROM_FORMAT )
	{
		if( Printer::debugErrors() )
		{
			Com::printF( PSTR( "prepareCompensationMatrix(): invalid version detected: " ), g_ZCompensationMatrix[0][0] );
			Com::printF( PSTR( " (expected: " ), EEPROM_FORMAT );
			Com::printFLN( PSTR( ")" ) );
		}
		return -1;
	}
	
	if( g_uZMatrixMaxX > COMPENSATION_MATRIX_MAX_X || g_uZMatrixMaxX < 2 )
	{
		if( Printer::debugErrors() )
		{
			Com::printF( PSTR( "prepareCompensationMatrix(): invalid x dimension detected: " ), g_uZMatrixMaxX );
			Com::printF( PSTR( " (max expected: " ), COMPENSATION_MATRIX_MAX_X );
			Com::printFLN( PSTR( ")" ) );
		}
		return -1;
	}

	if( g_uZMatrixMaxY > COMPENSATION_MATRIX_MAX_Y || g_uZMatrixMaxY < 2 )
	{
		if( Printer::debugErrors() )
		{
			Com::printF( PSTR( "prepareCompensationMatrix(): invalid y dimension detected: " ), g_uZMatrixMaxY );
			Com::printF( PSTR( " (max expected: " ), COMPENSATION_MATRIX_MAX_Y );
			Com::printFLN( PSTR( ")" ) );
		}
		return -1;
	}

	if( g_ZCompensationMatrix[2][0] > 0 )
	{
		// we have to fill x[1] with the values of x[2]
/*		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "prepareCompensationMatrix(): x[2] > 0" ) );
		}
*/		g_ZCompensationMatrix[1][0] = 0;
		for( y=1; y<g_uZMatrixMaxY; y++ )
		{
			g_ZCompensationMatrix[1][y] = g_ZCompensationMatrix[2][y];
		}
	}
	else
	{
		// we have to shift all x columns one index
/*		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "prepareCompensationMatrix(): x[2] = 0" ) );
		}
*/		for( x=1; x<g_uZMatrixMaxX-1; x++ )
		{
			for( y=0; y<g_uZMatrixMaxY; y++ )
			{
				g_ZCompensationMatrix[x][y] = g_ZCompensationMatrix[x+1][y];
			}
		}

		// we have one x column less now
		g_uZMatrixMaxX --;
	}

	if( g_ZCompensationMatrix[g_uZMatrixMaxX-1][0] < (short)X_MAX_LENGTH )
	{
		// we have to fill x[g_uZMatrixMaxX] with the values of x[g_uZMatrixMaxX-1]
/*		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "prepareCompensationMatrix(): x[g_uZMatrixMaxX-1] < X_MAX_LENGTH" ) );
		}
*/		g_ZCompensationMatrix[g_uZMatrixMaxX][0] = short(X_MAX_LENGTH);
		for( y=1; y<g_uZMatrixMaxY; y++ )
		{
			g_ZCompensationMatrix[g_uZMatrixMaxX][y] = g_ZCompensationMatrix[g_uZMatrixMaxX-1][y];
		}

		// we have one x column more now
		g_uZMatrixMaxX ++;
	}
	else
	{
		// there is nothing else to do here
/*		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "prepareCompensationMatrix(): x[g_uZMatrixMaxX-1] = X_MAX_LENGTH" ) );
		}
*/	}

	if( g_ZCompensationMatrix[0][2] > 0 )
	{
		// we have to fill y[1] with the values of y[2]
/*		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "prepareCompensationMatrix(): y[2] > 0" ) );
		}
*/		g_ZCompensationMatrix[0][1] = 0;
		for( x=1; x<g_uZMatrixMaxX; x++ )
		{
			g_ZCompensationMatrix[x][1] = g_ZCompensationMatrix[x][2];
		}
	}
	else
	{
		// we have to shift all y columns one index
/*		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "prepareCompensationMatrix(): y[2] = 0" ) );
		}
*/		for( x=0; x<g_uZMatrixMaxX; x++ )
		{
			for( y=1; y<g_uZMatrixMaxY-1; y++ )
			{
				g_ZCompensationMatrix[x][y] = g_ZCompensationMatrix[x][y+1];
			}
		}

		// we have one y column less now
		g_uZMatrixMaxY --;
	}

	if( g_ZCompensationMatrix[0][g_uZMatrixMaxY-1] < short(Y_MAX_LENGTH) )
	{
		// we have to fill y[g_uZMatrixMaxY] with the values of y[g_uZMatrixMaxY-1]
/*		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "prepareCompensationMatrix(): y[g_uZMatrixMaxY-1] < Y_MAX_LENGTH" ) );
		}
*/		g_ZCompensationMatrix[0][g_uZMatrixMaxY] = short(Y_MAX_LENGTH);
		for( x=1; x<g_uZMatrixMaxX; x++ )
		{
			g_ZCompensationMatrix[x][g_uZMatrixMaxY] = g_ZCompensationMatrix[x][g_uZMatrixMaxY-1];
		}

		// we have one y column more now
		g_uZMatrixMaxY ++;
	}
	else
	{
		// there is nothing else to do here
/*		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "prepareCompensationMatrix(): y[g_uZMatrixMaxY-1] = Y_MAX_LENGTH" ) );
		}
*/	}

	return 0;

} // prepareCompensationMatrix


char convertCompensationMatrix( void )
{
//	long	nSum;
	short	x;
	short	y;


#if FEATURE_HEAT_BED_Z_COMPENSATION
	g_offsetZCompensationSteps = -32000;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

	for( x=1; x<g_uZMatrixMaxX-1; x++ )
	{
		for( y=1; y<g_uZMatrixMaxY-1; y++ )
		{
/*			// we calculate the average of each rectangle m[x][y], m[x+1][y], m[x][y+1], m[x+1][y+1] and store it to m[x][y]
			nSum =  g_ZCompensationMatrix[x][y];
			nSum +=	g_ZCompensationMatrix[x+1][y];
			nSum += g_ZCompensationMatrix[x][y+1];
			nSum += g_ZCompensationMatrix[x+1][y+1];
			nSum /= 4;

			g_ZCompensationMatrix[x][y] = (short)nSum;
*/
#if FEATURE_HEAT_BED_Z_COMPENSATION
			if( x>0 && y>0 && g_ZCompensationMatrix[x][y] > g_offsetZCompensationSteps )
			{
				g_offsetZCompensationSteps = g_ZCompensationMatrix[x][y];
			}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
		}
	}
	return 0;

} // convertCompensationMatrix


char saveCompensationMatrix( unsigned int uAddress )
{
	unsigned int	uOffset;
	short			uTemp;
	short			uMax = -32000;
	short			x;
	short			y;


	if( g_ZCompensationMatrix[0][0] && g_uZMatrixMaxX && g_uZMatrixMaxY )
	{
		// we have valid compensation values
		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "saveCompensationMatrix(): valid data detected" ) );
		}

		// write the current header version
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, EEPROM_OFFSET_HEADER_FORMAT, EEPROM_FORMAT );
		
		// write the current sector version
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_SECTOR_FORMAT, EEPROM_FORMAT );
		
		// write the current x dimension
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_DIMENSION_X, g_uZMatrixMaxX );

		// write the current y dimension
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_DIMENSION_Y, g_uZMatrixMaxY );

		// write the current micro steps
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_MICRO_STEPS, RF1000_MICRO_STEPS );

		// write some information about the scanning area - note that this information is read only in case of work part z-compensation matrixes later
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_START_MM, (short)(g_nScanXStartSteps / XAXIS_STEPS_PER_MM) );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_START_MM, (short)(g_nScanYStartSteps / YAXIS_STEPS_PER_MM) );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_STEP_MM, (short)g_nScanXStepSizeMm );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_STEP_MM, (short)g_nScanYStepSizeMm );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_END_MM, (short)(g_nScanXMaxPositionSteps / XAXIS_STEPS_PER_MM) );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_END_MM, (short)(g_nScanYMaxPositionSteps / YAXIS_STEPS_PER_MM) );

		uOffset = uAddress + EEPROM_OFFSET_MAXTRIX_START;
		for( x=0; x<g_uZMatrixMaxX; x++ )
		{
			for( y=0; y<g_uZMatrixMaxY; y++ )
			{
#if FEATURE_WATCHDOG
				HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

				uTemp = g_ZCompensationMatrix[x][y];
				writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uOffset, uTemp );
				uOffset += 2;

				if( x>0 && y>0 )
				{
					// the first column and row is used for version and position information
					if( uTemp > uMax )	uMax = uTemp;
				}
			}
		}
	}
	else
	{
		// we do not have valid heat bed compensation values - clear the EEPROM data
		if( Printer::debugErrors() )
		{
			Com::printF( PSTR( "saveCompensationMatrix(): invalid data detected ( " ), g_ZCompensationMatrix[0][0] );
			Com::printF( PSTR( " / " ), g_uZMatrixMaxX );
			Com::printF( PSTR( " / " ), g_uZMatrixMaxY );
			Com::printFLN( PSTR( " )" ), g_uZMatrixMaxY );
		}

		// write the current version
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, EEPROM_OFFSET_HEADER_FORMAT, EEPROM_FORMAT );
		
		// write the current sector version
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_SECTOR_FORMAT, 0 );
		
		// write the current x dimension
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_DIMENSION_X, 0 );

		// write the current y dimension
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_DIMENSION_Y, 0 );

		// write the current micro steps
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_MICRO_STEPS, 0 );

		// write some information about the scanning area - note that this information is read only in case of work part z-compensation matrixes later
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_START_MM, 0 );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_START_MM, 0 );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_STEP_MM, 0 );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_STEP_MM, 0 );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_END_MM, 0 );
		writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_END_MM, 0 );

		uOffset = uAddress + EEPROM_OFFSET_MAXTRIX_START;
		for( x=0; x<COMPENSATION_MATRIX_MAX_X; x++ )
		{
			for( y=0; y<COMPENSATION_MATRIX_MAX_Y; y++ )
			{
#if FEATURE_WATCHDOG
				HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

				writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uOffset, 0 );
				uOffset += 2;
			}
		}
	}

#if FEATURE_HEAT_BED_Z_COMPENSATION
	g_offsetZCompensationSteps = uMax;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

	return 0;

} // saveCompensationMatrix


char loadCompensationMatrix( unsigned int uAddress )
{
	unsigned short	uTemp;
	unsigned short	uDimensionX;
	unsigned short	uDimensionY;
	unsigned short	uMicroSteps;
	unsigned int	uOffset;
	short			nTemp;
	short			uMax = -32000;
	short			x;
	short			y;
	float			fMicroStepCorrection;


	// check the stored header format
	uTemp = readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, EEPROM_OFFSET_HEADER_FORMAT );

	if( uTemp != EEPROM_FORMAT )
	{
		if( Printer::debugErrors() )
		{
			Com::printF( PSTR( "loadCompensationMatrix(): invalid header format detected: " ), (int)uTemp );
			Com::printF( PSTR( " (expected: " ), EEPROM_FORMAT );
			Com::printFLN( PSTR( ")" ) );
		}
		return -1;
	}

	if( !uAddress )
	{
		// we have to detect the to-be-loaded compensation matrix automatically
#if FEATURE_CNC_MODE == 2
		if( Printer::operatingMode == OPERATING_MODE_PRINT )
		{
#if FEATURE_HEAT_BED_Z_COMPENSATION
			// load the heat bed compensation matrix
			uAddress = EEPROM_SECTOR_SIZE;
#else
			// we do not support the heat bed compensation
			return -1;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
		}
		else
		{
#if FEATURE_WORK_PART_Z_COMPENSATION
			// load the currently active work part compensation matrix
			uTemp = readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, EEPROM_OFFSET_ACTIVE_WORK_PART );

			if( uTemp < 1 || uTemp > EEPROM_MAX_WORK_PART_SECTORS )
			{
				if( Printer::debugErrors() )
				{
					Com::printFLN( PSTR( "loadCompensationMatrix(): invalid active work part detected: " ), (int)uTemp );
				}
				return -1;
			}

			g_nActiveWorkPart = (char)uTemp;
			uAddress		  = EEPROM_SECTOR_SIZE + (unsigned int)(EEPROM_SECTOR_SIZE * uTemp);
#else
			// we do not support the work part compensation
			return -1;
#endif // FEATURE_WORK_PART_Z_COMPENSATION
		}
#else
#if FEATURE_HEAT_BED_Z_COMPENSATION
		// load the heat bed compensation matrix
		uAddress = EEPROM_SECTOR_SIZE;
#else
		// we do not support the heat bed compensation
		return -1;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
#endif // FEATURE_CNC_MODE == 2
	}

#if FEATURE_WORK_PART_Z_COMPENSATION && FEATURE_CNC_MODE == 2
	if( Printer::operatingMode == OPERATING_MODE_CNC )
	{
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "loadCompensationMatrix(): active work part: " ), (int)g_nActiveWorkPart );
		}
	}
#endif // FEATURE_WORK_PART_Z_COMPENSATION && FEATURE_CNC_MODE == 2

	// check the stored sector format
	uTemp = readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_SECTOR_FORMAT );

	if( uTemp != EEPROM_FORMAT )
	{
		if( Printer::debugErrors() )
		{
			Com::printF( PSTR( "loadCompensationMatrix(): invalid sector format detected: " ), (int)uTemp );
			Com::printF( PSTR( " (expected: " ), EEPROM_FORMAT );
			Com::printFLN( PSTR( ")" ) );
		}
		return -1;
	}

	// check the stored x dimension
	uDimensionX = readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_DIMENSION_X );

	if( uDimensionX > COMPENSATION_MATRIX_MAX_X )
	{
		if( Printer::debugErrors() )
		{
			Com::printF( PSTR( "loadCompensationMatrix(): invalid x dimension detected: " ), (int)uDimensionX );
			Com::printF( PSTR( " (max expected: " ), COMPENSATION_MATRIX_MAX_X );
			Com::printFLN( PSTR( ")" ) );
		}
		return -1;
	}

	// check the stored y dimension
	uDimensionY = readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_DIMENSION_Y );

	if( uDimensionY > COMPENSATION_MATRIX_MAX_Y )
	{
		if( Printer::debugErrors() )
		{
			Com::printF( PSTR( "loadCompensationMatrix(): invalid y dimension detected: " ), (int)uDimensionY );
			Com::printF( PSTR( " (max expected: " ), COMPENSATION_MATRIX_MAX_Y );
			Com::printFLN( PSTR( ")" ) );
		}
		return -1;
	}

	g_uZMatrixMaxX = (unsigned char)uDimensionX;
	g_uZMatrixMaxY = (unsigned char)uDimensionY;

	// check the stored microsteps
	uMicroSteps = readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_MICRO_STEPS );

	if( uMicroSteps == RF1000_MICRO_STEPS )
	{
		// the current z-compensation matrix has been determined with the current micro step setting, there is nothing to recalculate
		fMicroStepCorrection = 1.0;
	}
	else if( uMicroSteps > RF1000_MICRO_STEPS )
	{
		// the current z-compensation matrix has been determined with a higher than the current micro step setting, we must divide all z-correction values
		fMicroStepCorrection = (float)RF1000_MICRO_STEPS / (float)uMicroSteps;
	}
	else
	{
		// the current z-compensation matrix has been determined with a smaller than the current micro step setting, we must multiply all z-correction values
		fMicroStepCorrection = (float)RF1000_MICRO_STEPS / (float)uMicroSteps;
	}

	if( uAddress != EEPROM_SECTOR_SIZE )
	{
		// in case we are reading a work part z-compensation matrix, we have to read out some information about the scanning area
		g_nScanXStartSteps		 = (long)readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_START_MM ) * XAXIS_STEPS_PER_MM;
		g_nScanYStartSteps		 = (long)readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_START_MM ) * YAXIS_STEPS_PER_MM;
		g_nScanXStepSizeMm		 = (long)readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_STEP_MM );
		g_nScanYStepSizeMm		 = (long)readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_STEP_MM );
		g_nScanXMaxPositionSteps = (long)readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_X_END_MM ) * XAXIS_STEPS_PER_MM;
		g_nScanYMaxPositionSteps = (long)readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uAddress + EEPROM_OFFSET_Y_END_MM ) * YAXIS_STEPS_PER_MM;
		g_nScanXStepSizeSteps	 = g_nScanXStepSizeMm * XAXIS_STEPS_PER_MM;
		g_nScanYStepSizeSteps	 = g_nScanYStepSizeMm * YAXIS_STEPS_PER_MM;
	}

	// read out the actual compensation values
	uOffset = uAddress + EEPROM_OFFSET_MAXTRIX_START;
	for( x=0; x<g_uZMatrixMaxX; x++ )
	{
		for( y=0; y<g_uZMatrixMaxY; y++ )
		{
#if FEATURE_WATCHDOG
			HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

			nTemp = readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, uOffset );

			if( x == 0 || y == 0 )
			{
				// we must not modify our header row/column
				g_ZCompensationMatrix[x][y] = nTemp;
			}
			else
			{
				// we may have to update all z-compensation values
				g_ZCompensationMatrix[x][y] = (short)((float)nTemp * fMicroStepCorrection);
			}
			uOffset += 2;

			if( x>0 && y>0 )
			{
				// the first column and row is used for version and position information
				if( nTemp > uMax )	uMax = nTemp;
			}
		}
	}

#if FEATURE_HEAT_BED_Z_COMPENSATION
	g_offsetZCompensationSteps = uMax;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

	resetZCompensation();
	return 0;

} // loadCompensationMatrix


void clearCompensationMatrix( unsigned int uAddress )
{
	// clear all fields of the compensation matrix
	initCompensationMatrix();

	// store the cleared compensation matrix to the EEPROM
	saveCompensationMatrix( uAddress );

	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "clearCompensationMatrix(): the compensation matrix has been cleared" ) );
	}
	return;

} // clearCompensationMatrix


void outputPressureMatrix( void )
{
	if( Printer::debugInfo() )
	{
#if REMEMBER_PRESSURE
		short	i;
		short	j;


		Com::printFLN( PSTR( "Pressure matrix:" ) );
		Com::printFLN( PSTR( "front left ... front right" ) );
		Com::printFLN( PSTR( "...        ...         ..." ) );
		Com::printFLN( PSTR( "back left  ...  back right" ) );
		for( i=0; i<COMPENSATION_MATRIX_MAX_Y; i++ )
		{
			for( j=0; j<COMPENSATION_MATRIX_MAX_X; j++ )
			{
				Com::printF( PSTR( ";" ), g_HeatBedPressure[j][i] );
			}
			Com::printFLN( PSTR( " " ) );
		}
#endif // REMEMBER_PRESSURE
	}

	return;

} // outputPressureMatrix
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION


char clearExternalEEPROM( void )
{
	unsigned short	i;
	unsigned short	uMax = 32768;
	unsigned short	uTemp;
	unsigned short	uLast = 0;


	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "clearExternalEEPROM(): erasing external memory ..." ) );
	}

	// the external EEPROM is able to store 262.144 kBit (= 32.768 kByte)
	for( i=0; i<uMax; i++ )
	{
		writeByte24C256( I2C_ADDRESS_EXTERNAL_EEPROM, i, 0 );
		Commands::checkForPeriodicalActions();

		if( Printer::debugInfo() )
		{
			uTemp = i / 100;
			if( uTemp != uLast )
			{
				Com::printF( PSTR( "clearExternalEEPROM(): " ), (int)i );
				Com::printFLN( PSTR( " / " ), (long)uMax );
				uLast = uTemp;
			}
		}
	}

	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "clearExternalEEPROM(): erasing complete" ) );
	}
	return 0;

} // clearExternalEEPROM


void writeByte24C256( int addressI2C, unsigned int addressEEPROM, unsigned char data )
{
	HAL::delayMilliseconds( EEPROM_DELAY );
    Wire.beginTransmission( addressI2C );
    Wire.write( int(addressEEPROM >> 8));		// MSB
    Wire.write( int(addressEEPROM & 0xFF));		// LSB
    Wire.write( data );
    Wire.endTransmission();
	return;
    
} // writeByte24C256


void writeWord24C256( int addressI2C, unsigned int addressEEPROM, unsigned short data )
{
	unsigned short	Temp;


	Temp = byte(data >> 8);
	writeByte24C256( addressI2C, addressEEPROM, Temp );
	Temp = byte(data & 0x00FF);
	writeByte24C256( addressI2C, addressEEPROM+1, Temp );
	return;

} // writeWord24C256


unsigned char readByte24C256( int addressI2C, unsigned int addressEEPROM )
{
	HAL::delayMilliseconds( EEPROM_DELAY );
    Wire.beginTransmission( addressI2C );
    Wire.write( int(addressEEPROM >> 8));		// MSB
    Wire.write( int(addressEEPROM & 0xFF));		// LSB
    Wire.endTransmission();
    Wire.requestFrom( addressI2C, 1 );
    
    return Wire.read();
    
} // readByte24C256


unsigned short readWord24C256( int addressI2C, unsigned int addressEEPROM )
{
	unsigned short	data;
	byte			Temp;


	Temp = readByte24C256( addressI2C, addressEEPROM );
	data = Temp;
	data = data << 8;
	Temp = readByte24C256( addressI2C, addressEEPROM+1 );

	return data + Temp;

} // readWord24C256


void doZCompensation( void )
{
#if FEATURE_CNC_MODE == 2

	if( Printer::operatingMode == OPERATING_MODE_PRINT )
	{
#if FEATURE_HEAT_BED_Z_COMPENSATION
		doHeatBedZCompensation();
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
	}
	else
	{
#if FEATURE_WORK_PART_Z_COMPENSATION
		doWorkPartZCompensation();
#endif // FEATURE_WORK_PART_Z_COMPENSATION
	}

#else

#if FEATURE_HEAT_BED_Z_COMPENSATION
	doHeatBedZCompensation();
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#endif // FEATURE_CNC_MODE == 2

} // doZCompensation


void loopRF1000( void )
{
	static char		nEntered = 0;
	unsigned long	uTime;
	short			nPressure;
	
	
#if FEATURE_WATCHDOG
    HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

	if( nEntered )
	{
		// do not enter more than once
		return;
	}
	nEntered ++;

	uTime = HAL::timeInMilliseconds();

#if defined(CASE_FAN_PIN) && CASE_FAN_PIN >= 0
	if( Printer::prepareFanOff )
	{
		if( (uTime - Printer::prepareFanOff) > Printer::fanOffDelay )
		{
			// it is time to turn the case fan off
			Printer::prepareFanOff = 0;
			WRITE( CASE_FAN_PIN, 0 );
		}
	}
#endif // defined(CASE_FAN_PIN) && CASE_FAN_PIN >= 0

#if FEATURE_CNC_MODE == 2

	if( Printer::operatingMode == OPERATING_MODE_PRINT )
	{
#if FEATURE_HEAT_BED_Z_COMPENSATION
		if( g_nHeatBedScanStatus )
		{
			scanHeatBed();
		}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
	}
	else
	{
#if FEATURE_WORK_PART_Z_COMPENSATION
		if( g_nWorkPartScanStatus )
		{
			scanWorkPart();
		}
#endif // FEATURE_WORK_PART_Z_COMPENSATION
	}

#else

#if FEATURE_HEAT_BED_Z_COMPENSATION
	if( g_nHeatBedScanStatus )
	{
		scanHeatBed();
	}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#endif // FEATURE_CNC_MODE == 2

#if FEATURE_FIND_Z_ORIGIN
	if( g_nFindZOriginStatus )
	{
		findZOrigin();
	}
#endif // FEATURE_FIND_Z_ORIGIN

#if FEATURE_PAUSE_PRINTING
	if( g_uPauseTime )
	{
		if( !g_pauseBeepDone )
		{
			BEEP_PAUSE
			g_pauseBeepDone = 1;
		}

		if( g_pausePrint )
		{
#if EXTRUDER_CURRENT_PAUSE_DELAY
			if( (uTime - g_uPauseTime) > EXTRUDER_CURRENT_PAUSE_DELAY )
			{
				char	nProcessExtruder = 0;


#if FEATURE_CNC_MODE > 0
				if( Printer::operatingMode == OPERATING_MODE_PRINT )
				{
					// process the extruder only in case we are in mode "print"
					nProcessExtruder = 1;
				}
#else
				nProcessExtruder = 1;
#endif // FEATURE_CNC_MODE > 0

				if( nProcessExtruder )
				{
					// we have paused a few moments ago - reduce the current of the extruder motor in order to avoid unwanted heating of the filament for use cases where the printing is paused for several minutes
/*					Com::printF( PSTR( "loopRF1000(): PauseTime = " ), g_uPauseTime );
					Com::printF( PSTR( ", Time = " ), uTime );
					Com::printFLN( PSTR( ", Diff = " ), uTime - g_uPauseTime );
*/
					setExtruderCurrent( EXTRUDER_CURRENT_PAUSED );
				}
				g_uPauseTime = 0;
			}
#endif // EXTRUDER_CURRENT_PAUSE_DELAY
		}
		else
		{
			// we are not paused any more
			g_uPauseTime = 0;
		}
	}
#endif // FEATURE_PAUSE_PRINTING

#if FEATURE_EMERGENCY_PAUSE
	if( (uTime - g_uLastPressureTime) > EMERGENCY_PAUSE_INTERVAL )
	{
		g_uLastPressureTime = uTime;

		if( g_pausePrint == 0 && PrintLine::linesCount > 5 )
		{
			// this check shall be done only during the printing (for example, it shall not be done in case filament is extruded manually)
			g_nPressureSum	  += readStrainGauge( SCAN_STRAIN_GAUGE );
			g_nPressureChecks += 1;

			if( g_nPressureChecks == EMERGENCY_PAUSE_CHECKS )
			{
				nPressure		 = g_nPressureSum / g_nPressureChecks;

//				Com::printF( PSTR( "loopRF1000(): average = " ), nPressure );
//				Com::printFLN( PSTR( " / " ), g_nPressureChecks );

				g_nPressureSum	  = 0;
				g_nPressureChecks = 0;

				if( (nPressure < EMERGENCY_PAUSE_DIGITS_MIN) ||
					(nPressure > EMERGENCY_PAUSE_DIGITS_MAX) )
				{
					// the pressure is outside the allowed range, we must perform the emergency pause
					if( Printer::debugErrors() )
					{
						Com::printF( PSTR( "loopRF1000(): emergency pause: " ), nPressure );
						Com::printFLN( PSTR( " / " ), PrintLine::linesCount );
					}

					pausePrint();
				}
			}
		}
		else
		{
			g_nPressureSum	  = 0;
			g_nPressureChecks = 0;
		}
	}
#endif // FEATURE_EMERGENCY_PAUSE

#if FEATURE_EMERGENCY_Z_STOP
	if( (uTime - g_uLastZPressureTime) > EMERGENCY_Z_STOP_INTERVAL )
	{
		g_uLastZPressureTime = uTime;

		if( g_nMainDirectionZ && !g_nMainDirectionE )
		{
			// this check shall be done only when there is some moving into z-direction in progress and the extruder is not doing anything
			g_nZPressureSum	   += readStrainGauge( SCAN_STRAIN_GAUGE );
			g_nZPressureChecks += 1;

			if( g_nZPressureChecks == EMERGENCY_Z_STOP_CHECKS )
			{
				nPressure		 = g_nZPressureSum / g_nZPressureChecks;

//				Com::printF( PSTR( "loopRF1000(): average = " ), nPressure );
//				Com::printFLN( PSTR( " / " ), g_nZPressureChecks );

				g_nZPressureSum	   = 0;
				g_nZPressureChecks = 0;

				if( (nPressure < EMERGENCY_Z_STOP_DIGITS_MIN) ||
					(nPressure > EMERGENCY_Z_STOP_DIGITS_MAX) )
				{
					// the pressure is outside the allowed range, we must perform the emergency z-stop
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "loopRF1000(): emergency z-stop: " ), nPressure );
					}

					// block any further movement into the z-direction
					g_nBlockZ		  = 1;
					g_nMainDirectionZ = 0;
				}
			}
		}
		else
		{
			g_nZPressureSum	   = 0;
			g_nZPressureChecks = 0;
		}
	}
#endif // FEATURE_EMERGENCY_Z_STOP

	if( g_uStopTime )
	{
		if( (uTime - g_uStopTime) > CLEAN_UP_DELAY_AFTER_STOP_PRINT )
		{
			// we have stopped the printing a few moments ago, output the object now

			if( PrintLine::linesCount )
			{
				// wait until all moves are done
				g_uStopTime = uTime;
			}
			else
			{
				// there is no printing in progress any more, do all clean-up now
				g_uStopTime	= 0;

				// disable all heaters
				Extruder::setHeatedBedTemperature( 0, false );
				Extruder::setTemperatureForExtruder( 0, 0, false );

#if FEATURE_OUTPUT_PRINTED_OBJECT
				// output the object
				outputObject();
#else
				// disable all steppers
				Printer::setAllSteppersDisabled();
				Printer::disableXStepper();
				Printer::disableYStepper();
				Printer::disableZStepper();
				Extruder::disableCurrentExtruderMotor();
#endif // FEATURE_OUTPUT_PRINTED_OBJECT

#if FAN_PIN>-1
				// disable the fan
				Commands::setFanSpeed(0,false);
#endif // FAN_PIN>-1

				uTime = millis();
				g_uBlockCommands = uTime;
			}
		}
	}

	if( g_uBlockCommands )
	{
		if( (uTime - g_uBlockCommands) > COMMAND_BLOCK_DELAY )
		{
			g_uBlockCommands = 0;
		}
	}
	
#if FEATURE_ABORT_PRINT_AFTER_TEMPERATURE_ERROR
	if( Printer::isAnyTempsensorDefect() && sd.sdmode && PrintLine::linesCount )
	{
		// we are printing from the SD card and a temperature sensor got defect - abort the current printing
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "loopRF1000(): aborting print because of a temperature sensor defect" ) );
		}

		sd.abortPrint();
	}
#endif // FEATURE_ABORT_PRINT_AFTER_TEMPERATURE_ERROR

	if( (uTime - g_lastTime) > LOOP_INTERVAL )
	{
		// it is time for another processing
		g_lastTime = uTime;
/*
#if FEATURE_CNC_MODE == 2
		Com::printF( PSTR( "endstopZMinHit = " ), Printer::endstopZMinHit );
		Com::printF( PSTR( " / endstopZMaxHit = " ), Printer::endstopZMaxHit );
		Com::printF( PSTR( " / stepsSinceZMinEndstop = " ), Printer::stepsSinceZMinEndstop );
		Com::printFLN( PSTR( " / stepsSinceZMaxEndstop = " ), Printer::stepsSinceZMaxEndstop );
#endif // FEATURE_CNC_MODE == 2
*/
#if FEATURE_HEAT_BED_Z_COMPENSATION
//		if( g_debugLevel && Printer::debugInfo() )
		if( g_debugLevel && Printer::debugInfo() && PrintLine::linesCount )
		{
#if DEBUG_HEAT_BED_Z_COMPENSATION
			switch( g_debugLevel )
			{
				case 1:
				{
					Com::printF( PSTR( "tcZ;" ), Printer::targetCompensationZ );
					Com::printF( PSTR( ";ccZ;" ), Printer::currentCompensationZ );
					break;
				}
				case 2:
				{
					Com::printF( PSTR( "tpsZ;" ), Printer::targetPositionStepsZ );
					Com::printF( PSTR( ";cpsZ;" ), Printer::currentPositionStepsZ );
					break;
				}
				case 4:
				{
					Com::printF( PSTR( "tpsX;" ), Printer::targetPositionStepsX );
					Com::printF( PSTR( ";cpsX;" ), Printer::currentPositionStepsX );
					Com::printF( PSTR( ";tpsY;" ), Printer::targetPositionStepsY );
					Com::printF( PSTR( ";cpsY;" ), Printer::currentPositionStepsY );
					Com::printF( PSTR( ";tpsZ;" ), Printer::targetPositionStepsZ );
					Com::printF( PSTR( ";cpsZ;" ), Printer::currentPositionStepsZ );
					Com::printF( PSTR( ";tpsE;" ), Printer::targetPositionStepsE );
					Com::printF( PSTR( ";cpsE;" ), Printer::currentPositionStepsE );
					break;
				}
				case 5:
				{
					Com::printF( PSTR( "nCPS X;" ), Printer::nonCompensatedPositionStepsX );
					Com::printF( PSTR( ";nCPS Y;" ), Printer::nonCompensatedPositionStepsY );
					Com::printF( PSTR( ";nCPS Z;" ), Printer::nonCompensatedPositionStepsZ );
					Com::printF( PSTR( ";t Z;" ), Printer::targetCompensationZ );
					Com::printF( PSTR( ";c Z;" ), Printer::currentCompensationZ );
					Com::printF( PSTR( ";tPS Z;" ), Printer::targetPositionStepsZ );
					Com::printF( PSTR( ";cPS Z;" ), Printer::currentPositionStepsZ );
					Com::printF( PSTR( ";DirZ;" ), g_nMainDirectionZ );
					break;
				}
				case 6:
				{
					Com::printF( PSTR( "nCPS X;" ), Printer::nonCompensatedPositionStepsX );
					Com::printF( PSTR( "; nCPS Y;" ), Printer::nonCompensatedPositionStepsY );
					Com::printF( PSTR( "; nCPS Z;" ), Printer::nonCompensatedPositionStepsZ );
					Com::printF( PSTR( "; MDZ;" ), g_nMainDirectionZ );
					Com::printF( PSTR( "; tCZ;" ), Printer::targetCompensationZ );
					Com::printF( PSTR( "; cCZ;" ), Printer::currentCompensationZ );
					Com::printF( PSTR( "; tPSZ;" ), Printer::targetPositionStepsZ );
					Com::printF( PSTR( "; cPSZ;" ), Printer::currentPositionStepsZ );
					Com::printF( PSTR( "; dZ;" ), Printer::destinationSteps[Z_AXIS] );
					Com::printF( PSTR( "; cZ;" ), Printer::currentPositionSteps[Z_AXIS] );
					Com::printF( PSTR( "; Int32;" ), g_debugInt32 );
					Com::printF( PSTR( "; Int16;" ), g_debugInt16 );
					Com::printF( PSTR( "; RAM;" ), Commands::lowestRAMValue );
					Com::printF( PSTR( "; doZC;" ), Printer::doHeatBedZCompensation );
					break;
				}
				case 10:
				{
					Com::printF( PSTR( "Start;" ), g_nZCompensationUpdates );
					Com::printF( PSTR( "; nCPS X;" ), Printer::nonCompensatedPositionStepsX );
					Com::printF( PSTR( ";" ), g_nNonCompensatedPositionSteps[X_AXIS] );
					Com::printF( PSTR( "; nCPS Y;" ), Printer::nonCompensatedPositionStepsY );
					Com::printF( PSTR( ";" ), g_nNonCompensatedPositionSteps[Y_AXIS] );
					Com::printF( PSTR( "; nCPS Z;" ), Printer::nonCompensatedPositionStepsZ );
					Com::printF( PSTR( ";" ), g_nNonCompensatedPositionSteps[Z_AXIS] );
					Com::printF( PSTR( "; tCZ;" ), Printer::targetCompensationZ );
					Com::printF( PSTR( "; cCZ;" ), Printer::currentCompensationZ );
					Com::printF( PSTR( "; tPSZ;" ), Printer::targetPositionStepsZ );
					Com::printF( PSTR( "; cPSZ;" ), Printer::currentPositionStepsZ );
					Com::printF( PSTR( "; dZ;" ), Printer::destinationSteps[Z_AXIS] );
					Com::printF( PSTR( "; cZ;" ), Printer::currentPositionSteps[Z_AXIS] );
					Com::printF( PSTR( "; doZC;" ), Printer::doHeatBedZCompensation );
					Com::printF( PSTR( "; Delta;" ), g_nDelta[X_AXIS] );
					Com::printF( PSTR( ";" ), g_nDelta[Y_AXIS] );
					Com::printF( PSTR( "; StepSize;" ), g_nStepSize[X_AXIS] );
					Com::printF( PSTR( ";" ), g_nStepSize[Y_AXIS] );
					Com::printF( PSTR( "; TempX;" ), g_nTempXFront );
					Com::printF( PSTR( ";" ), g_nTempXBack );
					Com::printF( PSTR( "; NeededZ;" ), g_nNeededZ );
					Com::printF( PSTR( "; Index;" ), g_uIndex[0] );
					Com::printF( PSTR( ";" ), g_uIndex[1] );
					Com::printF( PSTR( ";" ), g_uIndex[2] );
					Com::printF( PSTR( ";" ), g_uIndex[3] );
					Com::printF( PSTR( "; Matrix;" ), g_nMatrix[0] );
					Com::printF( PSTR( ";" ), g_nMatrix[1] );
					Com::printF( PSTR( ";" ), g_nMatrix[2] );
					Com::printF( PSTR( ";" ), g_nMatrix[3] );
					Com::printF( PSTR( "; Z Delta Min;" ), g_nZDeltaMin );
					Com::printF( PSTR( "; Z Delta Max;" ), g_nZDeltaMax );
					Com::printF( PSTR( "; Z Delay Max;" ), g_nZCompensationDelayMax );
					Com::printF( PSTR( "; Too fast;" ), g_nTooFast );
					Com::printF( PSTR( "; End;" ), g_nZCompensationUpdates );

					g_nZDeltaMin			 = 100000;
					g_nZDeltaMax			 = -100000;
					g_nZCompensationDelayMax = 0;
					g_nTooFast				 = 0;
					break;
				}

				default:
				{
					Com::printF( PSTR( "unsupported debug level: " ), g_debugLevel );
					break;
				}
			}
			Com::printFLN( PSTR( " " ) );

#endif // DEBUG_HEAT_BED_Z_COMPENSATION

			// NOTE: there is no need to turn off the z compensation automatically
/*			if( Printer::nonCompensatedPositionStepsZ > g_maxZCompensationSteps &&
				Printer::targetCompensationZ == Printer::currentCompensationZ )
			{
				// turn off the z compensation in case we are far away from the surface
				Printer::doHeatBedZCompensation = 0;

				// NOTE: at this place we have to continue with the constant offset in z direction - do not set the target correction to 0
			    //Printer::targetCompensationZ = 0;
				Com::printFLN( PSTR( "loopRF1000(): The z compensation has been disabled (z)." ) );
			}
*/		}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
		if( g_debugLevel && Printer::debugInfo() && PrintLine::linesCount )
		{
#if DEBUG_WORK_PART_Z_COMPENSATION
			switch( g_debugLevel )
			{
				case 1:
				{
					Com::printF( PSTR( "tcZ;" ), Printer::targetCompensationZ );
					Com::printF( PSTR( ";ccZ;" ), Printer::currentCompensationZ );
					break;
				}
				case 2:
				{
					Com::printF( PSTR( "tpsZ;" ), Printer::targetPositionStepsZ );
					Com::printF( PSTR( ";cpsZ;" ), Printer::currentPositionStepsZ );
					break;
				}
				case 4:
				{
					Com::printF( PSTR( "tpsX;" ), Printer::targetPositionStepsX );
					Com::printF( PSTR( ";cpsX;" ), Printer::currentPositionStepsX );
					Com::printF( PSTR( ";tpsY;" ), Printer::targetPositionStepsY );
					Com::printF( PSTR( ";cpsY;" ), Printer::currentPositionStepsY );
					Com::printF( PSTR( ";tpsZ;" ), Printer::targetPositionStepsZ );
					Com::printF( PSTR( ";cpsZ;" ), Printer::currentPositionStepsZ );
					Com::printF( PSTR( ";tpsE;" ), Printer::targetPositionStepsE );
					Com::printF( PSTR( ";cpsE;" ), Printer::currentPositionStepsE );
					break;
				}
				case 5:
				{
					Com::printF( PSTR( "X;" ), Printer::nonCompensatedPositionStepsX );
					Com::printF( PSTR( "; Y;" ), Printer::nonCompensatedPositionStepsY );
					Com::printF( PSTR( "; Z;" ), Printer::nonCompensatedPositionStepsZ );
					Com::printF( PSTR( "; tZ;" ), Printer::targetCompensationZ );
					Com::printF( PSTR( "; cZ;" ), Printer::currentCompensationZ );
					break;
				}
				case 6:
				{
					Com::printF( PSTR( "nCPS X;" ), Printer::nonCompensatedPositionStepsX );
					Com::printF( PSTR( "; nCPS Y;" ), Printer::nonCompensatedPositionStepsY );
					Com::printF( PSTR( "; nCPS Z;" ), Printer::nonCompensatedPositionStepsZ );
					Com::printF( PSTR( "; tCZ;" ), Printer::targetCompensationZ );
					Com::printF( PSTR( "; cCZ;" ), Printer::currentCompensationZ );
					Com::printF( PSTR( "; tPSZ;" ), Printer::targetPositionStepsZ );
					Com::printF( PSTR( "; cPSZ;" ), Printer::currentPositionStepsZ );
					Com::printF( PSTR( "; dZ;" ), Printer::destinationSteps[Z_AXIS] );
					Com::printF( PSTR( "; cZ;" ), Printer::currentPositionSteps[Z_AXIS] );
					Com::printF( PSTR( "; Int32;" ), g_debugInt32 );
					Com::printF( PSTR( "; RAM;" ), Commands::lowestRAMValue );
					break;
				}

				default:
				{
					Com::printF( PSTR( "unsupported debug level: " ), g_debugLevel );
					break;
				}
			}
			Com::printFLN( PSTR( " " ) );
#endif // DEBUG_WORK_PART_Z_COMPENSATION
		}
#endif // FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_FIND_Z_ORIGIN
		if( g_debugLevel && Printer::debugInfo() && g_nFindZOriginStatus )
		{
#if DEBUG_FIND_Z_ORIGIN
			switch( g_debugLevel )
			{
				case 5:
				{
					Com::printF( PSTR( "OriginZ;" ), g_nZOriginZPosition );
					Com::printF( PSTR( "; Dir;" ), READ( Z_DIR_PIN ) );
					Com::printF( PSTR( "; TempDir;" ), g_nTempDirectionZ );
					Com::printF( PSTR( "; Status;" ), g_nFindZOriginStatus );
					//Com::printF( PSTR( "; Pressure;" ), g_debugInt16 );
					break;
				}

				default:
				{
					Com::printF( PSTR( "unsupported debug level: " ), g_debugLevel );
					break;
				}
			}
			Com::printFLN( PSTR( " " ) );
#endif // DEBUG_FIND_Z_ORIGIN
		}
#endif // FEATURE_FIND_Z_ORIGIN

/*		Com::printF( PSTR( "currentCompensationZ;" ), Printer::currentCompensationZ );
		Com::printF( PSTR( ";g_nZScanZPosition;" ), g_nZScanZPosition );
		Com::printF( PSTR( ";g_nZOriginZPosition;" ), g_nZOriginZPosition );
		Com::printF( PSTR( ";currentPositionStepsZ;" ), Printer::currentPositionStepsZ );
		Com::printF( PSTR( ";realZPosition();" ), Printer::realZPosition() );
*/	}

	if( g_debugLog )
	{
		switch( g_debugLog )
		{
#if FEATURE_FIND_Z_ORIGIN

			case 1:
			{
				Com::printF( PSTR( "Z-Origin X: " ), g_nZOriginXPosition );
				Com::printF( PSTR( "; Z-Origin Y: " ), g_nZOriginYPosition );
				Com::printFLN( PSTR( "; Z-Origin Z: " ), Printer::staticCompensationZ );
				break;
			}

#endif // FEATURE_FIND_Z_ORIGIN
		}

		g_debugLog = 0;
	}

	nEntered --;
	return;

} // loopRF1000


#if FEATURE_OUTPUT_PRINTED_OBJECT
void outputObject( void )
{
	if( PrintLine::linesCount )
	{
		// there is some printing in progress at the moment - do not park the printer in this case
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "outputObject(): the object can not be output while the printing is in progress" ) );
		}
		return;
	}

	if( !Printer::isHomed() )
	{
		// the printer does not know its home position, thus we can not output the object
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "outputObject(): the object can not be output because the home position is unknown" ) );
		}
		return;
	}

	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "outputObject()" ) );
	}

#if FAN_PIN>-1
	// disable the fan
    Commands::setFanSpeed(0,false);
#endif // FAN_PIN>-1

	Commands::printCurrentPosition();

#if FEATURE_CNC_MODE > 0
	if( Printer::operatingMode == OPERATING_MODE_CNC )
	{
		GCode::executeFString(Com::tOutputObjectMill);
	}
	else
	{
		GCode::executeFString(Com::tOutputObjectPrint);
	}
#else
	GCode::executeFString(Com::tOutputObjectPrint);
#endif // FEATURE_CNC_MODE > 0


	Commands::waitUntilEndOfAllMoves();
    Commands::printCurrentPosition();
	
	// disable all steppers
	Printer::setAllSteppersDisabled();
	Printer::disableXStepper();
	Printer::disableYStepper();
	Printer::disableZStepper();
	Extruder::disableCurrentExtruderMotor();

} // outputObject
#endif // FEATURE_OUTPUT_PRINTED_OBJECT

#if FEATURE_PARK
void parkPrinter( void )
{
	if( PrintLine::linesCount )
	{
		// there is some printing in progress at the moment - do not park the printer in this case
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "parkPrinter(): the printer can not be parked while the printing is in progress" ) );
		}
		return;
	}

	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "parkPrinter()" ) );
	}

	Printer::homeAxis( true, true, true );

	Printer::moveToReal( g_nParkPositionX, g_nParkPositionY, g_nParkPositionZ, IGNORE_COORDINATE, Printer::homingFeedrate[0]);

} // parkPrinter
#endif // FEATURE_PARK


#if FEATURE_PAUSE_PRINTING
void pausePrint( void )
{
	long	Temp;


	if( Printer::debugErrors() )
	{
		Com::printFLN( PSTR( "pausePrint()" ) );
	}

	if( g_pausePrint == 0 )
	{
		// the printing is not paused at the moment
		if( !Printer::isHomed() )
		{
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "pausePrint(): pause is not available at the moment because the home position is unknown" ) );
			}
			return;
		}

		if( PrintLine::linesCount )
		{
			g_pausePrint = 1;

			// wait until the current move is completed
			while( !g_printingPaused )
			{
#if FEATURE_WATCHDOG
				HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

				HAL::delayMilliseconds( 1 );
				Commands::checkForPeriodicalActions();
			}

			g_uPauseTime	= HAL::timeInMilliseconds();
			g_pauseBeepDone	= 0;

			if( Printer::debugInfo() )
			{
				Com::printFLN( PSTR( "pausePrint(): the printing has been paused" ) );
			}
		    UI_STATUS(UI_TEXT_PAUSED);
		    Printer::setMenuMode(MENU_MODE_SD_PAUSED,true);

			g_nContinueStepsX = 0;
			g_nContinueStepsY = 0;
			g_nContinueStepsZ = 0;

			if( g_nPauseStepsExtruder )
			{
				Printer::targetPositionStepsE -= g_nPauseStepsExtruder;
				g_nContinueStepsExtruder	  =  g_nPauseStepsExtruder;
			}
		}
		else
		{
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "pausePrint(): pause is not available at the moment because nothing is printed" ) );
			}
		}
		return;
	}

	if( g_pausePrint == 1 )
	{
#if FEATURE_CNC_MODE > 0
		if( Printer::operatingMode != OPERATING_MODE_PRINT )
		{
			// for now, we do not allow to move away from the pause position in case the operating mode is not print
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "pausePrint(): moving to the pause position is not supported in this mode" ) );
			}
			return;
		}
#endif // FEATURE_CNC_MODE > 0

		// in case the print is paused already, we move the printer head to the pause position
		g_pausePrint = 2;

		determinePausePosition();

		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "pausePrint(): moving to the pause position" ) );

			Com::printF( PSTR( "x;" ), Printer::targetPositionStepsX );
			Com::printF( PSTR( ";y;" ), Printer::targetPositionStepsY );
			Com::printFLN( PSTR( ";z;" ), Printer::targetPositionStepsZ );
		}

		// wait until the pause position has been reached
		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "pausePrint(): waiting for the pause position" ) );
		}

		while( (Printer::targetPositionStepsX != Printer::currentPositionStepsX) ||
			   (Printer::targetPositionStepsY != Printer::currentPositionStepsY) ||
			   (Printer::targetPositionStepsZ != Printer::currentPositionStepsZ) ||
			   (Printer::targetPositionStepsE != Printer::currentPositionStepsE) )
		{
#if FEATURE_WATCHDOG
			HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

			HAL::delayMilliseconds( 1 );
			Commands::checkForPeriodicalActions();

			// NOTE: do not run runStandardTasks() within this loop
			//runStandardTasks();
		}

		if( Printer::debugInfo() )
		{
			Com::printF( PSTR( "g_nPauseStepsX = " ), g_nPauseStepsX );
			Com::printF( PSTR( ", g_nPauseStepsY = " ), g_nPauseStepsX );
			Com::printFLN( PSTR( ", g_nPauseStepsZ = " ), g_nPauseStepsX );
			Com::printFLN( PSTR( "pausePrint(): the pause position has been reached" ) );
		}
		return;
	}

#if FEATURE_EMERGENCY_STOP_VIA_PAUSE
	if( g_pausePrint == 2 )
	{
		// in case the print is paused and the extruder is moved away already, we kill the printing
		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "pausePrint(): emergency stop" ) );
		}
		HAL::delayMilliseconds( 100 );
		Commands::emergencyStop();
		return;
	}
#endif // FEATURE_EMERGENCY_STOP_VIA_PAUSE

	return;

} // pausePrint


void continuePrint( void )
{
	const unsigned short	uMotorCurrents[] = MOTOR_CURRENT;
	char					nProcessExtruder = 0;


	if( g_pausePrint )
	{
		BEEP_CONTINUE

		if( g_pausePrint == 2 )
		{
#if FEATURE_CNC_MODE > 0
			if( Printer::operatingMode == OPERATING_MODE_PRINT )
			{
				// process the extruder only in case we are in mode "print"
				nProcessExtruder = 1;
			}
#else
			nProcessExtruder = 1;
#endif // FEATURE_CNC_MODE > 0

			// move to the continue position
			if( Printer::debugInfo() )
			{
				Com::printFLN( PSTR( "continuePrint(): moving to the continue position" ) );
			}

#if EXTRUDER_CURRENT_PAUSE_DELAY
			if( nProcessExtruder )
			{
				setExtruderCurrent( uMotorCurrents[E_AXIS] );
			}
#endif // EXTRUDER_CURRENT_PAUSE_DELAY

			HAL::forbidInterrupts();
			if( g_nContinueStepsY )				Printer::targetPositionStepsY += g_nContinueStepsY;
			if( g_nContinueStepsX )				Printer::targetPositionStepsX += g_nContinueStepsX;
			if( g_nContinueStepsZ )				Printer::targetPositionStepsZ += g_nContinueStepsZ;

			if( nProcessExtruder )
			{
				if( g_nContinueStepsExtruder )	Printer::targetPositionStepsE += g_nContinueStepsExtruder;
			}

			HAL::allowInterrupts();

			// wait until the continue position has been reached
			if( Printer::debugInfo() )
			{
				Com::printFLN( PSTR( "continuePrint(): waiting for the continue position" ) );
			}

			while( (Printer::targetPositionStepsX != Printer::currentPositionStepsX) ||
				   (Printer::targetPositionStepsY != Printer::currentPositionStepsY) ||
				   (Printer::targetPositionStepsZ != Printer::currentPositionStepsZ) ||
				   (Printer::targetPositionStepsE != Printer::currentPositionStepsE) )
			{
#if FEATURE_WATCHDOG
				HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

				HAL::delayMilliseconds( 1 );
				Commands::checkForPeriodicalActions();

				// NOTE: do not run runStandardTasks() within this loop
				//runStandardTasks();
			}
		}
		else if( g_pausePrint == 1 )
		{
#if FEATURE_CNC_MODE > 0
			if( Printer::operatingMode == OPERATING_MODE_PRINT )
			{
				// process the extruder only in case we are in mode "print"
				nProcessExtruder = 1;
			}
#else
			nProcessExtruder = 1;
#endif // FEATURE_CNC_MODE > 0

			if( nProcessExtruder )
			{
#if EXTRUDER_CURRENT_PAUSE_DELAY
				setExtruderCurrent( uMotorCurrents[E_AXIS] );
#endif // EXTRUDER_CURRENT_PAUSE_DELAY

				if( g_nContinueStepsExtruder )	Printer::targetPositionStepsE += g_nContinueStepsExtruder;

				// wait until the continue position has been reached
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "continuePrint(): waiting for the continue position" ) );
				}

				while( Printer::targetPositionStepsE != Printer::currentPositionStepsE )
				{
#if FEATURE_WATCHDOG
					HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

					HAL::delayMilliseconds( 1 );
					Commands::checkForPeriodicalActions();

					// NOTE: do not run runStandardTasks() within this loop
					//runStandardTasks();
				}
			}
		}

		// wait until the next move is started
		g_pausePrint = 0;
		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "continuePrint(): waiting for the next move" ) );
		}
		while( g_printingPaused )
		{
#if FEATURE_WATCHDOG
				HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

			if( !PrintLine::linesCount )
			{
				// the printing won't continue in case there is nothing else to do
				break;
			}
			HAL::delayMilliseconds( 1 );
			Commands::checkForPeriodicalActions();

			// NOTE: do not run runStandardTasks() within this loop
			//runStandardTasks();
		}
		g_preparePause = 0;

		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "continuePrint(): the printing has been continued" ) );
		}
		UI_STATUS(UI_TEXT_PRINTING);
	    Printer::setMenuMode(MENU_MODE_SD_PAUSED,false);
	}
	else
	{
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "continuePrint(): continue is not available at the moment" ) );
		}
	}
	return;

} // continuePrint


void determinePausePosition( void )
{
	long	Temp;


	if( g_nPauseStepsZ )
    {
		Temp = g_nPauseStepsZ;

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
		Temp += Printer::nonCompensatedPositionStepsZ;
		Temp += Printer::currentCompensationZ;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

		Temp += Printer::targetPositionStepsZ;

        if( Temp <= PAUSE_Z_MAX )
        {
            Printer::targetPositionStepsZ += g_nPauseStepsZ;
            g_nContinueStepsZ			  =  -g_nPauseStepsZ;
        }
		else
		{
			// we can move only partially
			Temp = PAUSE_Z_MAX - Printer::targetPositionStepsZ;
			
#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
			Temp -= Printer::nonCompensatedPositionStepsZ;
			Temp -= Printer::currentCompensationZ;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

			Printer::targetPositionStepsZ += Temp;
            g_nContinueStepsZ			  =  -Temp;
		}
    }
    if( g_nPauseStepsX )
    {
		Temp = g_nPauseStepsX;

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
		Temp += Printer::nonCompensatedPositionStepsX;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

		Temp += Printer::targetPositionStepsX;

        if( g_nPauseStepsX < 0 )
		{
			if( Temp < PAUSE_X_MIN )
			{
				// we can move only partially
				Temp = PAUSE_X_MIN - Printer::targetPositionStepsX;

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
				Temp -= Printer::nonCompensatedPositionStepsX;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

				Printer::targetPositionStepsX += Temp;
				g_nContinueStepsX			  =  -Temp;
			}
			else
			{
				Printer::targetPositionStepsX += g_nPauseStepsX;
				g_nContinueStepsX			  =  -g_nPauseStepsX;
			}
		}
        else if( g_nPauseStepsX > 0 )
		{
			if( Temp > PAUSE_X_MAX )
			{
				// we can move only partially
				Temp = PAUSE_X_MAX - Printer::targetPositionStepsX;

#if FEATURE_HEAT_BED_Z_COMPENSATION  || FEATURE_WORK_PART_Z_COMPENSATION
				Temp -= Printer::nonCompensatedPositionStepsX;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

				Printer::targetPositionStepsX += Temp;
				g_nContinueStepsX			  =  -Temp;
			}
			else
			{
				Printer::targetPositionStepsX += g_nPauseStepsX;
				g_nContinueStepsX			  =  -g_nPauseStepsX;
			}
		}
    }
    if( g_nPauseStepsY )
    {
		Temp = g_nPauseStepsY;

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
		Temp += Printer::nonCompensatedPositionStepsY;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

		Temp += Printer::targetPositionStepsY;

		if( g_nPauseStepsY < 0 )
		{
			if( Temp < PAUSE_Y_MIN )
			{
				// we can move only partially
				Temp = PAUSE_Y_MIN - Printer::targetPositionStepsY;

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
				Temp -= Printer::nonCompensatedPositionStepsY;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

				Printer::targetPositionStepsY += Temp;
				g_nContinueStepsY			  =  -Temp;
			}
			else
			{
				Printer::targetPositionStepsY += g_nPauseStepsY;
				g_nContinueStepsY			  =  -g_nPauseStepsY;
			}
		}
        else if( g_nPauseStepsY > 0 )
		{
			if( Temp > PAUSE_Y_MAX )
			{
				// we can move only partially
				Temp = PAUSE_Y_MAX - Printer::targetPositionStepsY;

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
				Temp -= Printer::nonCompensatedPositionStepsY;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

				Printer::targetPositionStepsY += Temp;
				g_nContinueStepsY			  =  -Temp;
			}
			else
			{
				Printer::targetPositionStepsY += g_nPauseStepsY;
				g_nContinueStepsY			  =  -g_nPauseStepsY;
			}
		}
    }
	return;

} // determinePausePosition


void waitUntilContinue( void )
{
	char	bWait = 0;


	if( g_preparePause )			bWait = 1;
	if( g_printingPaused )			bWait = 1;

	while( bWait )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

		GCode::readFromSerial();
        Commands::checkForPeriodicalActions();
        UI_MEDIUM;

		bWait = 0;
		if( g_preparePause )			bWait = 1;
		if( g_printingPaused )			bWait = 1;
	}

} // waitUntilContinue
#endif // FEATURE_PAUSE_PRINTING


void setExtruderCurrent( unsigned short level )
{
#if FEATURE_CNC_MODE > 0
	if( Printer::operatingMode != OPERATING_MODE_PRINT )
	{
		// we have no extruder when we are not in print mode
		return;
	}
#endif // FEATURE_CNC_MODE > 0

	// set the current for the extruder motor
	setMotorCurrent( 4, level );

	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "setExtruderCurrent(): new extruder current level: " ), (unsigned long)level );
	}
	return;

} // setExtruderCurrent


void processCommand( GCode* pCommand )
{
  	long	nTemp;


	if( pCommand->hasM() )
	{
		switch( pCommand->M )
		{
#if FEATURE_HEAT_BED_Z_COMPENSATION
			case 3000: // M3000 - turn the z-compensation off
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "M3000: disabling z compensation" ) );
					}
					queueTask( TASK_DISABLE_Z_COMPENSATION );
				}
				break;
			}
			case 3001: // M3001 - turn the z-compensation on
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( Printer::isHomed() )
					{
						if( g_ZCompensationMatrix[0][0] != EEPROM_FORMAT )
						{
							// we load the z compensation matrix before its first usage because this can take some time
							prepareZCompensation();
						}

						if( g_ZCompensationMatrix[0][0] == EEPROM_FORMAT )
						{
							// enable the z compensation only in case we have valid compensation values
							if( Printer::debugInfo() )
							{
								Com::printFLN( PSTR( "M3001: enabling z compensation" ) );
							}
							queueTask( TASK_ENABLE_Z_COMPENSATION );
						}
						else
						{
							if( Printer::debugErrors() )
							{
								Com::printF( PSTR( "M3001: the z compensation can not be enabled because the heat bed compensation matrix is not valid ( " ), g_ZCompensationMatrix[0][0] );
								Com::printF( PSTR( " / " ), EEPROM_FORMAT );
								Com::printFLN( PSTR( " )" ) );
							}
						}
					}
					else
					{
						if( Printer::debugErrors() )
						{
							Com::printFLN( PSTR( "M3001: the z compensation can not be enabled because the home position is unknown" ) );
						}
					}
				}
				break;
			}
			case 3002: // M3002 - configure the min z-compensation offset (units are [steps])
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < (ZAXIS_STEPS_PER_MM /10) )	nTemp = ZAXIS_STEPS_PER_MM /10;
						if( nTemp > (ZAXIS_STEPS_PER_MM *10) )	nTemp = ZAXIS_STEPS_PER_MM *10;

						if( nTemp > g_maxZCompensationSteps )
						{
							// the minimal z-compensation offset can not be bigger than the maximal z-compensation offset
							nTemp = g_maxZCompensationSteps;
						}

						g_minZCompensationSteps = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3002: new min z-compensation offset: " ), g_minZCompensationSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}

						g_diffZCompensationSteps = g_maxZCompensationSteps - g_minZCompensationSteps;
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3003: // M3003 - configure the max z-compensation offset (units are [steps])
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < (ZAXIS_STEPS_PER_MM /10) )	nTemp = ZAXIS_STEPS_PER_MM /10;
						if( nTemp > (ZAXIS_STEPS_PER_MM *10) )	nTemp = ZAXIS_STEPS_PER_MM *10;

						if( nTemp < g_minZCompensationSteps )
						{
							// the maximal z-compensation offset can not be smaller than the minimal z-compensation offset
							nTemp = g_minZCompensationSteps;
						}

						g_maxZCompensationSteps = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3003: new max z-compensation offset: " ), g_maxZCompensationSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}

						g_diffZCompensationSteps = g_maxZCompensationSteps - g_minZCompensationSteps;
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3007: // M3007 - configure the min z-compensation offset (units are [um])
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 100 )		nTemp = 100;
						if( nTemp > 10000 )		nTemp = 10000;

						g_minZCompensationSteps = (nTemp * ZAXIS_STEPS_PER_MM) / 1000;
						if( g_minZCompensationSteps > g_maxZCompensationSteps )
						{
							// the minimal z-compensation offset can not be bigger than the maximal z-compensation offset
							g_minZCompensationSteps = g_maxZCompensationSteps;
						}

						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3007: new min z-compensation offset: " ), nTemp );
							Com::printF( PSTR( " [um]" ) );
							Com::printF( PSTR( " / " ), g_minZCompensationSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}

						g_diffZCompensationSteps = g_maxZCompensationSteps - g_minZCompensationSteps;
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3008: // M3008 - configure the max z-compensation offset (units are [um])
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 100 )		nTemp = 100;
						if( nTemp > 10000 )		nTemp = 10000;

						g_maxZCompensationSteps = (nTemp * ZAXIS_STEPS_PER_MM) / 1000;;
						if( g_maxZCompensationSteps < g_minZCompensationSteps )
						{
							// the maximal z-compensation offset can not be smaller than the minimal z-compensation offset
							g_maxZCompensationSteps = g_minZCompensationSteps;
						}

						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3008: new max z-compensation offset: " ), nTemp );
							Com::printF( PSTR( " [um]" ) );
							Com::printF( PSTR( " / " ), g_maxZCompensationSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}

						g_diffZCompensationSteps = g_maxZCompensationSteps - g_minZCompensationSteps;
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

			case 3004: // M3004 - configure the static z-offset (units are [steps])
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "M3004: this command is not supported anymore, use M3006 instead and consider the changed meaning of the configured value" ) );
					}
				}
				break;
			}
			case 3005: // M3005 - enable custom debug outputs
			{
				if( pCommand->hasS() )
				{
					// test and take over the specified value
					nTemp = pCommand->S;
					if( nTemp < 0 )	nTemp = 0;

					g_debugLevel = (char)nTemp;
					if( Printer::debugInfo() )
					{
						Com::printF( PSTR( "M3005: new debug level: " ), g_debugLevel );
					}
				}
				else
				{
					showInvalidSyntax( pCommand->M );
				}
				break;
			}
			case 3006: // M3006 - configure the static z-offset (units are [um])
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;

						if( nTemp < -(HEAT_BED_Z_COMPENSATION_MAX_MM * 1000) )	nTemp = -(HEAT_BED_Z_COMPENSATION_MAX_MM * 1000);
						if( nTemp > (HEAT_BED_Z_COMPENSATION_MAX_MM * 1000) )	nTemp = (HEAT_BED_Z_COMPENSATION_MAX_MM * 1000);

						g_staticZSteps = (nTemp * ZAXIS_STEPS_PER_MM) / 1000;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3006: new static z-offset: " ), nTemp );
							Com::printF( PSTR( " [um]" ) );
							Com::printF( PSTR( " / " ), g_staticZSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}

#if FEATURE_HEAT_BED_Z_COMPENSATION
			case 3010: // M3010 - start/abort the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					startHeatBedScan();
				}
				break;
			}
			case 3011: // M3011 - clear the z-compensation matrix from the EEPROM
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					clearCompensationMatrix( EEPROM_SECTOR_SIZE );
				}
				break;
			}
			case 3012: // M3012 - restore the default scan parameters
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					restoreDefaultScanParameters();
				}
				break;
			}
			case 3013: // M3013 - output the current heat bed z-compensation matrix
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( g_ZCompensationMatrix[0][0] != EEPROM_FORMAT )
					{
						// we load the z compensation matrix before its first usage because this can take some time
						prepareZCompensation();
					}

					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "M3013: current heat bed compensation matrix: " ) );
					}
					outputCompensationMatrix();
				}
				break;
			}
			case 3020: // M3020 - configure the x start position for the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 5 )						nTemp = 5;
						if( nTemp > (X_MAX_LENGTH -5) )		nTemp = X_MAX_LENGTH -5;

						g_nScanXStartSteps = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3020: new x start position: " ), nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanXStartSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}				
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3021: // M3021 - configure the y start position for the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 5 )						nTemp = 5;
						if( nTemp > (Y_MAX_LENGTH -5 ) )	nTemp = Y_MAX_LENGTH -5;

						g_nScanYStartSteps = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3021: new y start position: " ), nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanYStartSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3022: // M3022 - configure the x step size for the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < HEAT_BED_SCAN_X_STEP_SIZE_MIN_MM )	nTemp = HEAT_BED_SCAN_X_STEP_SIZE_MIN_MM;
						if( nTemp > 100 )								nTemp = 100;

						g_nScanXStepSizeSteps = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3022: new x step size: " ), (int)nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanXStepSizeSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3023: // M3023 - configure the y step size for the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < HEAT_BED_SCAN_Y_STEP_SIZE_MIN_MM )	nTemp = HEAT_BED_SCAN_Y_STEP_SIZE_MIN_MM;
						if( nTemp > 100 )								nTemp = 100;

						g_nScanYStepSizeSteps = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3023: new y step size: " ), (int)nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanYStepSizeSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3024: // M3024 - configure the x end position for the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 5 )						nTemp = 5;
						if( nTemp > (X_MAX_LENGTH -5) )		nTemp = X_MAX_LENGTH -5;

						g_nScanXEndSteps = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3024: new x end position: " ), nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanXEndSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}

						g_nScanXMaxPositionSteps = long(X_MAX_LENGTH * XAXIS_STEPS_PER_MM - g_nScanXEndSteps);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3024: new x max position: " ), (int)g_nScanXMaxPositionSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3025: // M3025 - configure the y end position for the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 5 )						nTemp = 5;
						if( nTemp > (Y_MAX_LENGTH -5 ) )	nTemp = Y_MAX_LENGTH -5;

						g_nScanYEndSteps = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3025: new y end position: " ), nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanYEndSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}

						g_nScanYMaxPositionSteps = long(Y_MAX_LENGTH * YAXIS_STEPS_PER_MM - g_nScanYEndSteps);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3025: new y max position: " ), (int)g_nScanYMaxPositionSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3030: // M3030 - configure the fast step size for moving of the heat bed up during the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp > 50 )	nTemp = 50;
						if( nTemp < 1 )		nTemp = 1;

						g_nScanHeatBedUpFastSteps = -nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3030: new fast step size for moving of the heat bed up: " ), (int)g_nScanHeatBedUpFastSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3031: // M3031 - configure the slow step size for moving of the heat bed up during the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp > 50 )	nTemp = 50;
						if( nTemp < 1 )		nTemp = 1;

						g_nScanHeatBedUpSlowSteps = -nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3031: new slow step size for moving of the heat bed up: " ), (int)g_nScanHeatBedUpSlowSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3032: // M3032 - configure the fast step size for moving of the heat bed down during the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < ZAXIS_STEPS_PER_MM /20 )	nTemp = ZAXIS_STEPS_PER_MM /20;
						if( nTemp > ZAXIS_STEPS_PER_MM )		nTemp = ZAXIS_STEPS_PER_MM;

						g_nScanHeatBedDownFastSteps = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3032: new fast step size for moving of the heat bed down: " ), (int)g_nScanHeatBedDownFastSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3033: // M3033 - configure the slow step size for moving of the heat bed down during the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 50 )	nTemp = 50;

						g_nScanHeatBedDownSlowSteps = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3033: new slow step size for moving of the heat bed down: " ), (int)g_nScanHeatBedDownSlowSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3040: // M3040 - configure the delay (in ms) between two fast movements during the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 1000 )	nTemp = 1000;

						g_nScanFastStepDelay = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3040: new delay between two fast movements: " ), (int)g_nScanFastStepDelay );
							Com::printFLN( PSTR( " [ms]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3041: // M3041 - configure the delay (in ms) between two slow movements during the heat bed scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 1000 )	nTemp = 1000;

						g_nScanSlowStepDelay = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3041: new delay between two slow movements: " ), (int)g_nScanSlowStepDelay );
							Com::printFLN( PSTR( " [ms]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3042: // M3042 - configure the delay (in ms) between reaching of a new x/y position and the test of the idle pressure
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 10000 )	nTemp = 10000;

						g_nScanIdleDelay = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3042: new idle delay: " ), (int)g_nScanIdleDelay );
							Com::printFLN( PSTR( " [ms]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3050: // M3050 - configure the contact pressure delta (in digits)
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 1000 )	nTemp = 1000;

						g_nScanContactPressureDelta = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3050: new contact pressure delta: " ), (int)g_nScanContactPressureDelta );
							Com::printFLN( PSTR( " [digits]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3051: // M3051 - configure the retry pressure delta (in digits)
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 1000 )	nTemp = 1000;

						g_nScanRetryPressureDelta = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3051: new retry pressure delta: " ), (int)g_nScanRetryPressureDelta );
							Com::printFLN( PSTR( " [digits]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3052: // M3052 - configure the idle pressure tolerance (in digits)
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 1000 )	nTemp = 1000;

						g_nScanIdlePressureDelta = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3052: new idle pressure delta: " ), (int)g_nScanIdlePressureDelta );
							Com::printFLN( PSTR( " [digits]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3053: // M3053 - configure the number of A/D converter reads per pressure measurement
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 100 )	nTemp = 100;

						g_nScanPressureReads = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3053: new pressure reads per measurement: " ), (int)g_nScanPressureReads );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3054: // M3054 - configure the delay (in ms) between two A/D converter reads
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 1000 )	nTemp = 1000;

						g_nScanPressureReadDelay = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3054: new delay between two pressure reads: " ), (int)g_nScanPressureReadDelay );
							Com::printFLN( PSTR( " [ms]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3055: // M3055 - configure the pressure tolerance (in digits) per pressure measurement
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )		nTemp = 1;
						if( nTemp > 1000 )	nTemp = 1000;

						g_nScanPressureTolerance = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3055: new scan pressure tolerance: " ), (int)g_nScanPressureTolerance );
							Com::printFLN( PSTR( " [digits]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_PAUSE_PRINTING
			case 3070: // M3070 - pause the print as if the "Pause" button would have been pressed
			{
				if( pCommand->hasS() )
				{
					// test and take over the specified value
					nTemp = pCommand->S;
					if( nTemp < 1 )		nTemp = 1;
					if( nTemp > 2 )		nTemp = 2;

					g_preparePause = 1;

					if( nTemp == 1 )
					{
						// we shall pause the printing
						queueTask( TASK_PAUSE_PRINT_1 );
					}
					if( nTemp == 2 )
					{
						// we shall pause the printing and we shall move away
						queueTask( TASK_PAUSE_PRINT_2 );
					}
				}
				else
				{
					showInvalidSyntax( pCommand->M );
				}

				break;
			}
			case 3071: // M3071 - wait until the print has been continued via the "Continue" button
			{
				waitUntilContinue();
				break;
			}
#endif // FEATURE_PAUSE_PRINTING

#if FEATURE_OUTPUT_PRINTED_OBJECT
			case 3079: // M3079 - output the printed object
			{
				outputObject();
				break;
			}
#endif // FEATURE_OUTPUT_PRINTED_OBJECT

#if FEATURE_PARK
			case 3080: // M3080 - park the printer
			{
				parkPrinter();
				break;
			}
#endif // FEATURE_PARK

#if FEATURE_WATCHDOG
			case 3090: // M3090 - test the watchdog (this command resets the firmware)
			{
				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "M3090: the watchdog is going to reset the firmware" ) );
				}
				HAL::delayMilliseconds( 100 );
				HAL::testWatchdog();
				break;
			}
#endif // FEATURE_WATCHDOG

			case 3091: // M3091 - erase the external EEPROM
			{
				clearExternalEEPROM();
				break;
			}

#if FEATURE_EXTENDED_BUTTONS
			case 3100: // M3100 - configure the number of manual z steps after the "Z up" or "Z down" button has been pressed
			{
				if( pCommand->hasS() )
				{
					// test and take over the specified value
					nTemp = pCommand->S;
					if( nTemp < 1 )							nTemp = 1;
					if( nTemp > MAXIMAL_MANUAL_Z_STEPS )	nTemp = MAXIMAL_MANUAL_Z_STEPS;

					g_nManualZSteps = nTemp;
					if( Printer::debugInfo() )
					{
						Com::printF( PSTR( "M3100: new manual z steps: " ), (int)g_nManualZSteps );
						Com::printFLN( PSTR( " [steps]" ) );
					}
				}
				else
				{
					showInvalidSyntax( pCommand->M );
				}
				break;
			}
			case 3101: // M3101 - configure the number of manual extruder steps after the "Extruder up" or "Extruder down" button has been pressed
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_PRINT ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 1 )							nTemp = 1;
						if( nTemp > (EXT0_STEPS_PER_MM *10) )	nTemp = EXT0_STEPS_PER_MM *10;

						g_nManualExtruderSteps = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3101: new manual extruder steps: " ), (int)g_nManualExtruderSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
#endif // FEATURE_EXTENDED_BUTTONS

#if FEATURE_PAUSE_PRINTING
			case 3102: // M3102 - configure the offset in x, y and z direction which shall be applied in case the "Pause" button has been pressed (units are [steps])
			{
				if( pCommand->hasNoXYZ() && !pCommand->hasE() )
				{
					showInvalidSyntax( pCommand->M );
				}
				else
				{
					if( pCommand->hasX() )
					{
						// test and take over the specified value
						nTemp = pCommand->X;
						if( nTemp < -(XAXIS_STEPS_PER_MM * X_MAX_LENGTH) )		nTemp = -(XAXIS_STEPS_PER_MM * X_MAX_LENGTH);
						if( nTemp > (XAXIS_STEPS_PER_MM * X_MAX_LENGTH) )		nTemp = (XAXIS_STEPS_PER_MM * X_MAX_LENGTH);

						g_nPauseStepsX = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3102: new x pause offset: " ), g_nPauseStepsX );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					if( pCommand->hasY() )
					{
						// test and take over the specified value
						nTemp = pCommand->Y;
						if( nTemp < -(YAXIS_STEPS_PER_MM * Y_MAX_LENGTH) )		nTemp = -(YAXIS_STEPS_PER_MM * Y_MAX_LENGTH);
						if( nTemp > (YAXIS_STEPS_PER_MM * Y_MAX_LENGTH) )		nTemp = (YAXIS_STEPS_PER_MM * Y_MAX_LENGTH);

						g_nPauseStepsY = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3102: new y pause offset: " ), g_nPauseStepsY );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					if( pCommand->hasZ() )
					{
						// test and take over the specified value
						nTemp = pCommand->Z;
						if( nTemp < 0 )											nTemp = 0;
						if( nTemp > (ZAXIS_STEPS_PER_MM * Z_MAX_LENGTH) )		nTemp = (ZAXIS_STEPS_PER_MM * Z_MAX_LENGTH);

						g_nPauseStepsZ = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3102: new z pause offset: " ), g_nPauseStepsZ );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					if( pCommand->hasE() )
					{
						// test and take over the specified value
						nTemp = pCommand->E;
						if( nTemp < 0 )								nTemp = 0;
						if( nTemp > (EXT0_STEPS_PER_MM *5) )		nTemp = EXT0_STEPS_PER_MM *5;

						g_nPauseStepsExtruder = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3102: new extruder pause offset: " ), g_nPauseStepsExtruder );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
				}
				break;
			}
			case 3105: // M3105 - configure the offset in x, y and z direction which shall be applied in case the "Pause" button has been pressed (units are [mm])
			{
				if( pCommand->hasNoXYZ() && !pCommand->hasE() )
				{
					showInvalidSyntax( pCommand->M );
				}
				else
				{
					if( pCommand->hasX() )
					{
						// test and take over the specified value
						nTemp = pCommand->X;
						if( nTemp < -X_MAX_LENGTH )		nTemp = -X_MAX_LENGTH;
						if( nTemp > X_MAX_LENGTH )		nTemp = X_MAX_LENGTH;

						g_nPauseStepsX = nTemp * XAXIS_STEPS_PER_MM;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3105: new x pause offset: " ), nTemp );
							Com::printFLN( PSTR( " [mm]" ) );
						}
					}
					if( pCommand->hasY() )
					{
						// test and take over the specified value
						nTemp = pCommand->Y;
						if( nTemp < -Y_MAX_LENGTH )		nTemp = -Y_MAX_LENGTH;
						if( nTemp > Y_MAX_LENGTH )		nTemp = Y_MAX_LENGTH;

						g_nPauseStepsY = nTemp * YAXIS_STEPS_PER_MM;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3105: new y pause offset: " ), nTemp );
							Com::printFLN( PSTR( " [mm]" ) );
						}
					}
					if( pCommand->hasZ() )
					{
						// test and take over the specified value
						nTemp = pCommand->Z;
						if( nTemp < 0 )					nTemp = 0;
						if( nTemp > Z_MAX_LENGTH )		nTemp = Z_MAX_LENGTH;

						g_nPauseStepsZ = nTemp * ZAXIS_STEPS_PER_MM;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3105: new z pause offset: " ), nTemp );
							Com::printFLN( PSTR( " [mm]" ) );
						}
					}
					if( pCommand->hasE() )
					{
						// test and take over the specified value
						nTemp = pCommand->E;
						if( nTemp < 0 )		nTemp = 0;
						if( nTemp > 5 )		nTemp = 5;

						g_nPauseStepsExtruder = nTemp * EXT0_STEPS_PER_MM;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3105: new extruder pause offset: " ), nTemp );
							Com::printFLN( PSTR( " [mm]" ) );
						}
					}
				}
				break;
			}
#endif // FEATURE_PAUSE_PRINTING

#if FEATURE_PARK
			case 3103: // M3103 - configure the x, y and z position which shall set when the printer is parked
			{
				if( pCommand->hasNoXYZ() )
				{
					showInvalidSyntax( pCommand->M );
				}
				else
				{
					if( pCommand->hasX() )
					{
						// test and take over the specified value
						nTemp = pCommand->X;
						if( nTemp < 0 )				nTemp = 0;
						if( nTemp > X_MAX_LENGTH )	nTemp = X_MAX_LENGTH;

						g_nParkPositionX = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3103: new x park position: " ), g_nParkPositionX );
							Com::printFLN( PSTR( " [mm]" ) );
						}
					}
					if( pCommand->hasY() )
					{
						// test and take over the specified value
						nTemp = pCommand->Y;
						if( nTemp < 0 )				nTemp = 0;
						if( nTemp > Y_MAX_LENGTH )	nTemp = Y_MAX_LENGTH;

						g_nParkPositionY = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3103: new y park position: " ), g_nParkPositionY );
							Com::printFLN( PSTR( " [mm]" ) );
						}
					}
					if( pCommand->hasZ() )
					{
						// test and take over the specified value
						nTemp = pCommand->Z;
						if( nTemp < 0 )				nTemp = 0;
						if( nTemp > Z_MAX_LENGTH )	nTemp = Z_MAX_LENGTH;

						g_nParkPositionZ = nTemp;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3103: new z park position: " ), g_nParkPositionZ );
							Com::printFLN( PSTR( " [mm]" ) );
						}
					}
				}
				break;
			}
#endif // FEATURE_PARK

#if FEATURE_CONTROLLER == 4 || FEATURE_CONTROLLER == 33
			case 3110:	// M3110 - force a status text
			{
				if( pCommand->hasS() )
				{
					// take over the specified value
					if( pCommand->S )
					{
						// ensure that the current text won't be overwritten
						if( Printer::debugInfo() )
						{
							Com::printFLN( PSTR( "M3110: lock" ) );
						}
						uid.lock();
					}
					else
					{
						// allow to overwrite the current string again
						uid.unlock();
						if( Printer::debugInfo() )
						{
							Com::printFLN( PSTR( "M3110: unlock" ) );
						}
					}
				}
				break;
			}
#endif // FEATURE_CONTROLLER == 4 || FEATURE_CONTROLLER == 33

			case 3115:	// M3115 - set the x/y origin to the current x/y position
			{
	            Printer::setOrigin(-Printer::currentPosition[X_AXIS],-Printer::currentPosition[Y_AXIS],Printer::coordinateOffset[Z_AXIS]);
				break;
			}

#if defined(CASE_FAN_PIN) && CASE_FAN_PIN >= 0
			case 3120:	// M3120 - turn on the case fan
			{
				// enable the case fan
				Printer::prepareFanOff = 0;
				WRITE( CASE_FAN_PIN, 1 );

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "M3120: fan on" ) );
				}
				break;
			}

			case 3121:	// M3121 - turn off the case fan
			{
				// disable the case fan
				if( pCommand->hasS() )
				{
					// we shall set a new case fan off delay
					Printer::fanOffDelay =  pCommand->S;
					Printer::fanOffDelay *= 1000;	// convert from [s] to [ms]
				}

				if( Printer::fanOffDelay )
				{
					// we are going to disable the case fan after the delay
					Printer::prepareFanOff = HAL::timeInMilliseconds();

					if( Printer::debugInfo() )
					{
						Com::printF( PSTR( "M3121: fan off in " ), pCommand->S );
						Com::printFLN( PSTR( " [s]" ) );
					}
				}
				else
				{
					// we are going to disable the case fan now
					Printer::prepareFanOff = 0;
					WRITE(CASE_FAN_PIN, 0);

					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "M3121: fan off" ) );
					}
				}
				break;
			}
#endif // defined(CASE_FAN_PIN) && CASE_FAN_PIN >= 0

#if FEATURE_FIND_Z_ORIGIN
			case 3130: // M3130 - start/stop the search of the z-origin
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					startFindZOrigin();
				}
				break;
			}
#endif // FEATURE_FIND_Z_ORIGIN

#if FEATURE_WORK_PART_Z_COMPENSATION
			case 3140: // M3140 - turn the z-compensation off
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "M3140: disabling z compensation" ) );
					}
					queueTask( TASK_DISABLE_Z_COMPENSATION );
				}
				break;
			}
			case 3141: // M3141 - turn the z-compensation on
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( Printer::isHomed() )
					{
						if( g_ZCompensationMatrix[0][0] != EEPROM_FORMAT )
						{
							// we load the z compensation matrix before its first usage because this can take some time
							prepareZCompensation();
						}

						if( g_ZCompensationMatrix[0][0] == EEPROM_FORMAT )
						{
							// enable the z compensation only in case we have valid compensation values
							if( Printer::debugInfo() )
							{
								Com::printFLN( PSTR( "M3141: enabling z compensation" ) );
							}

/*							if( g_nZOriginXPosition && g_nZOriginYPosition )
							{
								Com::printF( PSTR( "g_nZOriginXPosition = " ), g_nZOriginXPosition );
								Com::printFLN( PSTR( ", g_nZOriginYPosition = " ), g_nZOriginYPosition );
							}
*/							queueTask( TASK_ENABLE_Z_COMPENSATION );
						}
						else
						{
							if( Printer::debugErrors() )
							{
								Com::printF( PSTR( "M3141: the z compensation can not be enabled because the work part compensation matrix is not valid ( " ), g_ZCompensationMatrix[0][0] );
								Com::printF( PSTR( " / " ), EEPROM_FORMAT );
								Com::printFLN( PSTR( " )" ) );
							}
						}
					}
					else
					{
						if( Printer::debugErrors() )
						{
							Com::printFLN( PSTR( "M3141: the z compensation can not be enabled because the home position is unknown" ) );
						}
					}
				}
				break;
			}

			case 3146: // M3146 - configure the static z-offset (units are [um])
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;

						if( nTemp < -(WORK_PART_MAX_STATIC_Z_OFFSET_MM * 1000) )	nTemp = -(WORK_PART_MAX_STATIC_Z_OFFSET_MM * 1000);
						if( nTemp > (WORK_PART_MAX_STATIC_Z_OFFSET_MM * 1000) )		nTemp = (WORK_PART_MAX_STATIC_Z_OFFSET_MM * 1000);

						g_staticZSteps = (nTemp * ZAXIS_STEPS_PER_MM) / 1000;
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3146: new static z-offset: " ), nTemp );
							Com::printF( PSTR( " [um]" ) );
							Com::printF( PSTR( " / " ), g_staticZSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}

			case 3149: // M3149 - get/choose the active work part z-compensation matrix
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						nTemp = pCommand->S;

						if( nTemp < 1 || nTemp > EEPROM_MAX_WORK_PART_SECTORS )
						{
							if( Printer::debugErrors() )
							{
								Com::printF( PSTR( "M3149: invalid work part (" ), nTemp );
								Com::printFLN( PSTR( ")" ) );
							}
							break;
						}

						if( g_nActiveWorkPart != nTemp )
						{
							// we have to switch to another work part
							switchActiveWorkPart( (char)nTemp );
						}
					}

					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "M3149: currently active work part: " ), g_nActiveWorkPart );
					}
				}
				break;
			}
			case 3150: // M3150 - start/abort the work part scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						nTemp = pCommand->S;
					}
					else
					{
						nTemp = 0;
					}

					startWorkPartScan( (char)nTemp );
				}
				break;
			}
			case 3151: // M3151 - clear the specified z-compensation matrix from the EEPROM
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						nTemp = pCommand->S;
					}
					else
					{
						// we clear the current z-compensation matrix in case no other z-compensation matrix is specified
						nTemp = g_nActiveWorkPart;
					}

					if( nTemp < 1 || nTemp > EEPROM_MAX_WORK_PART_SECTORS )
					{
						if( Printer::debugErrors() )
						{
							Com::printF( PSTR( "M3151: invalid work part (" ), nTemp );
							Com::printFLN( PSTR( ")" ) );
						}
						break;
					}

					// switch to the specified work part
					g_nActiveWorkPart = (char)nTemp;
					writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, EEPROM_OFFSET_ACTIVE_WORK_PART, g_nActiveWorkPart );
					clearCompensationMatrix( EEPROM_SECTOR_SIZE + (unsigned int)(EEPROM_SECTOR_SIZE * g_nActiveWorkPart) );

					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "M3151: cleared z-compensation matrix: " ), nTemp );
					}
				}
				break;
			}
			case 3152: // M3152 - restore the default scan parameters
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					restoreDefaultScanParameters();
				}
				break;
			}
			case 3153: // M3153 - output the specified work part z-compensation matrix
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						nTemp = pCommand->S;
					}
					else
					{
						// we output the current z-compensation matrix in case no other z-compensation matrix is specified
						nTemp = g_nActiveWorkPart;
					}

					if( nTemp < 0 || nTemp > EEPROM_MAX_WORK_PART_SECTORS )
					{
						if( Printer::debugErrors() )
						{
							Com::printF( PSTR( "M3153: invalid work part (" ), nTemp );
							Com::printFLN( PSTR( ")" ) );
						}
						break;
					}

					if( g_nActiveWorkPart != nTemp )
					{
						// we have to switch to another work part
						switchActiveWorkPart( (char)nTemp );
					}

					if( g_ZCompensationMatrix[0][0] != EEPROM_FORMAT )
					{
						// we load the z compensation matrix before its first usage because this can take some time
						prepareZCompensation();
					}

					if( Printer::debugInfo() )
					{
						Com::printFLN( PSTR( "M3153: current work part compensation matrix: " ) );
					}
					outputCompensationMatrix();
				}
				break;
			}
			case 3160: // M3160 - configure the x start position for the work part scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 5 )						nTemp = 5;
						if( nTemp > (X_MAX_LENGTH -5) )		nTemp = X_MAX_LENGTH -5;

						g_nScanXStartSteps = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3160: new x start position: " ), nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanXStartSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}				
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3161: // M3161 - configure the y start position for the work part scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 5 )						nTemp = 5;
						if( nTemp > (Y_MAX_LENGTH -5 ) )	nTemp = Y_MAX_LENGTH -5;

						g_nScanYStartSteps = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3161: new y start position: " ), nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanYStartSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3162: // M3162 - configure the x step size for the work part scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < WORK_PART_SCAN_X_STEP_SIZE_MIN_MM )	nTemp = WORK_PART_SCAN_X_STEP_SIZE_MIN_MM;
						if( nTemp > 100 )								nTemp = 100;

						g_nScanXStepSizeMm	  = nTemp;
						g_nScanXStepSizeSteps = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3162: new x step size: " ), (int)g_nScanXStepSizeMm );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanXStepSizeSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3163: // M3163 - configure the y step size for the work part scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < WORK_PART_SCAN_Y_STEP_SIZE_MIN_MM )	nTemp = WORK_PART_SCAN_Y_STEP_SIZE_MIN_MM;
						if( nTemp > 100 )								nTemp = 100;

						g_nScanYStepSizeMm	  = nTemp;
						g_nScanYStepSizeSteps = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3163: new y step size: " ), (int)g_nScanYStepSizeMm );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanYStepSizeSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3164: // M3164 - configure the x end position for the work part scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 5 )						nTemp = 5;
						if( nTemp > (X_MAX_LENGTH -5) )		nTemp = X_MAX_LENGTH -5;

						g_nScanXEndSteps = (long)((float)nTemp * XAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3164: new x end position: " ), nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanXEndSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}

						g_nScanXMaxPositionSteps = long(X_MAX_LENGTH * XAXIS_STEPS_PER_MM - g_nScanXEndSteps);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3164: new x max position: " ), (int)g_nScanXMaxPositionSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
			case 3165: // M3165 - configure the y end position for the work part scan
			{
				if( isSupportedCommand( pCommand->M, OPERATING_MODE_CNC ) )
				{
					if( pCommand->hasS() )
					{
						// test and take over the specified value
						nTemp = pCommand->S;
						if( nTemp < 5 )						nTemp = 5;
						if( nTemp > (Y_MAX_LENGTH -5 ) )	nTemp = Y_MAX_LENGTH -5;

						g_nScanYEndSteps = (long)((float)nTemp * YAXIS_STEPS_PER_MM);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3165: new y end position: " ), nTemp );
							Com::printF( PSTR( " [mm], " ), (int)g_nScanYEndSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}

						g_nScanYMaxPositionSteps = long(Y_MAX_LENGTH * YAXIS_STEPS_PER_MM - g_nScanYEndSteps);
						if( Printer::debugInfo() )
						{
							Com::printF( PSTR( "M3165: new y max position: " ), (int)g_nScanYMaxPositionSteps );
							Com::printFLN( PSTR( " [steps]" ) );
						}
					}
					else
					{
						showInvalidSyntax( pCommand->M );
					}
				}
				break;
			}
#endif // FEATURE_WORK_PART_Z_COMPENSATION

			case 3200: // M3200 - reserved for test and debug
			{
				if( pCommand->hasP() )
				{
					switch( pCommand->P )
					{
						case 1:
						{
							Com::printFLN( PSTR( "lowest free RAM: " ), Commands::lowestRAMValue );

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
							Com::printFLN( PSTR( "z-compensation matrix x: " ), COMPENSATION_MATRIX_MAX_X );
							Com::printFLN( PSTR( "z-compensation matrix y: " ), COMPENSATION_MATRIX_MAX_Y );
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
							break;
						}
						case 2:
						{
							Com::printFLN( PSTR( "Homing feedrates:" ) );
							Com::printF( PSTR( "x = " ), Printer::homingFeedrate[0] );
							Com::printF( PSTR( ", y = " ), Printer::homingFeedrate[1] );
							Com::printFLN( PSTR( ", z = " ), Printer::homingFeedrate[2] );
							break;
						}
						case 3:
						{
							if( pCommand->hasS() )
							{
								switch( pCommand->S )
								{
									case  1:	BEEP_SHORT					break;
									case  2:	BEEP_LONG					break;
									case  3:	BEEP_START_PRINTING			break;
									case  4:	BEEP_ABORT_PRINTING			break;
									case  5:	BEEP_STOP_PRINTING			break;
									case  6:	BEEP_PAUSE					break;
									case  7:	BEEP_CONTINUE				break;
									case  8:	BEEP_START_HEAT_BED_SCAN	break;
									case  9:	BEEP_ABORT_HEAT_BED_SCAN	break;
									case 10:	BEEP_STOP_HEAT_BED_SCAN		break;
								}
							}
							break;
						}
						case 4:
						{
							// simulate blocking of the z-axis
							Com::printFLN( PSTR( "M3200: block Z" ) );
							g_nBlockZ = 1;
							break;
						}
						case 5:
						{
							// simulate a temp sensor error
							Com::printFLN( PSTR( "M3200: simulating a defect temperature sensor" ) );
							Printer::flag0 |= PRINTER_FLAG0_TEMPSENSOR_DEFECT;
							reportTempsensorError();
							break;
						}
						case 6:
						{
							Com::printF( PSTR( "nCPS X;" ), Printer::nonCompensatedPositionStepsX );
							Com::printF( PSTR( "; nCPS Y;" ), Printer::nonCompensatedPositionStepsY );
							Com::printF( PSTR( "; nCPS Z;" ), Printer::nonCompensatedPositionStepsZ );
							Com::printF( PSTR( "; tCZ;" ), Printer::targetCompensationZ );
							Com::printF( PSTR( "; cCZ;" ), Printer::currentCompensationZ );
							Com::printF( PSTR( "; tPSZ;" ), Printer::targetPositionStepsZ );
							Com::printF( PSTR( "; cPSZ;" ), Printer::currentPositionStepsZ );
							Com::printF( PSTR( "; dZ;" ), Printer::destinationSteps[Z_AXIS] );
							Com::printF( PSTR( "; cZ;" ), Printer::currentPositionSteps[Z_AXIS] );
							Com::printFLN( PSTR( "; Int32;" ), g_debugInt32 );
							break;
						}

#if FEATURE_FIND_Z_ORIGIN
						case 7:
						{
							Com::printF( PSTR( "Z-Origin X: " ), g_nZOriginXPosition );
							Com::printF( PSTR( "; Z-Origin Y: " ), g_nZOriginYPosition );
							Com::printFLN( PSTR( "; Z-Origin Z: " ), Printer::staticCompensationZ );
							break;
						}
#endif // FEATURE_FIND_Z_ORIGIN

						case 8:
						{
							HAL::forbidInterrupts();
							if( PrintLine::cur )
							{
								Com::printF( PSTR( "Current command: " ) );
								PrintLine::cur->logLine2();
							}
							else
							{
								Com::printFLN( PSTR( "Current command = NULL " ) );
							}
							HAL::allowInterrupts();
							break;
						}
						case 9:
						{
							Com::printFLN( PSTR( "debug level: "), Printer::debugLevel );
							break;
						}
						case 10:
						{
							Com::printF( PSTR( "g_debugLevel= "), g_debugLevel );
							Com::printF( PSTR( ", g_debugLog= "), g_debugLog );
							Com::printF( PSTR( ", g_debugInt16= "), g_debugInt16 );
							Com::printF( PSTR( ", g_debugUInt16= "), (unsigned long)g_debugUInt16 );
							Com::printFLN( PSTR( ", g_debugInt32= "), g_debugInt32 );
							break;
						}
						case 11:
						{
#if FEATURE_CNC_MODE > 0
							Com::printF( PSTR( "operating mode= "), Printer::operatingMode );
							Com::printF( PSTR( ", Z endstop type= "), Printer::ZEndstopType );
							Com::printF( PSTR( ", Z-Min= "), Printer::isZMinEndstopHit() );
							Com::printF( PSTR( ", Z-Max= "), Printer::isZMaxEndstopHit() );
							Com::printF( PSTR( ", lastZDirection= "), Printer::lastZDirection );
							Com::printF( PSTR( ", endstopZMinHit= "), Printer::endstopZMinHit );
							Com::printF( PSTR( ", endstopZMaxHit= "), Printer::endstopZMaxHit );
							Com::printF( PSTR( ", stepsSinceZMinEndstop= "), Printer::stepsSinceZMinEndstop );
							Com::printF( PSTR( ", stepsSinceZMaxEndstop= "), Printer::stepsSinceZMaxEndstop );
							Com::printFLN( PSTR( ", ZEndstopUnknown= "), Printer::ZEndstopUnknown );
#endif // FEATURE_CNC_MODE > 0
							break;
						}
					}
				}

				break;
			}
		}
	}

	return;

} // processCommand


void runStandardTasks( void )
{
	GCode*	pCode;


#if FEATURE_WATCHDOG
	HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

	GCode::readFromSerial();
	pCode = GCode::peekCurrentCommand();
	if( pCode )
	{
#if DEBUG_COMMAND_PEEK
		Com::printFLN( PSTR( "runStandardTasks(): peek" ) );
#endif // DEBUG_COMMAND_PEEK

		Commands::executeGCode( pCode );
		pCode->popCurrentCommand();
	}
    Commands::checkForPeriodicalActions(); 
	return;

} // runStandardTasks


void queueTask( char task )
{
	while( PrintLine::linesCount >= MOVE_CACHE_SIZE )
	{
#if FEATURE_WATCHDOG
		HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

		// wait for a free entry in movement cache
		GCode::readFromSerial();
		Commands::checkForPeriodicalActions();
	}
  
	PrintLine::queueTask( task );
	return;

} // queueTask


extern void processButton( int nAction )
{
	long	Temp;
	switch( nAction )
	{
#if FEATURE_EXTENDED_BUTTONS
		case UI_ACTION_RF1000_HEAT_BED_UP:
		{
#if FEATURE_CNC_MODE > 0
			if( Printer::ZEndstopUnknown )
			{
				if( Printer::debugErrors() )
				{
					Com::printFLN( PSTR( "processButton(): heat bed up: moving aborted (perform a z-homing first)" ) );
				}
				break;
			}
#endif // FEATURE_CNC_MODE > 0

			if( PrintLine::linesCount )
			{
				// we are printing at the moment, the hardware buttons allow to configure an additional, manual offset
#if FEATURE_ENABLE_MANUAL_Z_SAFETY
				Temp = Printer::targetPositionStepsZ - g_nManualZSteps;

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
				Temp += Printer::nonCompensatedPositionStepsZ;
				Temp += Printer::currentCompensationZ;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

				if( Temp < -MANUAL_Z_OVERRIDE_MAX )
				{
					// do not allow to drive the heat bed into the extruder
					if( Printer::debugErrors() )
					{
						Com::printF( PSTR( "processButton(): heat bed up: moving aborted (targetPositionStepsZ = " ), Printer::targetPositionStepsZ );
						Com::printF( PSTR( ", g_nManualZSteps = " ), g_nManualZSteps );

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
						Com::printF( PSTR( ", nonCompensatedPositionStepsZ = " ), Printer::nonCompensatedPositionStepsZ );
						Com::printF( PSTR( ", currentCompensationZ = " ), Printer::currentCompensationZ );
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

						Com::printFLN( PSTR( "" ) );
					}
				}
				else
#endif // FEATURE_ENABLE_MANUAL_Z_SAFETY
				{
					Printer::enableZStepper();
					Printer::unsetAllSteppersDisabled();

					Printer::targetPositionStepsZ -= g_nManualZSteps;
					if( Printer::targetPositionStepsZ < EXTENDED_BUTTONS_Z_MIN )
					{
						Printer::targetPositionStepsZ = EXTENDED_BUTTONS_Z_MIN;
					}

					if( Printer::debugInfo() )
					{
						Com::printF( PSTR( "processButton(): current manual Z steps: " ), Printer::targetPositionStepsZ );
						Com::printFLN( PSTR( " [steps]" ) );
					}
				}
			}
			else
			{
				// we are not printing at the moment, the hardware buttons behave like the standard position menu
				nextPreviousZAction( -1 );
			}
			break;
		}
		case UI_ACTION_RF1000_HEAT_BED_DOWN:
		{
#if FEATURE_CNC_MODE > 0
			if( Printer::ZEndstopUnknown )
			{
				if( Printer::debugErrors() )
				{
					Com::printFLN( PSTR( "processButton(): heat bed down: moving aborted (perform a z-homing first)" ) );
				}
				break;
			}
#endif // FEATURE_CNC_MODE > 0

			if( PrintLine::linesCount )
			{
				// we are printing at the moment, the hardware buttons allow to configure an additional, manual offset
				Printer::enableZStepper();
				Printer::unsetAllSteppersDisabled();

				Printer::targetPositionStepsZ += g_nManualZSteps;

				if( Printer::targetPositionStepsZ > EXTENDED_BUTTONS_Z_MAX )
				{
					Printer::targetPositionStepsZ = EXTENDED_BUTTONS_Z_MAX;
				}

				if( Printer::debugInfo() )
				{
					Com::printF( PSTR( "processButton(): current manual Z steps: " ), Printer::targetPositionStepsZ );
					Com::printFLN( PSTR( " [steps]" ) );
				}
			}
			else
			{
				// we are not printing at the moment, the hardware buttons behave like the standard position menu
				nextPreviousZAction( 1 );
			}
			break;
		}
		case UI_ACTION_RF1000_EXTRUDER_OUTPUT:
		{
			if( Extruder::current->tempControl.targetTemperatureC < UI_SET_MIN_EXTRUDER_TEMP )
			{
				// we do not allow to move the extruder in case it is not heated up enough
				if( Printer::debugErrors() )
				{
					Com::printFLN( PSTR( "processButton(): extruder output: aborted" ) );
				}
				break;
			}

			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "processButton(): extruder output: " ), (int)g_nManualExtruderSteps );
				Com::printFLN( PSTR( " [steps]" ) );
			}

			Extruder::enable();
			Printer::targetPositionStepsE += g_nManualExtruderSteps;

			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "processButton(): current manual E steps: " ), (int)Printer::targetPositionStepsE );
				Com::printFLN( PSTR( " [steps]" ) );
			}
			break;
		}
		case UI_ACTION_RF1000_EXTRUDER_RETRACT:
		{
			if( Extruder::current->tempControl.targetTemperatureC < UI_SET_MIN_EXTRUDER_TEMP )
			{
				// we do not allow to move the extruder in case it is not heated up enough
				if( Printer::debugErrors() )
				{
					Com::printFLN( PSTR( "processButton(): extruder retract: aborted" ) );
				}
				break;
			}

			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "processButton(): extruder retract: " ), (int)g_nManualExtruderSteps );
				Com::printFLN( PSTR( " [steps]" ) );
			}

			Extruder::enable();
			Printer::targetPositionStepsE -= g_nManualExtruderSteps;

			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "processButton(): current manual E steps: " ), (int)Printer::targetPositionStepsE );
				Com::printFLN( PSTR( " [steps]" ) );
			}
			break;
		}
#if FEATURE_PAUSE_PRINTING
		case UI_ACTION_RF1000_PAUSE:
		{
			pausePrint();
			break;
		}
		case UI_ACTION_RF1000_CONTINUE:
		{
			continuePrint();
			break;
		}
#endif // FEATURE_PAUSE_PRINTING

#endif // FEATURE_EXTENDED_BUTTONS

#if FEATURE_HEAT_BED_Z_COMPENSATION
		case UI_ACTION_RF1000_DO_HEAT_BED_SCAN:
		{
			startHeatBedScan();
			break;
		}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
		case UI_ACTION_RF1000_DO_WORK_PART_SCAN:
		{
			startWorkPartScan( 0 );
			break;
		}
		case UI_ACTION_RF1000_SET_SCAN_XY_START:
		{
			setScanXYStart();
			break;
		}
		case UI_ACTION_RF1000_SET_SCAN_XY_END:
		{
			setScanXYEnd();
			break;
		}
#endif // FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_OUTPUT_PRINTED_OBJECT
		case UI_ACTION_RF1000_OUTPUT_OBJECT:
		{
			outputObject();
			break;
		}
#endif // FEATURE_OUTPUT_PRINTED_OBJECT

#if FEATURE_FIND_Z_ORIGIN
		case UI_ACTION_RF1000_FIND_Z_ORIGIN:
		{
			startFindZOrigin();
			break;
		}
#endif // FEATURE_FIND_Z_ORIGIN

#if FEATURE_PARK
		case UI_ACTION_RF1000_PARK:
		{
			parkPrinter();
			break;
		}
#endif // FEATURE_PARK

#if FEATURE_RESET_VIA_MENU
		case UI_ACTION_RF1000_RESET:
		{
			if( Printer::debugInfo() )
			{
				Com::printFLN( PSTR( "processButton(): restart" ) );
			}
			HAL::delayMilliseconds( 100 );
			Commands::emergencyStop();
			break;
		}
#endif // FEATURE_RESET_VIA_MENU
	}
	return;

} // processButton


void nextPreviousZAction( int8_t next )
{
    millis_t	actTime = HAL::timeInMilliseconds();
    millis_t	dtReal;
    millis_t	dt = dtReal = actTime-uid.lastNextPrev;
	int8_t		increment = next;
	long		steps;


    uid.lastNextPrev = actTime;
    if(dt<SPEED_MAX_MILLIS) dt = SPEED_MAX_MILLIS;
    if(dt>SPEED_MIN_MILLIS)
    {
        dt = SPEED_MIN_MILLIS;
        uid.lastNextAccumul = 1;
    }
    float f = (float)(SPEED_MIN_MILLIS-dt)/(float)(SPEED_MIN_MILLIS-SPEED_MAX_MILLIS);
    uid.lastNextAccumul = 1.0f+(float)SPEED_MAGNIFICATION*f*f*f;

#if UI_SPEEDDEPENDENT_POSITIONING
    {
        float d = 0.01*(float)increment*uid.lastNextAccumul;
        if(fabs(d)*2000>Printer::maxFeedrate[Z_AXIS]*dtReal)
            d *= Printer::maxFeedrate[Z_AXIS]*dtReal/(2000*fabs(d));
        steps = (long)(d*Printer::axisStepsPerMM[Z_AXIS]);
        steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));
    }
#else
	steps = increment;
#endif

#if	!FEATURE_ALLOW_UNKNOWN_POSITIONS
	if(!Printer::isHomed())
	{
		// we do not allow unknown positions and the printer is not homed, thus we do not move
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "nextPreviousZAction(): moving z aborted (not homed)" ) );
		}
		return;
	}
#endif // !FEATURE_ALLOW_UNKNOWN_POSITIONS

	if(steps<0 && Printer::isZMinEndstopHit())
	{
		// we shall move upwards but the z-min-endstop is hit already, so we do nothing
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "nextPreviousZAction(): moving z aborted (min reached)" ) );
		}
		return;
	}

#if FEATURE_CNC_MODE == 2
	if(steps>0 && Printer::isZMaxEndstopHit())
	{
		// we shall move downwards but the z-max-endstop is hit already, so we do nothing
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "nextPreviousZAction(): moving z aborted (max reached)" ) );
		}
		return;
	}
#endif // FEATURE_CNC_MODE == 2

	if(steps>0 && Printer::lastCmdPos[Z_AXIS] >= Z_MAX_LENGTH)
	{
		// we shall move downwards but the end of the z-axis has been reached already, so we do nothing
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "nextPreviousZAction(): moving z aborted (z-max reached)" ) );
		}
		return;
	}

	PrintLine::moveRelativeDistanceInStepsReal(0,0,steps,0,Printer::maxFeedrate[Z_AXIS],true);

} // nextPreviousZAction


#if STEPPER_CURRENT_CONTROL==CURRENT_CONTROL_LTC2600

void setMotorCurrent( uint8_t channel, unsigned short level )
{
    const uint8_t ltc_channels[] =  LTC2600_CHANNELS;
    if(channel>LTC2600_NUM_CHANNELS) return;
    uint8_t address = ltc_channels[channel];
    char i;


    // NOTE: Do not increase the current endlessly. In case the engine reaches its current saturation, the engine and the driver can heat up and loss power.
    // When the saturation is reached, more current causes more heating and more power loss.
    // In case of engines with lower quality, the saturation current may be reached before the nominal current.

    // configure the pins
    WRITE( LTC2600_CS_PIN, HIGH );
    SET_OUTPUT( LTC2600_CS_PIN );
    WRITE( LTC2600_SCK_PIN, LOW );
    SET_OUTPUT( LTC2600_SCK_PIN );
    WRITE( LTC2600_SDI_PIN, LOW );
    SET_OUTPUT( LTC2600_SDI_PIN );

    // enable the command interface of the LTC2600
    WRITE( LTC2600_CS_PIN, LOW );

    // transfer command and address
    for( i=7; i>=0; i-- )
    {
        WRITE( LTC2600_SDI_PIN, address & (0x01 << i));
        WRITE( LTC2600_SCK_PIN, 1 );
        WRITE( LTC2600_SCK_PIN, 0 );
    }

    // transfer the data word
    for( i=15; i>=0; i-- )
    {
        WRITE( LTC2600_SDI_PIN, level & (0x01 << i));
        WRITE( LTC2600_SCK_PIN, 1 );
        WRITE( LTC2600_SCK_PIN, 0 );
    }

    // disable the command interface of the LTC2600 -
    // this carries out the specified command
    WRITE( LTC2600_CS_PIN, HIGH );

} // setMotorCurrent


void motorCurrentControlInit( void )
{
    const unsigned int ltc_current[] =  MOTOR_CURRENT;
    uint8_t i;
    for(i=0; i<LTC2600_NUM_CHANNELS; i++)
    {
        setMotorCurrent(i, ltc_current[i] );
    }
}

#endif // CURRENT_CONTROL_LTC2600


#if STEPPER_CURRENT_CONTROL==CURRENT_CONTROL_DRV8711

void drv8711Transmit( unsigned short command )
{
	char	i;

	// transfer the command (= direction, address and data)
	HAL::forbidInterrupts();
	for( i=15; i>=0; i-- )
	{
		WRITE( DRV_SDATI, command & (0x01 << i));
		HAL::delayMicroseconds( 1 );
		WRITE( DRV_SCLK, 1 );
		HAL::delayMicroseconds( 5 );
		WRITE( DRV_SCLK, 0 );
	}
	HAL::allowInterrupts();

} // drv8711Transmit


unsigned short drv8711Receive( unsigned char address )
{
	unsigned short	acknowledge = 0;
	unsigned short	temp;
	char				i;


	if( address > 7 )	return 0;

	acknowledge =  address;
	acknowledge =  acknowledge << 12;
	acknowledge |= 0x8000;

	// transfer the read request plus the register address (4 bits)
	HAL::forbidInterrupts();
	for( i=15; i>=12; i-- )
	{
		WRITE( DRV_SDATI, acknowledge & (0x01 << i));
		HAL::delayMicroseconds( 1 );
		WRITE( DRV_SCLK, 1 );
		HAL::delayMicroseconds( 5 );
		WRITE( DRV_SCLK, 0 );
	}

	HAL::delayMicroseconds( 20 );
  
	// read the acknowledge (12 bits)
	for( i=11; i>=0; i-- )
	{
		temp = READ( DRV_SDATO );
		acknowledge = acknowledge | (temp << i);
		WRITE( DRV_SCLK, 1 );
		HAL::delayMicroseconds( 25 );
		WRITE( DRV_SCLK, 0 );
		HAL::delayMicroseconds( 25 );
	}
	HAL::allowInterrupts();

	return acknowledge;

} // drv8711Receive


void drv8711EnableAll( void )
{
	// enable the chip select of all present DRV8711
	switch( DRV8711_NUM_CHANNELS )
	{
		case 5:	 {  WRITE( O1_SCS_PIN, LOW ); SET_OUTPUT( O1_SCS_PIN ); WRITE( O1_SCS_PIN, HIGH ); }  // fall through
		case 4:  {  WRITE( O0_SCS_PIN, LOW ); SET_OUTPUT( O0_SCS_PIN ); WRITE( O0_SCS_PIN, HIGH ); }  // fall through
		case 3:  {  WRITE( Z_SCS_PIN, LOW );  SET_OUTPUT( Z_SCS_PIN );  WRITE( Z_SCS_PIN, HIGH );  }  // fall through
		case 2:  {  WRITE( Y_SCS_PIN, LOW );  SET_OUTPUT( Y_SCS_PIN );  WRITE( Y_SCS_PIN, HIGH );  }  // fall through
		case 1:  {  WRITE( X_SCS_PIN, LOW );  SET_OUTPUT( X_SCS_PIN );  WRITE( X_SCS_PIN, HIGH );  }
	}

} // drv8711EnableAll


void drv8711DisableAll( void )
{
	// disable the chip select of all present DRV8711
	switch( DRV8711_NUM_CHANNELS )
	{
		case 5:  {  WRITE( O1_SCS_PIN, LOW ); }  // fall through
		case 4:  {  WRITE( O0_SCS_PIN, LOW ); }  // fall through
		case 3:  {  WRITE( Z_SCS_PIN, LOW );  }  // fall through
		case 2:  {  WRITE( Y_SCS_PIN, LOW );  }  // fall through
		case 1:  {  WRITE( X_SCS_PIN, LOW );  }
	}

} // drv8711DisableAll


void drv8711Enable( unsigned char driver )
{
	// enable the chip select of the DRV8711
	switch( driver )
	{
		case 5:  {  WRITE( O1_SCS_PIN, LOW ); SET_OUTPUT( O1_SCS_PIN ); WRITE( O1_SCS_PIN, HIGH ); break;  }
		case 4:  {  WRITE( O0_SCS_PIN, LOW ); SET_OUTPUT( O0_SCS_PIN ); WRITE( O0_SCS_PIN, HIGH ); break;  }
		case 3:  {  WRITE( Z_SCS_PIN, LOW );  SET_OUTPUT( Z_SCS_PIN );  WRITE( Z_SCS_PIN, HIGH );  break;  }
		case 2:  {  WRITE( Y_SCS_PIN, LOW );  SET_OUTPUT( Y_SCS_PIN );  WRITE( Y_SCS_PIN, HIGH );  break;  }
		case 1:  {  WRITE( X_SCS_PIN, LOW );  SET_OUTPUT( X_SCS_PIN );  WRITE( X_SCS_PIN, HIGH );  break;  }
	}

} // drv8711Enable


void drv8711Disable( unsigned char driver )
{
	// disable the chip select of the DRV8711
	switch( driver )
	{
		case 5:  {  WRITE( O1_SCS_PIN, LOW ); break;  }
		case 4:  {  WRITE( O0_SCS_PIN, LOW ); break;  }
		case 3:  {  WRITE( Z_SCS_PIN, LOW );  break;  }
		case 2:  {  WRITE( Y_SCS_PIN, LOW );  break;  }
		case 1:  {  WRITE( X_SCS_PIN, LOW );  break;  }
	}

} // drv8711Disable


void drv8711Init( void )
{
	char	i;
  

	// configure the pins
	WRITE( DRV_RESET1, LOW );
	SET_OUTPUT( DRV_RESET1 );

#if DRV_RESET2
	WRITE( DRV_RESET2, LOW );
	SET_OUTPUT( DRV_RESET2 );
#endif // DRV_RESET2

	WRITE( DRV_SCLK, LOW );
	SET_OUTPUT( DRV_SCLK );
	WRITE( DRV_SDATI, LOW );
	SET_OUTPUT( DRV_SDATI );

	// configure the following inputs as pullup
	WRITE( DRV_SDATO, HIGH );
	WRITE( DRV_FAULT, HIGH );
	WRITE( X_STALL_PIN, HIGH );
	WRITE( Y_STALL_PIN, HIGH );
	WRITE( Z_STALL_PIN, HIGH );
	WRITE( O0_STALL_PIN, HIGH );
	WRITE( O1_STALL_PIN, HIGH );

	// reset all DRV8711 (active high)
	WRITE( DRV_RESET1, HIGH );

#if DRV_RESET2
	WRITE( DRV_RESET2, HIGH );
#endif // DRV_RESET2

	HAL::delayMicroseconds( 5000 );
	WRITE( DRV_RESET1, LOW );

#if DRV_RESET2
	WRITE( DRV_RESET2, LOW );
#endif // DRV_RESET2

	HAL::delayMicroseconds( 5000 );
  
	// configure all registers except the motor current (= register 01)
	drv8711EnableAll();
	drv8711Transmit( DRV8711_REGISTER_00 );
	drv8711DisableAll();
	HAL::delayMicroseconds( 1 );

	drv8711EnableAll();
	drv8711Transmit( DRV8711_REGISTER_02 );
	drv8711DisableAll();
	HAL::delayMicroseconds( 1 );

	drv8711EnableAll();
	drv8711Transmit( DRV8711_REGISTER_03 );
	drv8711DisableAll();
	HAL::delayMicroseconds( 1 );

	drv8711EnableAll();
	drv8711Transmit( DRV8711_REGISTER_04 );
	drv8711DisableAll();
	HAL::delayMicroseconds( 1 );

	drv8711EnableAll();
	drv8711Transmit( DRV8711_REGISTER_05 );
	drv8711DisableAll();
	HAL::delayMicroseconds( 1 );

	drv8711EnableAll();
	drv8711Transmit( DRV8711_REGISTER_06 );
	drv8711DisableAll();
	HAL::delayMicroseconds( 1 );

	drv8711EnableAll();
	drv8711Transmit( DRV8711_REGISTER_07 );
	drv8711DisableAll();

} // drv8711Init


void setMotorCurrent( unsigned char driver, unsigned short level )
{
	unsigned short	command;
	char			i;
	
	
	// NOTE: Do not increase the current endlessly. In case the engine reaches its current saturation, the engine and the driver can heat up and loss power.
	// When the saturation is reached, more current causes more heating and more power loss.
	// In case of engines with lower quality, the saturation current may be reached before the nominal current.

	// configure the pins
	WRITE( DRV_SCLK, LOW );
	SET_OUTPUT( DRV_SCLK );
	WRITE( DRV_SDATI, LOW );
	SET_OUTPUT( DRV_SDATI );
		
	drv8711Enable( driver);

	// we have to write to register 01
	command = 0x1100 + level;
	drv8711Transmit( command );

	drv8711Disable( driver );

} // setMotorCurrent


void motorCurrentControlInit( void )
{
	const unsigned short	drv_current[] =  MOTOR_CURRENT;
	unsigned char			i;

	// configure all DRV8711
	drv8711Init();

	// set all motor currents
	for(i=0;i<DRV8711_NUM_CHANNELS;i++)
	{
		setMotorCurrent( i+1, drv_current[i] );
	}

} // motorCurrentControlInit

#endif // CURRENT_CONTROL_DRV8711


void cleanupXPositions( void )
{
	HAL::forbidInterrupts();

#if FEATURE_HEAT_BED_Z_COMPENSATION  || FEATURE_WORK_PART_Z_COMPENSATION
    Printer::nonCompensatedPositionStepsX = 0;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_EXTENDED_BUTTONS
	Printer::targetPositionStepsX = Printer::currentPositionStepsX = 0;
#endif // FEATURE_EXTENDED_BUTTONS
	
#if FEATURE_PAUSE_PRINTING
	g_nContinueStepsX = 0;
	g_preparePause	  = 0;
	g_pausePrint	  = 0;
	g_printingPaused  = 0;
	g_uPauseTime	  = 0;
	g_pauseBeepDone	  = 0;
#endif // FEATURE_PAUSE_PRINTING

	HAL::allowInterrupts();

} // cleanupXPositions


void cleanupYPositions( void )
{
	HAL::forbidInterrupts();

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
    Printer::nonCompensatedPositionStepsY = 0;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_EXTENDED_BUTTONS
	Printer::targetPositionStepsY = Printer::currentPositionStepsY = 0;
#endif // FEATURE_EXTENDED_BUTTONS
	
#if FEATURE_PAUSE_PRINTING
	g_nContinueStepsY = 0;
	g_preparePause	  = 0;
	g_pausePrint	  = 0;
	g_printingPaused  = 0;
	g_uPauseTime	  = 0;
	g_pauseBeepDone	  = 0;
#endif // FEATURE_PAUSE_PRINTING

	HAL::allowInterrupts();

} // cleanupYPositions


void cleanupZPositions( void )
{
	HAL::forbidInterrupts();

#if FEATURE_HEAT_BED_Z_COMPENSATION
    Printer::doHeatBedZCompensation = 0;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
    Printer::doWorkPartZCompensation = 0;
	Printer::staticCompensationZ	 = 0;
#endif // FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
    Printer::nonCompensatedPositionStepsZ = 0;
    Printer::targetCompensationZ		  = 0;
    Printer::currentCompensationZ		  = 0;
	g_nZScanZPosition					  = 0;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_FIND_Z_ORIGIN
	g_nZOriginXPosition = 0;
	g_nZOriginYPosition = 0;
	g_nZOriginZPosition = 0;
#endif // FEATURE_FIND_Z_ORIGIN

#if FEATURE_EXTENDED_BUTTONS
	Printer::targetPositionStepsZ = Printer::currentPositionStepsZ = 0;
#endif // FEATURE_EXTENDED_BUTTONS
	
#if FEATURE_PAUSE_PRINTING
	g_nContinueStepsZ = 0;
	g_preparePause	  = 0;
	g_pausePrint	  = 0;
	g_printingPaused  = 0;
	g_uPauseTime	  = 0;
	g_pauseBeepDone	  = 0;
#endif // FEATURE_PAUSE_PRINTING

	HAL::allowInterrupts();

} // cleanupZPositions


void cleanupEPositions( void )
{
	HAL::forbidInterrupts();

#if FEATURE_EXTENDED_BUTTONS
	Printer::targetPositionStepsE = Printer::currentPositionStepsE = 0;
#endif // FEATURE_EXTENDED_BUTTONS
	
#if FEATURE_PAUSE_PRINTING
	g_nContinueStepsExtruder = 0;
	g_preparePause			 = 0;
	g_pausePrint			 = 0;
	g_printingPaused		 = 0;
	g_uPauseTime			 = 0;
	g_pauseBeepDone			 = 0;
#endif // FEATURE_PAUSE_PRINTING

	HAL::allowInterrupts();

} // cleanupEPositions


void setZOrigin( void )
{
#if FEATURE_FIND_Z_ORIGIN
	g_nZOriginXPosition = Printer::currentPositionSteps[X_AXIS];
	g_nZOriginYPosition = Printer::currentPositionSteps[Y_AXIS];
	g_nZOriginZPosition = 0;
#endif // FEATURE_FIND_Z_ORIGIN

	Printer::updateCurrentPosition();

	// it does not make sense to change the length here because Printer::currentPosition[Z_AXIS] can be a more or less random value
	//Printer::zLength -= Printer::currentPosition[Z_AXIS];

    Printer::currentPositionSteps[Z_AXIS] = 0;
    Printer::updateDerivedParameter();
#if NONLINEAR_SYSTEM
    transformCartesianStepsToDeltaSteps(Printer::currentPositionSteps, Printer::currentDeltaPositionSteps);
#endif
    Printer::updateCurrentPosition(true);
    
	if( Printer::debugInfo() )
	{
		Com::printFLN(Com::tZProbePrinterHeight,Printer::zLength);
	}

#if EEPROM_MODE!=0
    EEPROM::storeDataIntoEEPROM(false);

	if( Printer::debugInfo() )
	{
		Com::printFLN(Com::tEEPROMUpdated);
	}
#endif
    
	Commands::printCurrentPosition();

	Printer::setZOriginSet(true);

	BEEP_ACCEPT_SET_POSITION

} // setZOrigin


#if FEATURE_FIND_Z_ORIGIN
void startFindZOrigin( void )
{
	if( g_nFindZOriginStatus )
	{
		if( !g_abortSearch )
		{
			// abort the finding of the z-origin
			if( Printer::debugInfo() )
			{
				Com::printFLN( PSTR( "startFindZOrigin(): the search has been cancelled" ) );
			}
			g_abortSearch = 1;
		}
	}
	else
	{
		if( Printer::operatingMode != OPERATING_MODE_CNC )
		{
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "startFindZOrigin(): finding of the z-origin is not supported in this mode" ) );
				return;
			}
		}

/*		if( PrintLine::linesCount )
		{
			// there is some printing in progress at the moment - do not start the search in this case
			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "startFindZOrigin(): the search can not be started while the milling is in progress" ) );
				return;
			}
		}
*/
		// start the search
		g_nFindZOriginStatus = 1;

#if FEATURE_HEAT_BED_Z_COMPENSATION
		// when the search is running, the z-compensation must be disabled
		if( Printer::doHeatBedZCompensation )
		{
			if( Printer::debugInfo() )
			{
				Com::printFLN( PSTR( "startFindZOrigin(): the z compensation has been disabled" ) );
			}
			resetZCompensation();
		}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
		// when the search is running, the z-compensation must be disabled
		if( Printer::doWorkPartZCompensation )
		{
			if( Printer::debugInfo() )
			{
				Com::printFLN( PSTR( "startFindZOrigin(): the z compensation has been disabled" ) );
			}
			resetZCompensation();
		}
#endif // FEATURE_WORK_PART_Z_COMPENSATION

	}

	return;

} // startFindZOrigin


void findZOrigin( void )
{
	static short	nMaxPressureContact;
	static short	nMinPressureContact;
	short			nCurrentPressure;
	unsigned long	uStartTime;
	unsigned long	uCurrentTime;


	if( g_abortSearch )
	{
		// the search has been aborted
		g_abortSearch		= 0;
		g_nZOriginZPosition = 0;

		// turn off the engines
		Printer::disableZStepper();

		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "findZOrigin(): the search has been aborted" ) );
		}

		UI_STATUS_UPD( UI_TEXT_FIND_Z_ORIGIN_ABORTED );
		g_nFindZOriginStatus = 0;
		return;
	}

	// show that we are active
	previousMillisCmd = HAL::timeInMilliseconds();

	if( g_nFindZOriginStatus )
	{
		UI_STATUS( UI_TEXT_FIND_Z_ORIGIN );

		switch( g_nFindZOriginStatus )
		{
			case 1:
			{
				g_abortSearch		= 0;
				g_nZOriginZPosition = 0;

				if( Printer::debugInfo() )
				{
					Com::printFLN( PSTR( "findZOrigin(): the search has been started" ) );
				}

				if( readAveragePressure( &nCurrentPressure ) )
				{
					// some error has occurred
					if( Printer::debugErrors() )
					{
						Com::printFLN( PSTR( "findZOrigin(): the start pressure could not be determined" ) );
					}
					g_abortSearch = 1;
					return;
				}

				nMinPressureContact = nCurrentPressure - SEARCH_Z_ORIGIN_CONTACT_PRESSURE_DELTA;
				nMaxPressureContact = nCurrentPressure + SEARCH_Z_ORIGIN_CONTACT_PRESSURE_DELTA;

				if( Printer::debugInfo() )
				{
					Com::printF( PSTR( "findZOrigin(): nMinPressureContact = " ), nMinPressureContact );
					Com::printFLN( PSTR( ", nMaxPressureContact = " ), nMaxPressureContact );
				}

				Printer::enableZStepper();
				Printer::unsetAllSteppersDisabled();

				// prepare the direction of the z-axis (we have to move the milling bed up)
				prepareBedUp();

				g_nTempDirectionZ	 = -1;
				g_nFindZOriginStatus = 10;

#if DEBUG_FIND_Z_ORIGIN
				Com::printFLN( PSTR( "findZOrigin(): 1 -> 10" ) );
#endif // DEBUG_FIND_Z_ORIGIN
				break;
			}
			case 10:
			{
				// move the heat bed up until we detect the contact pressure
				uStartTime = HAL::timeInMilliseconds();
				while( 1 )
				{
#if FEATURE_WATCHDOG
					HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

					nCurrentPressure = readStrainGauge( SCAN_STRAIN_GAUGE );
					//g_debugInt16	 = nCurrentPressure;

					if( nCurrentPressure > nMaxPressureContact || nCurrentPressure < nMinPressureContact )
					{
						// we have reached the target pressure
						g_nFindZOriginStatus = 20;

#if DEBUG_FIND_Z_ORIGIN
						Com::printFLN( PSTR( "findZOrigin(): 10 -> 20" ) );
#endif // DEBUG_FIND_Z_ORIGIN
						return;
					}

					if( Printer::isZMinEndstopHit() )
					{
						if( Printer::debugErrors() )
						{
							Com::printFLN( PSTR( "findZOrigin(): the z-min endstop has been reached" ) );
						}
						g_abortSearch = 1;
						return;
					}

					g_nZOriginZPosition += moveZ( SEARCH_Z_ORIGIN_BED_UP_STEPS );

					uCurrentTime = HAL::timeInMilliseconds();
					if( (uCurrentTime - uStartTime) > SEARCH_Z_ORIGIN_BREAKOUT_DELAY )
					{
						// do not stay within this loop forever
						return;
					}

					if( g_abortSearch )
					{
						break;
					}
				}

				// we should never end up here
				break;
			}
			case 20:
			{
				// move the heat bed down again until we do not detect any contact anymore
				uStartTime = HAL::timeInMilliseconds();
				while( 1 )
				{
#if FEATURE_WATCHDOG
					HAL::pingWatchdog();
#endif // FEATURE_WATCHDOG

					nCurrentPressure = readStrainGauge( SCAN_STRAIN_GAUGE );
					//g_debugInt16	 = nCurrentPressure;

					if( nCurrentPressure > nMinPressureContact && nCurrentPressure < nMaxPressureContact )
					{
						// we have reached the target pressure
						g_nFindZOriginStatus = 30;

#if DEBUG_FIND_Z_ORIGIN
						Com::printFLN( PSTR( "findZOrigin(): 20 -> 30" ) );
#endif // DEBUG_FIND_Z_ORIGIN
						return;
					}

					if( Printer::isZMaxEndstopHit() )
					{
						if( Printer::debugErrors() )
						{
							Com::printFLN( PSTR( "findZOrigin(): the z-max endstop has been reached" ) );
						}
						g_abortSearch = 1;
						return;
					}

					g_nZOriginZPosition += moveZ( SEARCH_Z_ORIGIN_BED_DOWN_STEPS );

					uCurrentTime = HAL::timeInMilliseconds();
					if( (uCurrentTime - uStartTime) > SEARCH_Z_ORIGIN_BREAKOUT_DELAY )
					{
						// do not stay within this loop forever
						return;
					}

					if( g_abortSearch )
					{
						break;
					}
				}

				// we should never end up here
				break;
			}
			case 30:
			{
				// we have found the z-origin
				setZOrigin();

				GCode::executeFString( Com::tFindZOrigin );
				g_nFindZOriginStatus = 40;

#if DEBUG_FIND_Z_ORIGIN
				Com::printFLN( PSTR( "findZOrigin(): 30 -> 40" ) );
#endif // DEBUG_FIND_Z_ORIGIN
				break;
			}
			case 40:
			{
				if( PrintLine::linesCount )
				{
					// wait until all moves have been done
					break;
				}

				g_nFindZOriginStatus = 0;
				UI_STATUS( UI_TEXT_FIND_Z_ORIGIN_DONE );

#if DEBUG_FIND_Z_ORIGIN
				Com::printFLN( PSTR( "findZOrigin(): 40 -> 0" ) );
#endif // DEBUG_FIND_Z_ORIGIN
				break;
			}
		}
	}

	// we should never end up here
	return;

} // findZOrigin
#endif // FEATURE_FIND_Z_ORIGIN


#if FEATURE_CNC_MODE > 0
void switchOperatingMode( char newOperatingMode )
{
	if( newOperatingMode != OPERATING_MODE_PRINT && newOperatingMode != OPERATING_MODE_CNC )
	{
		// do not allow not-supported operating modes
		return;
	}

	Printer::operatingMode = newOperatingMode;
	if( Printer::operatingMode == OPERATING_MODE_PRINT )
	{
		setupForPrinting();
	}
	else
	{
		setupForMilling();
	}

	g_staticZSteps = 0;
	return;

} // switchOperatingMode


void switchActiveWorkPart( char newActiveWorkPart )
{
	if( newActiveWorkPart < 1 || newActiveWorkPart > EEPROM_MAX_WORK_PART_SECTORS )
	{
		// do not allow not-supported active work parts
		return;
	}

	g_nActiveWorkPart = newActiveWorkPart;
	writeWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, EEPROM_OFFSET_ACTIVE_WORK_PART, g_nActiveWorkPart );

	if( loadCompensationMatrix( EEPROM_SECTOR_SIZE + (unsigned int)(EEPROM_SECTOR_SIZE * g_nActiveWorkPart) ) )
	{
		// there is no valid compensation matrix available
		initCompensationMatrix();
	}
	return;

} // switchActiveWorkPart


void setScanXYStart( void )
{
/*	if( Printer::currentPositionSteps[X_AXIS] > g_nScanXMaxPositionSteps ||
		Printer::currentPositionSteps[Y_AXIS] > g_nScanYMaxPositionSteps )
	{
		// we can not take over the new x/y start position in case it is bigger than the current x/y end position
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "setScanXYStart(): the new x/y start position can not be set because it is bigger than the current x/y end position" ) );
			Com::printF( PSTR( "current: x = " ), (float)Printer::currentPositionSteps[X_AXIS] / XAXIS_STEPS_PER_MM );
			Com::printF( PSTR( ", y = " ), (float)Printer::currentPositionSteps[Y_AXIS] / YAXIS_STEPS_PER_MM );
			Com::printFLN( PSTR( " [mm]" ) );
			Com::printF( PSTR( "end: x = " ), (float)g_nScanXMaxPositionSteps / XAXIS_STEPS_PER_MM );
			Com::printF( PSTR( ", y = " ), (float)g_nScanYMaxPositionSteps / YAXIS_STEPS_PER_MM );
			Com::printFLN( PSTR( " [mm]" ) );
		}
		BEEP_ABORT_SET_POSITION
		return;
	}
*/
	// round to Integer [mm]
	g_nScanXStartSteps = (float)Printer::currentPositionSteps[X_AXIS] / XAXIS_STEPS_PER_MM;
	g_nScanXStartSteps *= XAXIS_STEPS_PER_MM;
	g_nScanYStartSteps = (float)Printer::currentPositionSteps[Y_AXIS] / YAXIS_STEPS_PER_MM;
	g_nScanYStartSteps *= YAXIS_STEPS_PER_MM;

	if( g_nScanXStartSteps > g_nScanXMaxPositionSteps ||
		g_nScanYStartSteps > g_nScanYMaxPositionSteps )
	{
		// the new start position would be bigger than the current end position - we set the end position to the start position in this case
		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "setScanXYStart(): the new x/y start position is bigger than the current x/y end position, the x/y end position will be set to the new x/y start position" ) );
			g_nScanXMaxPositionSteps = g_nScanXStartSteps;
			g_nScanYMaxPositionSteps = g_nScanYStartSteps;
		}
	}
	
	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "setScanXYStart(): the new x/y start position has been set" ) );
		Com::printF( PSTR( "start: x = " ), (float)g_nScanXStartSteps / XAXIS_STEPS_PER_MM );
		Com::printF( PSTR( ", y = " ), (float)g_nScanYStartSteps / YAXIS_STEPS_PER_MM );
		Com::printFLN( PSTR( " [mm]" ) );
		Com::printF( PSTR( "end: x = " ), (float)g_nScanXMaxPositionSteps / XAXIS_STEPS_PER_MM );
		Com::printF( PSTR( ", y = " ), (float)g_nScanYMaxPositionSteps / YAXIS_STEPS_PER_MM );
		Com::printFLN( PSTR( " [mm]" ) );
	}
	BEEP_ACCEPT_SET_POSITION
	return;

} // setScanXYStart


void setScanXYEnd( void )
{
/*	if( Printer::currentPositionSteps[X_AXIS] < g_nScanXStartSteps ||
		Printer::currentPositionSteps[Y_AXIS] < g_nScanYStartSteps )
	{
		// we can not take over the new x/y end position in case it is smaller than the current x/y start position
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "setScanXYEnd(): the new x/y end position can not be set because it is smaller than the current x/y start position" ) );
			Com::printF( PSTR( "start: x = " ), (float)g_nScanXStartSteps / XAXIS_STEPS_PER_MM );
			Com::printF( PSTR( ", y = " ), (float)g_nScanYStartSteps / YAXIS_STEPS_PER_MM );
			Com::printFLN( PSTR( " [mm]" ) );
			Com::printF( PSTR( "current: x = " ), (float)Printer::currentPositionSteps[X_AXIS] / XAXIS_STEPS_PER_MM );
			Com::printF( PSTR( ", y = " ), (float)Printer::currentPositionSteps[Y_AXIS] / YAXIS_STEPS_PER_MM );
			Com::printFLN( PSTR( " [mm]" ) );
		}
		BEEP_ABORT_SET_POSITION
		return;
	}
*/
	// round to Integer [mm]
	g_nScanXMaxPositionSteps =  (float)Printer::currentPositionSteps[X_AXIS] / XAXIS_STEPS_PER_MM;
	g_nScanXMaxPositionSteps *= XAXIS_STEPS_PER_MM;
	g_nScanYMaxPositionSteps =  (float)Printer::currentPositionSteps[Y_AXIS] / YAXIS_STEPS_PER_MM;
	g_nScanYMaxPositionSteps *= YAXIS_STEPS_PER_MM;

	if( g_nScanXMaxPositionSteps < g_nScanXStartSteps ||
		g_nScanYMaxPositionSteps < g_nScanYStartSteps )
	{
		// the new end position would be smaller than the current start position - we set the start position to the end position in this case
		if( Printer::debugInfo() )
		{
			Com::printFLN( PSTR( "setScanXYEnd(): the new x/y end position is smaller than the current x/y start position, the x/y start position will be set to the new x/y end position" ) );
			g_nScanXStartSteps = g_nScanXMaxPositionSteps;
			g_nScanYStartSteps = g_nScanYMaxPositionSteps;
		}
	}
	
	if( Printer::debugInfo() )
	{
		Com::printFLN( PSTR( "setScanXYEnd(): the new x/y end position has been set" ) );
		Com::printF( PSTR( "start: x = " ), (float)g_nScanXStartSteps / XAXIS_STEPS_PER_MM );
		Com::printF( PSTR( ", y = " ), (float)g_nScanYStartSteps / YAXIS_STEPS_PER_MM );
		Com::printFLN( PSTR( " [mm]" ) );
		Com::printF( PSTR( "end: x = " ), (float)g_nScanXMaxPositionSteps / XAXIS_STEPS_PER_MM );
		Com::printF( PSTR( ", y = " ), (float)g_nScanYMaxPositionSteps / YAXIS_STEPS_PER_MM );
		Com::printFLN( PSTR( " [mm]" ) );
	}
	BEEP_ACCEPT_SET_POSITION
	return;

} // setScanXYEnd
#endif // FEATURE_CNC_MODE > 0


void setupForPrinting( void )
{
	Printer::flag0 &= ~PRINTER_FLAG0_TEMPSENSOR_DEFECT;

#if FEATURE_HEAT_BED_Z_COMPENSATION
	// restore the default scan parameters
	restoreDefaultScanParameters();
	
/*	// restore the last known compensation matrix
	// this operation must be performed after restoring of the default scan parameters because the values from the EEPROM can overwrite some scan parameters
	if( loadCompensationMatrix( EEPROM_SECTOR_SIZE ) )
	{
		// there is no valid compensation matrix available
		initCompensationMatrix();

		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "setupForPrinting(): the compensation matrix is not available" ) );
		}
	}
*/
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if EEPROM_MODE
	// read the settings from the EEPROM
	Printer::homingFeedrate[0] = HAL::eprGetFloat(EPR_X_HOMING_FEEDRATE_PRINT);
	Printer::homingFeedrate[1] = HAL::eprGetFloat(EPR_Y_HOMING_FEEDRATE_PRINT);
	Printer::homingFeedrate[2] = HAL::eprGetFloat(EPR_Z_HOMING_FEEDRATE_PRINT);
#else
	// read the settings from Configuration.h
	Printer::homingFeedrate[X_AXIS] = HOMING_FEEDRATE_X_PRINT;
	Printer::homingFeedrate[Y_AXIS] = HOMING_FEEDRATE_Y_PRINT;
	Printer::homingFeedrate[Z_AXIS] = HOMING_FEEDRATE_Z_PRINT;
#endif // EEPROM_MODE

	Printer::setMenuMode( MENU_MODE_MILLER, false );
	Printer::setMenuMode( MENU_MODE_PRINTER, true );
	UI_STATUS( UI_TEXT_PRINTER_READY );
	return;

} // setupForPrinting


void setupForMilling( void )
{

#if FEATURE_WORK_PART_Z_COMPENSATION
	// we must restore the default work part scan parameters
	restoreDefaultScanParameters();

	g_nActiveWorkPart = (char)readWord24C256( I2C_ADDRESS_EXTERNAL_EEPROM, EEPROM_OFFSET_ACTIVE_WORK_PART );

	if( g_nActiveWorkPart < 1 || g_nActiveWorkPart > EEPROM_MAX_WORK_PART_SECTORS )
	{
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "setupForMilling(): invalid active work part detected: " ), (int)g_nActiveWorkPart );
		}

		// continue with the default work part
		g_nActiveWorkPart = 1;
	}

/*	// we must restore the work part z-compensation matrix
	// this operation must be performed after restoring of the default scan parameters because the values from the EEPROM can overwrite some scan parameters
	if( loadCompensationMatrix( 0 ) )
	{
		// there is no valid compensation matrix available
		initCompensationMatrix();

		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "setupForMilling(): the compensation matrix is not available" ) );
		}
	}
*/
#endif // FEATURE_WORK_PART_Z_COMPENSATION

#if EEPROM_MODE
	// read the settings from the EEPROM
	Printer::homingFeedrate[0] = HAL::eprGetFloat(EPR_X_HOMING_FEEDRATE_CNC);
	Printer::homingFeedrate[1] = HAL::eprGetFloat(EPR_Y_HOMING_FEEDRATE_CNC);
	Printer::homingFeedrate[2] = HAL::eprGetFloat(EPR_Z_HOMING_FEEDRATE_CNC);
#else
	// read the settings from Configuration.h
	Printer::homingFeedrate[X_AXIS] = HOMING_FEEDRATE_X_CNC;
	Printer::homingFeedrate[Y_AXIS] = HOMING_FEEDRATE_Y_CNC;
	Printer::homingFeedrate[Z_AXIS] = HOMING_FEEDRATE_Z_CNC;
#endif // EEPROM_MODE

	// disable all heaters
	Extruder::setHeatedBedTemperature( 0, false );
	Extruder::setTemperatureForExtruder( 0, 0, false );

	Printer::setMenuMode( MENU_MODE_PRINTER, false );
	Printer::setMenuMode( MENU_MODE_MILLER, true );
	UI_STATUS( UI_TEXT_MILLER_READY );
	return;

} // setupForMilling


void prepareZCompensation( void )
{
	char	mode;


#if FEATURE_CNC_MODE > 0
	mode = Printer::operatingMode;
#else
	mode = OPERATING_MODE_PRINT;
#endif // FEATURE_CNC_MODE > 0

#if FEATURE_HEAT_BED_Z_COMPENSATION
	if( mode == OPERATING_MODE_PRINT )
	{
		// restore the default scan parameters
		restoreDefaultScanParameters();
	
		// restore the last known compensation matrix
		// this operation must be performed after restoring of the default scan parameters because the values from the EEPROM can overwrite some scan parameters
		if( loadCompensationMatrix( EEPROM_SECTOR_SIZE ) )
		{
			// there is no valid compensation matrix available
			initCompensationMatrix();

			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "prepareZCompensation(): the compensation matrix is not available" ) );
			}
		}
	}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
	if( mode == OPERATING_MODE_CNC )
	{
		// we must restore the default work part scan parameters
		restoreDefaultScanParameters();

		// we must restore the work part z-compensation matrix
		// this operation must be performed after restoring of the default scan parameters because the values from the EEPROM can overwrite some scan parameters
		if( loadCompensationMatrix( 0 ) )
		{
			// there is no valid compensation matrix available
			initCompensationMatrix();

			if( Printer::debugErrors() )
			{
				Com::printFLN( PSTR( "prepareZCompensation(): the compensation matrix is not available" ) );
			}
		}
	}
#endif // FEATURE_WORK_PART_Z_COMPENSATION

	if( COMPENSATION_MATRIX_SIZE > EEPROM_SECTOR_SIZE )
	{
		if( Printer::debugErrors() )
		{
			Com::printFLN( PSTR( "prepareZCompensation(): the size of the compensation matrix is too big" ) );
		}
	}

} // prepareZCompensation


void resetZCompensation( void )
{
	HAL::forbidInterrupts();

	// disable and reset the z-compensation

#if FEATURE_HEAT_BED_Z_COMPENSATION
	Printer::doHeatBedZCompensation = 0;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
	Printer::doWorkPartZCompensation = 0;
	Printer::staticCompensationZ	 = 0;
#endif // FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
	Printer::targetCompensationZ  = 0;
	Printer::currentCompensationZ = 0;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

	HAL::allowInterrupts();
	return;

} // resetZCompensation


unsigned char isSupportedCommand( unsigned int currentMCode, char neededMode, char outputLog )
{
	char	currentMode = OPERATING_MODE_PRINT;


#if FEATURE_CNC_MODE > 0
	if( Printer::operatingMode == OPERATING_MODE_CNC )
	{
		currentMode = OPERATING_MODE_CNC;
	}
#endif // FEATURE_CNC_MODE > 0

	if( currentMode == neededMode )
	{
		return 1;
	}

	if( Printer::debugErrors() && outputLog )
	{
		Com::printF( PSTR( "M" ), (int)currentMCode );
		Com::printFLN( PSTR( ": this command is not supported in this mode" ) );
	}
	return 0;

} // isSupportedCommand


unsigned char isMovingAllowed( const char* pszCommand, char outputLog )
{
#if FEATURE_HEAT_BED_Z_COMPENSATION
	if( g_nHeatBedScanStatus )
	{
		// do not allow manual movements while the heat bed scan is in progress
		if( Printer::debugErrors() && outputLog )
		{
			Com::printF( pszCommand );
			Com::printFLN( PSTR( ": this command can not be used while the heat bed scan is in progress" ) );
		}
		return 0;
	}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
		
#if FEATURE_WORK_PART_Z_COMPENSATION
	if( g_nWorkPartScanStatus )
	{
		// do not allow manual movements while the work part scan is in progress
		if( Printer::debugErrors() && outputLog )
		{
			Com::printF( pszCommand );
			Com::printFLN( PSTR( ": this command can not be used while the work part scan is in progress" ) );
		}
		return 0;
	}
#endif // FEATURE_WORK_PART_Z_COMPENSATION
		
#if FEATURE_FIND_Z_ORIGIN
	if( g_nFindZOriginStatus && g_nFindZOriginStatus != 30 )
	{
		// do not allow manual movements while the z-origin is searched
		if( Printer::debugErrors() && outputLog )
		{
			Com::printF( pszCommand );
			Com::printFLN( PSTR( ": this command can not be used while the z-origin is searched" ) );
		}
		return 0;
	}
#endif // FEATURE_FIND_Z_ORIGIN

#if FEATURE_CNC_MODE > 0
	if( Printer::ZEndstopUnknown )
	{
		// in case we do not know which Z-endstop is active at the moment, we do not allow to move until a z-homing has been performed
		if( Printer::debugErrors() && outputLog )
		{
			Com::printF( pszCommand );
			Com::printFLN( PSTR( ": this command can not be used until a z-homing has been performed" ) );
		}
		return 0;
	}
#endif // FEATURE_CNC_MODE > 0

	// we allow the manual movements at the moment
	return 1;

} // isMovingAllowed


unsigned char isHomingAllowed( GCode* com, char outputLog )
{
#if FEATURE_HEAT_BED_Z_COMPENSATION
	if( g_nHeatBedScanStatus )
	{
		// do not allow homing while the heat bed scan is in progress
		if( Printer::debugErrors() && outputLog )
		{
			Com::printFLN( PSTR( "G28: homing can not be performed while the heat bed scan is in progress" ) );
		}
		return 0;
	}
#endif // FEATURE_HEAT_BED_Z_COMPENSATION
		
#if FEATURE_WORK_PART_Z_COMPENSATION
	if( g_nWorkPartScanStatus )
	{
		// do not allow homing while the work part scan is in progress
		if( Printer::debugErrors() && outputLog )
		{
			Com::printFLN( PSTR( "G28: homing can not be performed while the work part scan is in progress" ) );
		}
		return 0;
	}
#endif // FEATURE_WORK_PART_Z_COMPENSATION
		
#if FEATURE_FIND_Z_ORIGIN
	if( g_nFindZOriginStatus )
	{
		// do not allow homing while the z-origin is searched
		if( Printer::debugErrors() && outputLog )
		{
			Com::printFLN( PSTR( "G28: homing can not be performed while the z-origin is searched" ) );
		}
		return 0;
	}
#endif // FEATURE_FIND_Z_ORIGIN

#if FEATURE_CNC_MODE > 0
	if( Printer::ZEndstopUnknown && (com->hasX() || com->hasY() || com->hasNoXYZ()) )
	{
		// in case we do not know which Z-endstop is active at the moment, we do not allow any homing except the z-homing
		if( Printer::debugErrors() && outputLog )
		{
			Com::printFLN( PSTR( "G28: x/y homing can not be performed until a z-homing has been performed" ) );
		}

		// turn off the homing in x- and y-direction
		com->setX( 0 );
		com->setY( 0 );

		if( com->hasZ() || com->hasNoXYZ() )
		{
			com->setZ( 1 );

			if( Printer::debugInfo() )
			{
				Com::printF( PSTR( "isHomingAllowed(): x=" ), com->hasX() );
				Com::printF( PSTR( ", y=" ), com->hasY() );
				Com::printFLN( PSTR( ", z=" ), com->hasZ() );
			}
			return 1;
		}

		return 0;
	}
#endif // FEATURE_CNC_MODE > 0

	// we allow the manual movements at the moment
	return 1;

} // isHomingAllowed


void showInvalidSyntax( unsigned int currentMCode )
{
	if( Printer::debugErrors() )
	{
		Com::printF( PSTR( "M" ), (int)currentMCode );
		Com::printFLN( PSTR( ": invalid syntax" ) );
	}
	return;

} // showInvalidSyntax
