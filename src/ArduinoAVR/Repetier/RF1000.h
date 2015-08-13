// Custom UI Actions

#define UI_ACTION_RF1000_MIN_REPEATABLE			 500

#define UI_ACTION_RF1000_HEAT_BED_UP			 514
#define UI_ACTION_RF1000_HEAT_BED_DOWN			 515
#define UI_ACTION_RF1000_EXTRUDER_OUTPUT		 516
#define UI_ACTION_RF1000_EXTRUDER_RETRACT		 517
#define	UI_ACTION_RF1000_SET_WORK_PART			 518
#define UI_ACTION_RF1000_SET_SCAN_DELTA_X		 519
#define UI_ACTION_RF1000_SET_SCAN_DELTA_Y		 520

#define UI_ACTION_RF1000_MAX_REPEATABLE			 600

#define UI_ACTION_RF1000_MIN_SINGLE				1500

#define UI_ACTION_RF1000_SCAN_HEAT_BED			1512
#define UI_ACTION_RF1000_TEST_COMPENSATION		1513
#define UI_ACTION_RF1000_PAUSE					1518
#define UI_ACTION_RF1000_CONTINUE				1519
#define UI_ACTION_RF1000_DO_HEAT_BED_SCAN		1520
#define UI_ACTION_RF1000_PARK					1521
#define UI_ACTION_RF1000_RESET					1522
#define UI_ACTION_RF1000_OUTPUT_OBJECT			1523
#define UI_ACTION_RF1000_FIND_Z_ORIGIN			1524
#define UI_ACTION_RF1000_DO_WORK_PART_SCAN		1525
#define UI_ACTION_RF1000_SET_SCAN_XY_START		1526
#define UI_ACTION_RF1000_SET_SCAN_XY_END		1527

#define UI_ACTION_RF1000_MAX_SINGLE				1600

/* Custom M Codes

// the following M Codes are for the heat bed scan in operating mode "print"
- M3000 - turn the z-compensation off
- M3001 - turn the z-compensation on
- M3002 - configure the min z-compensation offset (units are [steps])
- M3003 - configure the max z-compensation offset (units are [steps])
- M3004 - obsolete (use M3006 from now on and consider the changed meaning of the configured value)
- M3006 - configure the static z-offset (units are [um])
- M3007 - configure the min z-compensation offset (units are [um])
- M3008 - configure the max z-compensation offset (units are [um])

- M3010 - start/abort the heat bed scan
- M3011 - clear the z-compensation matrix from the EEPROM
- M3012 - restore the default scan parameters
- M3013 - output the current compensation matrix

- M3020 - configure the x start position for the heat bed scan (units are [mm])
- M3021 - configure the y start position for the heat bed scan (units are [mm])
- M3022 - configure the x step size for the heat bed scan (units are [mm])
- M3023 - configure the y step size for the heat bed scan (units are [mm])
- M3024 - configure the x end position for the heat bed scan (units are [mm])
- M3025 - configure the y end position for the heat bed scan (units are [mm])

- M3030 - configure the fast step size for moving of the heat bed up during the heat bed scan
- M3031 - configure the slow step size for moving of the heat bed up during the heat bed scan
- M3032 - configure the fast step size for moving of the heat bed down during the heat bed scan
- M3033 - configure the slow step size for moving of the heat bed down during the heat bed scan

- M3040 - configure the delay (in ms) between two fast movements during the heat bed scan
- M3041 - configure the delay (in ms) between two slow movements during the heat bed scan
- M3042 - configure the delay (in ms) between reaching of a new x/y position and the test of the idle pressure

- M3050 - configure the contact pressure delta (units are [digits])
- M3051 - configure the retry pressure delta (units are [digits])
- M3052 - configure the idle pressure tolerance (units are [digits])
- M3053 - configure the number of A/D converter reads per pressure measurement
- M3054 - configure the delay between two A/D converter reads (units are [ms])
- M3055 - configure the pressure tolerance per pressure measurement (units are [digits])


// the following M Codes are for the general configuration
- M3005 - enable custom debug outputs

- M3070 - pause the print as if the "Pause" button would have been pressed
- M3071 - wait until the print has been continued via the "Continue" button

- M3079 - output the printed object
- M3080 - park the printer

- M3090 - test the watchdog (this command resets the firmware)
- M3091 - erase the external EEPROM

- M3100 - configure the number of manual z steps after the "Heat Bed up" or "Heat Bed down" button has been pressed
- M3101 - configure the number of manual extruder steps after the "Extruder output" or "Extruder retract" button has been pressed
- M3102 - configure the offset in x, y and z direction as well as the extruder retract which shall be applied in case the "Pause Printing" button has been pressed (units are [steps])
- M3103 - configure the x, y and z position which shall set when the printer is parked
- M3105 - configure the offset in x, y and z direction as well as the extruder retract which shall be applied in case the "Pause Printing" button has been pressed (units are [mm])

- M3110 - lock the current status text
- M3115 - set the x/y origin to the current x/y position

- M3120 - turn on the case fan
- M3121 - turn off the case fan


// the following M Codes are for the work part scan in operating mode "mill"
- M3130 - start/stop the search of the z-origin

- M3140 - turn the z-compensation off
- M3141 - turn the z-compensation on
- M3146 - configure the static z-offset (units are [um])
- M3149 - get/choose the active work part z-compensation matrix

- M3150 - start/abort the work part scan
- M3151 - clear the specified z-compensation matrix from the EEPROM
- M3152 - restore the default scan parameters
- M3153 - output the specified work part z-compensation matrix

- M3160 - configure the x start position for the work part scan (units are [mm])
- M3161 - configure the y start position for the work part scan (units are [mm])
- M3162 - configure the x step size for the work part scan (units are [mm])
- M3163 - configure the y step size for the work part scan (units are [mm])
- M3164 - configure the x end position for the work part scan (units are [mm])
- M3165 - configure the y end position for the work part scan (units are [mm])


// other M Codes
- M3200 - reserved for test and debug
*/

/* Structure of the external EEPROM (32.768 bytes)

we use blocks of 2 kByte size for the structure of our EEPROM

00000 ... 02047 bytes [2 kBytes] = general information
  00000 [2 bytes] EEPROM format
  00002 [2 bytes] active work part compensation matrix (1 ... EEPROM_MAX_WORK_PART_SECTORS)

02048 ... 04095 bytes [2 kBytes]  = heat bed compensation matrix
  02048 [2 bytes] x-dimension of the heat bed compensation matrix
  02050 [2 bytes] y-dimension of the heat bed compensation matrix
  02052 [2 bytes] used micro steps
  02054 [12 bytes] reserved
  02066 [x bytes] heat bed compensation matrix

04096 ... 06143 bytes [2 kBytes]  = work part compensation matrix 1
  04096 [2 bytes] x-dimension of the work part compensation matrix 1
  04098 [2 bytes] y-dimension of the work part compensation matrix 1
  04100 [2 bytes] used micro steps
  04102 [2 bytes] x start position of the work part scan [mm]
  04104 [2 bytes] y start position of the work part scan [mm]
  04106 [2 bytes] x step size of the work part scan [mm]
  04108 [2 bytes] y step size of the work part scan [mm]
  04110 [2 bytes] x end position of the work part scan [mm]
  04112 [2 bytes] y end position of the work part scan [mm]
  04114 [x bytes] work part compensation matrix 1

06144 ... 08191 bytes [2 kBytes]  = work part compensation matrix 2
  ...

*/


#define	SCAN_STRAIN_GAUGE					ACTIVE_STRAIN_GAUGE
#define EEPROM_DELAY						10																// [ms]
#define	EEPROM_SECTOR_SIZE					2048															// [bytes]
#define	EEPROM_MAX_WORK_PART_SECTORS		9

#define EEPROM_OFFSET_HEADER_FORMAT			0																// [bytes]
#define EEPROM_OFFSET_ACTIVE_WORK_PART		2																// [bytes]

#define EEPROM_FORMAT						6


#if FEATURE_HEAT_BED_Z_COMPENSATION

#define	HEAT_BED_SCAN_DELAY					1000															// [ms]

#endif // FEATURE_HEAT_BED_Z_COMPENSATION


#if FEATURE_WORK_PART_Z_COMPENSATION

#define	WORK_PART_SCAN_DELAY				1000															// [ms]

#endif // FEATURE_WORK_PART_Z_COMPENSATION


#if FEATURE_HEAT_BED_Z_COMPENSATION

// determine the maximal needed size for the heat bed compensation
// in case also FEATURE_WORK_PART_Z_COMPENSATION is enabled, only the defined dimensions for the heat bed scan count (so it must be ensured that the dimensions of the heat bed compensation matrix are at least of the size of the work part compensation matrix)
#define	COMPENSATION_MATRIX_MAX_X			long((X_MAX_LENGTH - HEAT_BED_SCAN_X_START_MM - HEAT_BED_SCAN_X_END_MM) / HEAT_BED_SCAN_X_STEP_SIZE_MIN_MM + 4)
#define	COMPENSATION_MATRIX_MAX_Y			long((Y_MAX_LENGTH - HEAT_BED_SCAN_Y_START_MM - HEAT_BED_SCAN_Y_END_MM) / HEAT_BED_SCAN_Y_STEP_SIZE_MIN_MM + 4)

#define	COMPENSATION_MATRIX_SIZE			long(COMPENSATION_MATRIX_MAX_X * COMPENSATION_MATRIX_MAX_Y * 2 + EEPROM_OFFSET_MAXTRIX_START)	// [bytes]

#elif FEATURE_WORK_PART_Z_COMPENSATION

// determine the maximal needed size for the work part compensation
#define	COMPENSATION_MATRIX_MAX_X			long((X_MAX_LENGTH - WORK_PART_SCAN_X_START_MM - WORK_PART_SCAN_X_END_MM) / WORK_PART_SCAN_X_STEP_SIZE_MIN_MM + 4)
#define	COMPENSATION_MATRIX_MAX_Y			long((Y_MAX_LENGTH - WORK_PART_SCAN_Y_START_MM - WORK_PART_SCAN_Y_END_MM) / WORK_PART_SCAN_Y_STEP_SIZE_MIN_MM + 4)

#define	COMPENSATION_MATRIX_SIZE			long(COMPENSATION_MATRIX_MAX_X * COMPENSATION_MATRIX_MAX_Y * 2 + EEPROM_OFFSET_MAXTRIX_START)	// [bytes]

#endif // FEATURE_HEAT_BED_Z_COMPENSATION && FEATURE_WORK_PART_Z_COMPENSATION


#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
#define	EEPROM_OFFSET_SECTOR_FORMAT			0
#define EEPROM_OFFSET_DIMENSION_X			2
#define EEPROM_OFFSET_DIMENSION_Y			4
#define	EEPROM_OFFSET_MICRO_STEPS			6
#define	EEPROM_OFFSET_X_MIN					8																
#define	EEPROM_OFFSET_X_MAX					10															
#define	EEPROM_OFFSET_Y_MIN					12																
#define	EEPROM_OFFSET_Y_MAX					14																
#define EEPROM_OFFSET_X_START_MM			16
#define EEPROM_OFFSET_Y_START_MM			18
#define EEPROM_OFFSET_X_STEP_MM				20
#define EEPROM_OFFSET_Y_STEP_MM				22
#define EEPROM_OFFSET_X_END_MM				24
#define EEPROM_OFFSET_Y_END_MM				26
#define EEPROM_OFFSET_MAXTRIX_START			28
#endif // FEATURE_WORK_PART_Z_COMPENSATION


#define XYZ_DIRECTION_CHANGE_DELAY			250																// [us]
#define XYZ_STEPPER_HIGH_DELAY				250																// [us]
#define XYZ_STEPPER_LOW_DELAY				250																// [us]
#define EXTRUDER_DIRECTION_CHANGE_DELAY		250																// [us]
#define EXTRUDER_STEPPER_HIGH_DELAY			40000															// [us]
#define EXTRUDER_STEPPER_LOW_DELAY			250																// [us]
#define	LOOP_INTERVAL						500																// [ms]

#define	TASK_NO_TASK						-1
#define	TASK_ENABLE_Z_COMPENSATION			1
#define	TASK_DISABLE_Z_COMPENSATION			2
#define	TASK_INIT_Z_COMPENSATION			3
#define TASK_PAUSE_PRINT_1					4
#define TASK_PAUSE_PRINT_2					5


extern	char			g_nMainDirectionX;			// this is the current x-direction from the processing of G-Codes
extern	char			g_nMainDirectionY;			// this is the current y-direction from the processing of G-Codes
extern	char			g_nMainDirectionZ;			// this is the current z-direction from the processing of G-Codes
extern	char			g_nMainDirectionE;			// this is the current e-direction frmo the processing of G-Codes
extern	char			g_nBlockZ;

#if FEATURE_HEAT_BED_Z_COMPENSATION
extern	long			g_offsetZCompensationSteps;	// this is the minimal distance between the heat bed and the extruder at the moment when the z-min endstop is hit
extern	long			g_minZCompensationSteps;
extern	long			g_maxZCompensationSteps;
extern	long			g_diffZCompensationSteps;
extern	char			g_nHeatBedScanStatus;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
extern	char			g_nWorkPartScanStatus;
extern	char			g_nWorkPartScanMode;		// 0 = do not home z-axis, 1 = home z-axis
extern	char			g_nActiveWorkPart;
#endif // FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
extern	char			g_abortZScan;
extern	short			g_ZCompensationMatrix[COMPENSATION_MATRIX_MAX_X][COMPENSATION_MATRIX_MAX_Y];
extern	unsigned char	g_uZMatrixMaxX;
extern	unsigned char	g_uZMatrixMaxY;
extern	long			g_nZScanZPosition;

extern	long			g_nScanXStepSizeMm;
extern	long			g_nScanXStepSizeSteps;
extern	long			g_nScanYStepSizeMm;
extern	long			g_nScanYStepSizeSteps;
#endif // #if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

extern	long			g_staticZSteps;
extern	char			g_debugLevel;
extern	char			g_debugLog;
//extern	short			g_debugCounter[10];
extern	unsigned long	g_uStopTime;
extern	unsigned long	g_uBlockCommands;
extern	short			g_debugInt16;
extern	unsigned short	g_debugUInt16;
extern	long			g_debugInt32;

// other configurable parameters
#if FEATURE_EXTENDED_BUTTONS
extern	unsigned long	g_nManualZSteps;
extern	unsigned long	g_nManualExtruderSteps;
#endif // FEATURE_EXTENDED_BUTTONS

#if FEATURE_PAUSE_PRINTING
extern	long			g_nPauseStepsX;
extern	long			g_nPauseStepsY;
extern	long			g_nPauseStepsZ;
extern	long			g_nPauseStepsExtruder;
extern	long			g_nContinueStepsX;
extern	long			g_nContinueStepsY;
extern	long			g_nContinueStepsZ;
extern	long			g_nContinueStepsExtruder;
extern	char			g_preparePause;
extern	char			g_pausePrint;
extern	char			g_printingPaused;
extern	unsigned long	g_uPauseTime;
extern	char			g_pauseBeepDone;
#endif // FEATURE_PAUSE_PRINTING

#if FEATURE_FIND_Z_ORIGIN
extern	char			g_nFindZOriginStatus;
extern	long			g_nZOriginXPosition;
extern	long			g_nZOriginYPosition;
extern	long			g_nZOriginZPosition;
#endif // FEATURE_FIND_Z_ORIGIN


#if DEBUG_HEAT_BED_Z_COMPENSATION
extern	long			g_nNonCompensatedPositionSteps[3];
extern	long			g_nTargetCompensationZ;
extern	long			g_nZCompensationUpdates;
extern	long			g_nDelta[2];
extern	long			g_nStepSize[2];
extern	long			g_nTempXFront;
extern	long			g_nTempXBack;
extern	long			g_nNeededZ;
extern	unsigned char	g_uIndex[4];
extern	short			g_nMatrix[4];
extern	long			g_nZDeltaMin;
extern	long			g_nZDeltaMax;
extern	long			g_nZCompensationUpdateTime;
extern	long			g_nZCompensationDelayMax;
extern	long			g_nTooFast;
#endif // DEBUG_HEAT_BED_Z_COMPENSATION

// initRF1000()
extern void initRF1000( void );

// initStrainGauge()
extern void initStrainGauge( void );

// readStrainGauge()
extern short readStrainGauge( unsigned char uAddress );

#if FEATURE_HEAT_BED_Z_COMPENSATION
// startHeatBedScan()
extern void startHeatBedScan( void );

// scanHeatBed()
extern void scanHeatBed( void );

// testExtruderTemperature()
extern short testExtruderTemperature( void );

// testHeatBedTemperature()
extern short testHeatBedTemperature( void );

// doHeatBedZCompensation()
extern void doHeatBedZCompensation( void );
#endif // FEATURE_HEAT_BED_Z_COMPENSATION


#if FEATURE_WORK_PART_Z_COMPENSATION
// startWorkPartScan()
extern void startWorkPartScan( char nMode );

// scanWorkPart()
extern void scanWorkPart( void );

// doWorkPartZCompensation()
extern void doWorkPartZCompensation( void );

// determineStaticCompensationZ()
extern void determineStaticCompensationZ( void );
#endif // FEATURE_WORK_PART_Z_COMPENSATION


#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
// readIdlePressure()
extern short readIdlePressure( short* pnIdlePressure );

// testIdlePressure()
extern short testIdlePressure( void );

// readAveragePressure()
extern short readAveragePressure( short* pnAveragePressure );

// moveZUpFast()
extern short moveZUpFast( void );

// moveZDownSlow()
extern short moveZDownSlow( void );

// moveZUpSlow()
extern short moveZUpSlow( short* pnContactPressure, char* pnRetry );

// moveZDownFast()
extern short moveZDownFast( void );

// moveZ()
extern int moveZ( int nSteps );

// moveExtruder()
extern int moveExtruder( int nSteps );

// restoreDefaultScanParameters()
extern void restoreDefaultScanParameters( void );

// outputScanParameters()
extern void outputScanParameters( void );

// outputCompensationMatrix()
extern void outputCompensationMatrix( void );

// initCompensationMatrix()
extern void initCompensationMatrix( void );

// prepareCompensationMatrix()
extern char prepareCompensationMatrix( void );

// convertCompensationMatrix()
extern char convertCompensationMatrix( void );

// saveCompensationMatrix()
extern char saveCompensationMatrix( unsigned int uAddress );

// loadCompensationMatrix()
extern char loadCompensationMatrix( unsigned int uAddress );

// clearCompensationMatrix()
extern void clearCompensationMatrix( unsigned int uAddress );

// outputPressureMatrix()
extern void outputPressureMatrix( void );
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

// clearExternalEEPROM()
extern char clearExternalEEPROM( void );

// writeByte24C256()
extern void writeByte24C256( int addressI2C, unsigned int addressEEPROM, unsigned char data );

// writeWord24C256()
extern void writeWord24C256( int addressI2C, unsigned int addressEEPROM, unsigned short data );

// readByte24C256()
extern unsigned char readByte24C256( int addressI2C, unsigned int addressEEPROM );

// readWord24C256()
extern unsigned short readWord24C256( int addressI2C, unsigned int addressEEPROM );

// doZCompensation()
extern void doZCompensation( void );

// loopRF1000()
extern void loopRF1000( void );

#if FEATURE_OUTPUT_PRINTED_OBJECT
// outputObject()
extern void outputObject( void );
#endif // FEATURE_OUTPUT_PRINTED_OBJECT

#if FEATURE_PARK
// parkPrinter()
extern void parkPrinter( void );
#endif // FEATURE_PARK

// pausePrint()
extern void pausePrint( void );

// continuePrint()
extern void continuePrint( void );

#if FEATURE_PAUSE_PRINTING
// determinePausePosition()
extern void determinePausePosition( void );

// waitUntilContinue
extern void waitUntilContinue( void );
#endif // FEATURE_PAUSE_PRINTING

// setExtruderCurrent()
extern void setExtruderCurrent( unsigned short level );

// processCommand()
extern void processCommand( GCode* pCommand );

// runStandardTasks()
extern void runStandardTasks( void );

// queueTask()
extern void queueTask( char task );

// processButton()
extern void processButton( int nAction );

// nextPreviousZAction()
extern void nextPreviousZAction( int8_t next );


#if STEPPER_CURRENT_CONTROL==CURRENT_CONTROL_LTC2600

// setMotorCurrent()
extern void setMotorCurrent( uint8_t channel, unsigned short level );

// motorCurrentControlInit()
extern void motorCurrentControlInit( void );

#endif // CURRENT_CONTROL_LTC2600


#if STEPPER_CURRENT_CONTROL==CURRENT_CONTROL_DRV8711

// setMotorCurrent()
extern void setMotorCurrent( unsigned char driver, unsigned short level );

// motorCurrentControlInit()
extern void motorCurrentControlInit( void );

#endif // CURRENT_CONTROL_DRV8711


// cleanupXPositions
extern void cleanupXPositions( void );

// cleanupYPositions
extern void cleanupYPositions( void );

// cleanupZPositions
extern void cleanupZPositions( void );

// cleanupEPositions
extern void cleanupEPositions( void );

// setZOrigin()
extern void setZOrigin( void );


#if FEATURE_FIND_Z_ORIGIN

// startFindZOrigin()
extern void startFindZOrigin( void );

// findZOrigin()
extern void findZOrigin( void );

#endif // FEATURE_FIND_Z_ORIGIN


#if FEATURE_CNC_MODE > 0

// switchOperatingMode()
extern void switchOperatingMode( char newOperatingMode );

// switchActiveWorkPart()
extern void switchActiveWorkPart( char newActiveWorkPart );

// setScanXYStart()
extern void setScanXYStart( void );

// setScanXYEnd()
extern void setScanXYEnd( void );

#endif // FEATURE_CNC_MODE > 0


// setupForPrinting
extern void setupForPrinting( void );

// setupForMilling()
extern void setupForMilling( void );

// prepareZCompensation()
extern void prepareZCompensation( void );

// resetZCompensation()
extern void resetZCompensation( void );

// isSupportedCommand()
extern unsigned char isSupportedCommand( unsigned int currentMCode, char neededMode, char outputLog = 1 );

// isMovingAllowed()
extern unsigned char isMovingAllowed( const char* pszCommand, char outputLog = 1 );

// isHomingAllowed()
extern unsigned char isHomingAllowed( GCode* com, char outputLog = 1 );

// showInvalidSyntax()
extern void showInvalidSyntax( unsigned int currentMCode );