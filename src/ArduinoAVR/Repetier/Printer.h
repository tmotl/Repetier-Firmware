/*
    This file is part of Repetier-Firmware.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

    This firmware is a nearly complete rewrite of the sprinter firmware
    by kliment (https://github.com/kliment/Sprinter)
    which based on Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.
*/

#ifndef PRINTER_H_INCLUDED
#define PRINTER_H_INCLUDED

union floatLong {
    float f;
    long l;
};

#define PRINTER_FLAG0_STEPPER_DISABLED      1
#define PRINTER_FLAG0_SEPERATE_EXTRUDER_INT 2
#define PRINTER_FLAG0_TEMPSENSOR_DEFECT     4
#define PRINTER_FLAG0_FORCE_CHECKSUM        8
#define PRINTER_FLAG0_MANUAL_MOVE_MODE      16
#define PRINTER_FLAG0_AUTOLEVEL_ACTIVE      32
#define PRINTER_FLAG0_ZPROBEING             64
#define PRINTER_FLAG0_LARGE_MACHINE         128
#define PRINTER_FLAG1_HOMED                 1
#define PRINTER_FLAG1_AUTOMOUNT             2
#define PRINTER_FLAG1_ANIMATION             4
#define PRINTER_FLAG1_ALLKILLED             8
#define PRINTER_FLAG1_UI_ERROR_MESSAGE      16
#define PRINTER_FLAG1_NO_DESTINATION_CHECK  32
#define PRINTER_FLAG1_Z_ORIGIN_SET			64

class Printer
{
public:
#if defined(USE_ADVANCE)
    static volatile int extruderStepsNeeded; ///< This many extruder steps are still needed, <0 = reverse steps needed.
    static uint8_t minExtruderSpeed;            ///< Timer delay for start extruder speed
    static uint8_t maxExtruderSpeed;            ///< Timer delay for end extruder speed
    //static uint8_t extruderAccelerateDelay;     ///< delay between 2 speec increases
    static int advanceStepsSet;
#ifdef ENABLE_QUADRATIC_ADVANCE
    static long advanceExecuted;             ///< Executed advance steps
#endif
#endif
    static uint8_t menuMode;
    static float axisStepsPerMM[];
    static float invAxisStepsPerMM[];
    static float maxFeedrate[];
    static float homingFeedrate[];
    static float maxAccelerationMMPerSquareSecond[];
    static float maxTravelAccelerationMMPerSquareSecond[];
    static unsigned long maxPrintAccelerationStepsPerSquareSecond[];
    static unsigned long maxTravelAccelerationStepsPerSquareSecond[];
    static uint8_t relativeCoordinateMode;    ///< Determines absolute (false) or relative Coordinates (true).
    static uint8_t relativeExtruderCoordinateMode;  ///< Determines Absolute or Relative E Codes while in Absolute Coordinates mode. E is always relative in Relative Coordinates mode.

    static uint8_t unitIsInches;

    static uint8_t debugLevel;
    static uint8_t flag0,flag1; // 1 = stepper disabled, 2 = use external extruder interrupt, 4 = temp Sensor defect, 8 = homed
    static uint8_t stepsPerTimerCall;
    static unsigned long interval;    ///< Last step duration in ticks.
    static unsigned long timer;              ///< used for acceleration/deceleration timing
    static unsigned long stepNumber;         ///< Step number in current move.
    static float coordinateOffset[3];
    static long currentPositionSteps[4];     ///< Position in steps from origin.
    static float currentPosition[3];
    static float lastCmdPos[3]; ///< Last coordinates send by gcodes
    static long destinationSteps[4];         ///< Target position in steps.
#if NONLINEAR_SYSTEM
    static long currentDeltaPositionSteps[4];
    static long maxDeltaPositionSteps;
    static floatLong deltaDiagonalStepsSquaredA;
    static floatLong deltaDiagonalStepsSquaredB;
    static floatLong deltaDiagonalStepsSquaredC;
    static float deltaMaxRadiusSquared;
    static long deltaAPosXSteps;
    static long deltaAPosYSteps;
    static long deltaBPosXSteps;
    static long deltaBPosYSteps;
    static long deltaCPosXSteps;
    static long deltaCPosYSteps;
    static long realDeltaPositionSteps[3];
    static int16_t travelMovesPerSecond;
    static int16_t printMovesPerSecond;
#endif
#if FEATURE_Z_PROBE || MAX_HARDWARE_ENDSTOP_Z || NONLINEAR_SYSTEM
    static long stepsRemainingAtZHit;
#endif
#if DRIVE_SYSTEM==3
    static long stepsRemainingAtXHit;
    static long stepsRemainingAtYHit;
#endif
#ifdef SOFTWARE_LEVELING
    static long levelingP1[3];
    static long levelingP2[3];
    static long levelingP3[3];
#endif
#if FEATURE_AUTOLEVEL
    static float autolevelTransformation[9]; ///< Transformation matrix
#endif
    static signed char zBabystepsMissing;
    static float minimumSpeed;               ///< lowest allowed speed to keep integration error small
    static float minimumZSpeed;              ///< lowest allowed speed to keep integration error small
    static long xMaxSteps;                   ///< For software endstops, limit of move in positive direction.
    static long yMaxSteps;                   ///< For software endstops, limit of move in positive direction.
    static long zMaxSteps;                   ///< For software endstops, limit of move in positive direction.
    static long xMinSteps;                   ///< For software endstops, limit of move in negative direction.
    static long yMinSteps;                   ///< For software endstops, limit of move in negative direction.
    static long zMinSteps;                   ///< For software endstops, limit of move in negative direction.
    static float xLength;
    static float xMin;
    static float yLength;
    static float yMin;
    static float zLength;
    static float zMin;
    static float feedrate;                   ///< Last requested feedrate.
    static int feedrateMultiply;             ///< Multiplier for feedrate in percent (factor 1 = 100)
    static unsigned int extrudeMultiply;     ///< Flow multiplier in percdent (factor 1 = 100)
    static float maxJerk;                    ///< Maximum allowed jerk in mm/s
#if DRIVE_SYSTEM != 3
    static float maxZJerk;                   ///< Maximum allowed jerk in z direction in mm/s
#endif
    static float offsetX;                    ///< X-offset for different extruder positions.
    static float offsetY;                    ///< Y-offset for different extruder positions.
    static speed_t vMaxReached;              ///< Maximumu reached speed
    static unsigned long msecondsPrinting;   ///< Milliseconds of printing time (means time with heated extruder)
    static float filamentPrinted;            ///< mm of filament printed since counting started
    static uint8_t wasLastHalfstepping;      ///< Indicates if last move had halfstepping enabled
#if ENABLE_BACKLASH_COMPENSATION
    static float backlashX;
    static float backlashY;
    static float backlashZ;
    static uint8_t backlashDir;
#endif
#ifdef DEBUG_STEPCOUNT
    static long totalStepsRemaining;
#endif
#if FEATURE_MEMORY_POSITION
    static float memoryX;
    static float memoryY;
    static float memoryZ;
    static float memoryE;
#endif
#ifdef XY_GANTRY
    static char motorX;
    static char motorY;
#endif
#ifdef DEBUG_SEGMENT_LENGTH
    static float maxRealSegmentLength;
#endif
#ifdef DEBUG_REAL_JERK
    static float maxRealJerk;
#endif

#if FEATURE_HEAT_BED_Z_COMPENSATION
    static char	doHeatBedZCompensation;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION

#if FEATURE_WORK_PART_Z_COMPENSATION
    static char	doWorkPartZCompensation;
	static long staticCompensationZ;			// this is the z-delta which can occur in case the x/y position of the z-origin from the work part scan is different to the x/y position of the z-origin from the moment of the start of the milling
#endif // FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION
    static long nonCompensatedPositionStepsX;
    static long nonCompensatedPositionStepsY;
    static long	nonCompensatedPositionStepsZ;
    static long targetCompensationZ;
    static long currentCompensationZ;
#endif // FEATURE_HEAT_BED_Z_COMPENSATION || FEATURE_WORK_PART_Z_COMPENSATION

#if FEATURE_EXTENDED_BUTTONS || FEATURE_PAUSE_PRINTING
    static long targetPositionStepsX;
    static long targetPositionStepsY;
    static long targetPositionStepsZ;
    static long targetPositionStepsE;
    static long currentPositionStepsX;
    static long currentPositionStepsY;
    static long currentPositionStepsZ;
    static long currentPositionStepsE;
#endif // FEATURE_EXTENDED_BUTTONS || FEATURE_PAUSE_PRINTING

#if FEATURE_CNC_MODE > 0
	static char operatingMode;
	static char ZEndstopType;
	static char	ZEndstopUnknown;
#endif // FEATURE_CNC_MODE > 0

#if FEATURE_CNC_MODE == 2
	static char				lastZDirection;
	static char				endstopZMinHit;
	static char				endstopZMaxHit;
	static long				stepsSinceZMinEndstop;
	static long				stepsSinceZMaxEndstop;
#endif // FEATURE_CNC_MODE == 2

#if STEPPER_ON_DELAY
	static char	enabledX;
	static char	enabledY;
	static char	enabledZ;
#endif // STEPPER_ON_DELAY

#if FEATURE_BEEPER
	static char enableBeeper;
#endif // FEATURE_BEEPER

#if defined(CASE_LIGHTS_PIN) && CASE_LIGHTS_PIN >= 0
	static char				enableLights;
#endif // CASE_LIGHTS_PIN >= 0

#if defined(CASE_FAN_PIN) && CASE_FAN_PIN >= 0
	static unsigned long	prepareFanOff;
	static unsigned long	fanOffDelay;
#endif // CASE_FAN_PIN >= 0

	static inline void setMenuMode(uint8_t mode,bool on)
	{
        if(on)
            menuMode |= mode;
        else
            menuMode &= ~mode;
    }
    static inline bool isMenuMode(uint8_t mode)
	{
        return (menuMode & mode)==mode;
    }
    static inline bool debugEcho()
    {
        return ((debugLevel & 1)!=0);
    }
    static inline bool debugInfo()
    {
        return ((debugLevel & 2)!=0);
    }
    static inline bool debugErrors()
    {
        return ((debugLevel & 4)!=0);
    }
    static inline bool debugDryrun()
    {
        return ((debugLevel & 8)!=0);
    }
    static inline bool debugCommunication()
    {
        return ((debugLevel & 16)!=0);
    }
    static inline bool debugNoMoves() {
        return ((debugLevel & 32)!=0);
    }

    /** \brief Disable stepper motor for x direction. */
    static inline void disableXStepper()
    {
#if (X_ENABLE_PIN > -1)
        WRITE(X_ENABLE_PIN,!X_ENABLE_ON);
#endif
#if FEATURE_TWO_XSTEPPER && (X2_ENABLE_PIN > -1)
        WRITE(X2_ENABLE_PIN,!X_ENABLE_ON);
#endif

#if STEPPER_ON_DELAY
		Printer::enabledX = 0;
#endif // STEPPER_ON_DELAY

		// when the stepper is disabled we loose our home position because somebody else can move our mechanical parts
		setHomed(false);
		cleanupXPositions();
	}
    /** \brief Disable stepper motor for y direction. */
    static inline void disableYStepper()
    {
#if (Y_ENABLE_PIN > -1)
        WRITE(Y_ENABLE_PIN,!Y_ENABLE_ON);
#endif
#if FEATURE_TWO_YSTEPPER && (Y2_ENABLE_PIN > -1)
        WRITE(Y2_ENABLE_PIN,!Y_ENABLE_ON);
#endif

#if STEPPER_ON_DELAY
		Printer::enabledY = 0;
#endif // STEPPER_ON_DELAY

		// when the stepper is disabled we loose our home position because somebody else can move our mechanical parts
		setHomed(false);
		cleanupYPositions();
	}
    /** \brief Disable stepper motor for z direction. */
    static inline void disableZStepper()
    {
#if (Z_ENABLE_PIN > -1)
        WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON);
#endif
#if FEATURE_TWO_ZSTEPPER && (Z2_ENABLE_PIN > -1)
        WRITE(Z2_ENABLE_PIN,!Z_ENABLE_ON);
#endif

#if STEPPER_ON_DELAY
		Printer::enabledZ = 0;
#endif // STEPPER_ON_DELAY

#if FEATURE_CNC_MODE == 2
		Printer::lastZDirection		   = 0;
		Printer::endstopZMinHit		   = ENDSTOP_NOT_HIT;
		Printer::endstopZMaxHit		   = ENDSTOP_NOT_HIT;
		Printer::stepsSinceZMinEndstop = 0;
		Printer::stepsSinceZMaxEndstop = 0;
#endif // FEATURE_CNC_MODE == 2

		// when the stepper is disabled we loose our home position because somebody else can move our mechanical parts
		setHomed(false);
		setZOriginSet(false);
		cleanupZPositions();
	}
    /** \brief Enable stepper motor for x direction. */
    static inline void enableXStepper()
    {
#if (X_ENABLE_PIN > -1)
        WRITE(X_ENABLE_PIN, X_ENABLE_ON);
#endif
#if FEATURE_TWO_XSTEPPER && (X2_ENABLE_PIN > -1)
        WRITE(X2_ENABLE_PIN,X_ENABLE_ON);
#endif

#if STEPPER_ON_DELAY
		if( !Printer::enabledX )
		{
			Printer::enabledX = 1;
			HAL::delayMilliseconds( STEPPER_ON_DELAY );
		}
#endif // STEPPER_ON_DELAY
    }
    /** \brief Enable stepper motor for y direction. */
    static inline void enableYStepper()
    {
#if (Y_ENABLE_PIN > -1)
        WRITE(Y_ENABLE_PIN, Y_ENABLE_ON);
#endif
#if FEATURE_TWO_YSTEPPER && (Y2_ENABLE_PIN > -1)
        WRITE(Y2_ENABLE_PIN,Y_ENABLE_ON);
#endif

#if STEPPER_ON_DELAY
		if( !Printer::enabledY )
		{
			Printer::enabledY = 1;
			HAL::delayMilliseconds( STEPPER_ON_DELAY );
		}
#endif // STEPPER_ON_DELAY
    }
    /** \brief Enable stepper motor for z direction. */
    static inline void enableZStepper()
    {
#if (Z_ENABLE_PIN > -1)
        WRITE(Z_ENABLE_PIN, Z_ENABLE_ON);
#endif
#if FEATURE_TWO_ZSTEPPER && (Z2_ENABLE_PIN > -1)
        WRITE(Z2_ENABLE_PIN,Z_ENABLE_ON);
#endif

#if STEPPER_ON_DELAY
		if( !Printer::enabledZ )
		{
			Printer::enabledZ = 1;
			HAL::delayMilliseconds( STEPPER_ON_DELAY );
		}
#endif // STEPPER_ON_DELAY
    }
    static inline void setXDirection(bool positive)
    {
        if(positive)
        {
            // extruder moves to the right
            if( g_nMainDirectionX != 1 )
            {
                WRITE(X_DIR_PIN,!INVERT_X_DIR);
#if FEATURE_TWO_XSTEPPER
                WRITE(X2_DIR_PIN,!INVERT_X_DIR);
#endif
                g_nMainDirectionX = 1;
            }
        }
        else
        {
            // extruder moves to the left
            if( g_nMainDirectionX != -1 )
            {
                WRITE(X_DIR_PIN,INVERT_X_DIR);
#if FEATURE_TWO_XSTEPPER
                WRITE(X2_DIR_PIN,INVERT_X_DIR);
#endif
                g_nMainDirectionX = -1;
            }
        }
    }
    static inline void setYDirection(bool positive)
    {
        if(positive)
        {
            // heat bed moves to the front
            if( g_nMainDirectionY != 1 )
            {
                WRITE(Y_DIR_PIN,!INVERT_Y_DIR);
#if FEATURE_TWO_YSTEPPER
                WRITE(Y2_DIR_PIN,!INVERT_Y_DIR);
#endif
                g_nMainDirectionY = 1;
            }
        }
        else
        {
            // heat bed moves to the back
            if( g_nMainDirectionY != -1 )
            {
                WRITE(Y_DIR_PIN,INVERT_Y_DIR);
#if FEATURE_TWO_YSTEPPER
                WRITE(Y2_DIR_PIN,INVERT_Y_DIR);
#endif
                g_nMainDirectionY = -1;
            }
        }
    }
    static inline void setZDirection(bool positive)
    {
		if( g_nBlockZ )
		{
			return;
		}

        if(positive)
        {
            // heat bed moves to the bottom
//            if( g_nMainDirectionZ != 1 )
            {
				// prepareBedDown() can not be called at this place
				WRITE( Z_DIR_PIN, !INVERT_Z_DIR );
#if FEATURE_TWO_ZSTEPPER
				WRITE( Z2_DIR_PIN, !INVERT_Z_DIR );
#endif

#if FEATURE_CNC_MODE == 2
				increaseLastZDirection();
#endif // FEATURE_CNC_MODE == 2

				g_nMainDirectionZ = 1;
            }
        }
        else
        {
            // heat bed moves to the top
//            if( g_nMainDirectionZ != -1 )
            {
				// prepareBedUp() can not be called at this place
				WRITE( Z_DIR_PIN, INVERT_Z_DIR );
#if FEATURE_TWO_ZSTEPPER
				WRITE( Z2_DIR_PIN, INVERT_Z_DIR );
#endif

#if FEATURE_CNC_MODE == 2
				Printer::decreaseLastZDirection();
#endif // FEATURE_CNC_MODE == 2

                g_nMainDirectionZ = -1;
            }
        }
    }
    static inline bool getZDirection()
    {
        return ((READ(Z_DIR_PIN)!=0) ^ INVERT_Z_DIR);
    }

#if FEATURE_CNC_MODE == 2
	static inline void increaseLastZDirection()
	{
		lastZDirection = 1;
	}
	static inline void decreaseLastZDirection()
	{
		lastZDirection = -1;
	}
#endif // FEATURE_CNC_MODE == 2

    static inline bool getYDirection()
    {
        return((READ(Y_DIR_PIN)!=0) ^ INVERT_Y_DIR);
    }
    static inline bool getXDirection()
    {
        return((READ(X_DIR_PIN)!=0) ^ INVERT_X_DIR);
    }
    static inline uint8_t isLargeMachine()
    {
        return flag0 & PRINTER_FLAG0_LARGE_MACHINE;
    }
    static inline void setLargeMachine(uint8_t b)
    {
        flag0 = (b ? flag0 | PRINTER_FLAG0_LARGE_MACHINE : flag0 & ~PRINTER_FLAG0_LARGE_MACHINE);
    }
    static inline uint8_t isAdvanceActivated()
    {
        return flag0 & PRINTER_FLAG0_SEPERATE_EXTRUDER_INT;
    }
    static inline void setAdvanceActivated(uint8_t b)
    {
        flag0 = (b ? flag0 | PRINTER_FLAG0_SEPERATE_EXTRUDER_INT : flag0 & ~PRINTER_FLAG0_SEPERATE_EXTRUDER_INT);
    }
    static inline uint8_t isHomed()
    {
        return flag1 & PRINTER_FLAG1_HOMED;
    }
    static inline void setHomed(uint8_t b)
    {
        flag1 = (b ? flag1 | PRINTER_FLAG1_HOMED : flag1 & ~PRINTER_FLAG1_HOMED);
    }
    static inline uint8_t isZOriginSet()
    {
        return flag1 & PRINTER_FLAG1_Z_ORIGIN_SET;
    }
    static inline void setZOriginSet(uint8_t b)
    {
        flag1 = (b ? flag1 | PRINTER_FLAG1_Z_ORIGIN_SET : flag1 & ~PRINTER_FLAG1_Z_ORIGIN_SET);
    }
    static inline uint8_t isAllKilled()
    {
        return flag1 & PRINTER_FLAG1_ALLKILLED;
    }
    static inline void setAllKilled(uint8_t b)
    {
        flag1 = (b ? flag1 | PRINTER_FLAG1_ALLKILLED : flag1 & ~PRINTER_FLAG1_ALLKILLED);
    }
    static inline uint8_t isAutomount()
    {
        return flag1 & PRINTER_FLAG1_AUTOMOUNT;
    }
    static inline void setAutomount(uint8_t b)
    {
        flag1 = (b ? flag1 | PRINTER_FLAG1_AUTOMOUNT : flag1 & ~PRINTER_FLAG1_AUTOMOUNT);
    }
    static inline uint8_t isAnimation()
    {
        return flag1 & PRINTER_FLAG1_ANIMATION;
    }
    static inline void setAnimation(uint8_t b)
    {
        flag1 = (b ? flag1 | PRINTER_FLAG1_ANIMATION : flag1 & ~PRINTER_FLAG1_ANIMATION);
    }
    static inline uint8_t isUIErrorMessage()
    {
        return flag1 & PRINTER_FLAG1_UI_ERROR_MESSAGE;
    }
    static inline void setUIErrorMessage(uint8_t b)
    {
        flag1 = (b ? flag1 | PRINTER_FLAG1_UI_ERROR_MESSAGE : flag1 & ~PRINTER_FLAG1_UI_ERROR_MESSAGE);
    }
    static inline uint8_t isNoDestinationCheck()
    {
        return flag1 & PRINTER_FLAG1_NO_DESTINATION_CHECK;
    }
    static inline void setNoDestinationCheck(uint8_t b)
    {
        flag1 = (b ? flag1 | PRINTER_FLAG1_NO_DESTINATION_CHECK : flag1 & ~PRINTER_FLAG1_NO_DESTINATION_CHECK);
    }
    static inline void toggleAnimation()
	{
        setAnimation(!isAnimation());
    }
    static inline float convertToMM(float x)
    {
        return (unitIsInches ? x*25.4 : x);
    }
    static inline bool isXMinEndstopHit()
    {
#if X_MIN_PIN>-1 && MIN_HARDWARE_ENDSTOP_X
        return READ(X_MIN_PIN) != ENDSTOP_X_MIN_INVERTING;
#else
        return false;
#endif
    }
    static inline bool isYMinEndstopHit()
    {
#if Y_MIN_PIN>-1 && MIN_HARDWARE_ENDSTOP_Y
        return READ(Y_MIN_PIN) != ENDSTOP_Y_MIN_INVERTING;
#else
        return false;
#endif
    }
    static inline bool isZMinEndstopHit()
    {
#if Z_MIN_PIN>-1 && MIN_HARDWARE_ENDSTOP_Z

#if FEATURE_CNC_MODE == 2

		if( ZEndstopType == ENDSTOP_TYPE_SINGLE )
		{
			if( operatingMode == OPERATING_MODE_PRINT )
			{
				// in case there is only one z-endstop and we are in operating mode "print", the z-min endstop must be connected
				return READ(Z_MIN_PIN) != ENDSTOP_Z_MIN_INVERTING;
			}

			// in case there is only one z-endstop and we are in operating mode "mill", the z-min endstop is not connected and can not be detected
			return false;
		}

		// we end up here in case the z-min and z-max endstops are connected in a circuit
		if( READ(Z_MIN_PIN) != ENDSTOP_Z_MIN_INVERTING )
		{
			// either the min or the max endstop is hit
			if( ZEndstopUnknown )
			{
				// this is save as long as we do not allow any movement except the z-homing while ZEndstopUnknown is != 0
				return false;
			}

			if( endstopZMaxHit == ENDSTOP_IS_HIT )
			{
				// when the z-max endstop is hit already we know that the z-min endstop is not hit
				return false;
			}

			if( endstopZMinHit == ENDSTOP_IS_HIT )
			{
				// we remember that the z-min endstop is hit at the moment
				return true;
			}

			if( stepsSinceZMaxEndstop && stepsSinceZMaxEndstop > MINIMAL_Z_ENDSTOP_MAX_TO_MIN_STEPS )
			{
				// the z-max endstop was hit a few steps ago, so the z-min endstop can not be hit right now
				return false;
			}
				
			if( lastZDirection > 0 )
			{
				// z-min was not hit and we are moving downwards, so z-min can not become hit right now
				return false;
			}

			// the last z-direction is unknown or the heat bed has been moved upwards, thus we have to assume that the z-min endstop is hit
			endstopZMinHit		  = ENDSTOP_IS_HIT;
			endstopZMaxHit		  = ENDSTOP_NOT_HIT;
			stepsSinceZMinEndstop = Z_ENDSTOP_MIN_TO_MAX_INITIAL_STEPS;
			stepsSinceZMaxEndstop = 0;
			return true;
		}

		// no z endstop is hit
		if( endstopZMinHit == ENDSTOP_IS_HIT )
		{
			endstopZMinHit = ENDSTOP_WAS_HIT;
		}
		if( endstopZMaxHit == ENDSTOP_IS_HIT )
		{
			endstopZMaxHit = ENDSTOP_WAS_HIT;
		}
		ZEndstopUnknown = 0;
		return false;

#else
        return READ(Z_MIN_PIN) != ENDSTOP_Z_MIN_INVERTING;
#endif // FEATURE_CNC_MODE == 2

#else
        return false;
#endif
    }
    static inline bool isXMaxEndstopHit()
    {
#if X_MAX_PIN>-1 && MAX_HARDWARE_ENDSTOP_X
        return READ(X_MAX_PIN) != ENDSTOP_X_MAX_INVERTING;
#else
        return false;
#endif
    }
    static inline bool isYMaxEndstopHit()
    {
#if Y_MAX_PIN>-1 && MAX_HARDWARE_ENDSTOP_Y
        return READ(Y_MAX_PIN) != ENDSTOP_Y_MAX_INVERTING;
#else
        return false;
#endif
    }
    static inline bool isZMaxEndstopHit()
    {
#if Z_MAX_PIN>-1 && MAX_HARDWARE_ENDSTOP_Z

#if FEATURE_CNC_MODE == 2

		if( ZEndstopType == ENDSTOP_TYPE_SINGLE )
		{
			if( operatingMode == OPERATING_MODE_CNC )
			{
				// in case there is only one z-endstop and we are in operating mode "mill", the z-max endstop must be connected
				return READ(Z_MAX_PIN) != ENDSTOP_Z_MAX_INVERTING;
			}

			// in case there is only one z-endstop and we are in operating mode "print", the z-max endstop is not connected and can not be detected
			return false;
		}

		// we end up here in case the z-min and z-max endstops are connected in a circuit
		if( READ(Z_MAX_PIN) != ENDSTOP_Z_MAX_INVERTING )
		{
			// either the min or the max endstop is hit
			if( ZEndstopUnknown )
			{
				// this is save as long as we do not allow any movement except the z-homing while ZEndstopUnknown is != 0
				return false;
			}

			if( endstopZMinHit == ENDSTOP_IS_HIT )
			{
				// when the z-min endstop is hit already we know that the z-max endstop is not hit
				return false;
			}

			if( endstopZMaxHit == ENDSTOP_IS_HIT )
			{
				// we remember that the z-max endstop is hit at the moment
				return true;
			}
				
			if( stepsSinceZMinEndstop && stepsSinceZMinEndstop < MINIMAL_Z_ENDSTOP_MIN_TO_MAX_STEPS )
			{
				// the z-min endstop was hit a few steps ago, so the z-max endstop can not be hit right now
				return false;
			}
				
			if( lastZDirection < 0 )
			{
				// z-max was not hit and we are moving upwards, so z-max can not become hit right now
				return false;
			}

//			g_debugInt32 = stepsSinceZMinEndstop;

			// the last z-direction is unknown or the heat bed has been moved downwards, thus we have to assume that the z-max endstop is hit
			endstopZMinHit		  = ENDSTOP_NOT_HIT;
			endstopZMaxHit		  = ENDSTOP_IS_HIT;
			stepsSinceZMinEndstop = 0;
			stepsSinceZMaxEndstop = Z_ENDSTOP_MAX_TO_MIN_INITIAL_STEPS;
			return true;
		}

		// no z endstop is hit
		if( endstopZMinHit == ENDSTOP_IS_HIT )
		{
			endstopZMinHit = ENDSTOP_WAS_HIT;
		}
		if( endstopZMaxHit == ENDSTOP_IS_HIT )
		{
			endstopZMaxHit = ENDSTOP_WAS_HIT;
		}
		ZEndstopUnknown = 0;
		return false;

#else
		return READ(Z_MAX_PIN) != ENDSTOP_Z_MAX_INVERTING;
#endif // FEATURE_CNC_MODE == 2

#else
        return false;
#endif
    }
    static inline bool areAllSteppersDisabled()
    {
        return flag0 & PRINTER_FLAG0_STEPPER_DISABLED;
    }
    static inline void setAllSteppersDisabled()
    {
        flag0 |= PRINTER_FLAG0_STEPPER_DISABLED;

		// when the stepper is disabled we loose our home position because somebody else can move our mechanical parts
		setHomed(false);
		setZOriginSet(false);
    }
    static inline void unsetAllSteppersDisabled()
    {
        flag0 &= ~PRINTER_FLAG0_STEPPER_DISABLED;
#if FAN_BOARD_PIN>-1
        pwm_pos[NUM_EXTRUDER+1] = 255;
#endif // FAN_BOARD_PIN
    }
    static inline bool isAnyTempsensorDefect()
    {
#if FEATURE_CNC_MODE > 0
		if( Printer::operatingMode != OPERATING_MODE_PRINT )
		{
			// we do not support temperature sensors in case we are not in operating mode print
			return 0;
		}
#endif // FEATURE_CNC_MODE > 0

		return (flag0 & PRINTER_FLAG0_TEMPSENSOR_DEFECT);
    }
    static inline bool isManualMoveMode()
    {
        return (flag0 & PRINTER_FLAG0_MANUAL_MOVE_MODE);
    }
    static inline void setManualMoveMode(bool on)
    {
        flag0 = (on ? flag0 | PRINTER_FLAG0_MANUAL_MOVE_MODE : flag0 & ~PRINTER_FLAG0_MANUAL_MOVE_MODE);
    }
    static inline bool isAutolevelActive()
    {
        return (flag0 & PRINTER_FLAG0_AUTOLEVEL_ACTIVE)!=0;
    }
    static void setAutolevelActive(bool on);
    static inline void setZProbingActive(bool on)
    {
        flag0 = (on ? flag0 | PRINTER_FLAG0_ZPROBEING : flag0 & ~PRINTER_FLAG0_ZPROBEING);
    }
    static inline bool isZProbingActive()
    {
        return (flag0 & PRINTER_FLAG0_ZPROBEING);
    }
    static inline bool isZProbeHit()
    {
#if FEATURE_Z_PROBE
        return (Z_PROBE_ON_HIGH ? READ(Z_PROBE_PIN) : !READ(Z_PROBE_PIN));
#else
        return false;
#endif
    }
    static inline void executeXYGantrySteps()
    {
#if defined(XY_GANTRY)
        if(motorX <= -2)
        {
            ANALYZER_ON(ANALYZER_CH2);
            WRITE(X_STEP_PIN,HIGH);
#if FEATURE_TWO_XSTEPPER
            WRITE(X2_STEP_PIN,HIGH);
#endif
            motorX += 2;
        }
        else if(motorX >= 2)
        {
            ANALYZER_ON(ANALYZER_CH2);
            WRITE(X_STEP_PIN,HIGH);
#if FEATURE_TWO_XSTEPPER
            WRITE(X2_STEP_PIN,HIGH);
#endif
            motorX -= 2;
        }
        if(motorY <= -2)
        {
            ANALYZER_ON(ANALYZER_CH3);
            WRITE(Y_STEP_PIN,HIGH);
#if FEATURE_TWO_YSTEPPER
            WRITE(Y2_STEP_PIN,HIGH);
#endif
            motorY += 2;
        }
        else if(motorY >= 2)
        {
            ANALYZER_ON(ANALYZER_CH3);
            WRITE(Y_STEP_PIN,HIGH);
#if FEATURE_TWO_YSTEPPER
            WRITE(Y2_STEP_PIN,HIGH);
#endif
            motorY -= 2;
        }
#endif
    }
    static inline void endXYZSteps()
    {
        WRITE(X_STEP_PIN,LOW);
        WRITE(Y_STEP_PIN,LOW);
        WRITE(Z_STEP_PIN,LOW);
#if FEATURE_TWO_XSTEPPER
        WRITE(X2_STEP_PIN,LOW);
#endif
#if FEATURE_TWO_YSTEPPER
        WRITE(Y2_STEP_PIN,LOW);
#endif
#if FEATURE_TWO_ZSTEPPER
        WRITE(Z2_STEP_PIN,LOW);
#endif
        ANALYZER_OFF(ANALYZER_CH1);
        ANALYZER_OFF(ANALYZER_CH2);
        ANALYZER_OFF(ANALYZER_CH3);
        ANALYZER_OFF(ANALYZER_CH6);
        ANALYZER_OFF(ANALYZER_CH7);
    }
    static inline speed_t updateStepsPerTimerCall(speed_t vbase)
    {
        if(vbase>STEP_DOUBLER_FREQUENCY)
        {
#if ALLOW_QUADSTEPPING
            if(vbase>STEP_DOUBLER_FREQUENCY*2)
            {
                Printer::stepsPerTimerCall = 4;
                return vbase>>2;
            }
            else
            {
                Printer::stepsPerTimerCall = 2;
                return vbase>>1;
            }
#else
            Printer::stepsPerTimerCall = 2;
            return vbase>>1;
#endif
        }
        else
        {
            Printer::stepsPerTimerCall = 1;
        }
        return vbase;
    }
    static inline void disableAllowedStepper()
    {
#ifdef XY_GANTRY
        if(DISABLE_X && DISABLE_Y)
        {
            disableXStepper();
            disableYStepper();
        }
#else
        if(DISABLE_X) disableXStepper();
        if(DISABLE_Y) disableYStepper();
#endif
        if(DISABLE_Z) disableZStepper();
    }
    static inline float realXPosition()
    {
        return currentPosition[X_AXIS];
    }

    static inline float realYPosition()
    {
        return currentPosition[Y_AXIS];
    }

    static inline float realZPosition()
    {
        return currentPosition[Z_AXIS];
    }
    static inline void realPosition(float &xp,float &yp,float &zp)
    {
        xp = currentPosition[X_AXIS];
        yp = currentPosition[Y_AXIS];
        zp = currentPosition[Z_AXIS];
    }
    static inline void insertStepperHighDelay() {
#if STEPPER_HIGH_DELAY>0
        HAL::delayMicroseconds(STEPPER_HIGH_DELAY);
#endif
    }
    static void constrainDestinationCoords();
    static void updateDerivedParameter();
    static void updateCurrentPosition(bool copyLastCmd = false);
    static void kill(uint8_t only_steppers);
    static void updateAdvanceFlags();
    static void setup();
    static void defaultLoopActions();
    static uint8_t setDestinationStepsFromGCode(GCode *com);
    static void moveTo(float x,float y,float z,float e,float f);
    static void moveToReal(float x,float y,float z,float e,float f);
    static void homeAxis(bool xaxis,bool yaxis,bool zaxis); /// Home axis
    static uint8_t setOrigin(float xOff,float yOff,float zOff);
    static bool isPositionAllowed(float x,float y,float z);
    static inline int getFanSpeed()
	{
        return (int)pwm_pos[NUM_EXTRUDER+2];
    }
#if NONLINEAR_SYSTEM
    static inline void setDeltaPositions(long xaxis, long yaxis, long zaxis)
    {
        currentDeltaPositionSteps[X_AXIS] = xaxis;
        currentDeltaPositionSteps[Y_AXIS] = yaxis;
        currentDeltaPositionSteps[Z_AXIS] = zaxis;
    }
    static void deltaMoveToTopEndstops(float feedrate);
#endif

#if FEATURE_Z_PROBE
#if MAX_HARDWARE_ENDSTOP_Z
    static float runZMaxProbe();
#endif
    static float runZProbe(bool first,bool last,uint8_t repeat = Z_PROBE_REPETITIONS);
    static void waitForZProbeStart();
#if FEATURE_AUTOLEVEL
    static void transformToPrinter(float x,float y,float z,float &transX,float &transY,float &transZ);
    static void transformFromPrinter(float x,float y,float z,float &transX,float &transY,float &transZ);
    static void resetTransformationMatrix(bool silent);
    static void buildTransformationMatrix(float h1,float h2,float h3);
#endif
#endif
#if FEATURE_MEMORY_POSITION
    static void MemoryPosition();
    static void GoToMemoryPosition(bool x,bool y,bool z,bool e,float feed);
#endif
    static void zBabystep();
private:
    static void homeXAxis();
    static void homeYAxis();
    static void homeZAxis();
};

#endif // PRINTER_H_INCLUDED
