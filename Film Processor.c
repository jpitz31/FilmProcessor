//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                              NXT Brick User Controlled Button Example.
//
// By default the NXT firmware has standard processing for the four buttons on the NXT. This
// program illustrates how the standard processing can be over-ridden and user defined actions
// can be processed within user's Robotc program.
//
//                                     *  ********  *
//                                   * *  *      *  * *
//                                 * L *  * Enter*  * R *
//                                   * *  *      *  * *
//                                     *  ********  *
//
//                                        ********
//                                        * Exit *
//                                        ********
//
// The four buttons are shown in the above graphic: Top line is left, enter, right buttons
//                                                  second line is 'exit' button.
//
// There are two approaches to perform button management in a user program.
//  1. The user program can continuously poll the button states and look for changes.
//  2. The user program can be notified whenever a button is pushed. The notification is by
//     starting a user specified task running.
//  For both methods, you have to set some variables to override the default system button
//  handler.
//
//  There are three intrinsic variables defined for NXT button management.
//
//  nNxtButtonPressed   This read-only variable contains the currently pressed button. [The NXT
//                      firmware only supports recognition of one button at a time]. The value
//                      is one of the following:
//                           typedef enum
//                           {
//                             kNoButton      = 0xFF,
//                             kExitButton    = 0,
//                             kRightButton   = 1,
//                             kLeftButton    = 2,
//                             kEnterButton   = 3
//                           } TButtonMasks;
//
//  nNxtButtonTask  This variable contains the task number that should be started whenever any NXT
//                   button is pushed. There are three value ranges:
//                       -1       Indicates that the normal system processing for exit button should
//                                be used. Anything else will over-ride the system actions. Note if
//                                overwritten, you will not be able to stop program running with button
//                                control.
//                      1..9      Indicates that the indicated task (1..9) should be started whenever
//                                the exit button is pressed. Task will only be started once per
//                                keypress.
//                      Other			Standard processing is overwritten. No task will be started when exit
//                                button is pushed; user program must used nNxtButtons to check for
//
//  nNxtExitClicks  Contains the number of 'clicks' that must be received to abort a running program.
//                  The default value is one. But if you want to use the EXIT button in your program
//                  you can set this to 2 (for a double click) or 3 (triple click) or higher so that
//                  there is always an escape sequence, independent of your user program, for existing
//                  your program.
//
//                  This variable is reset to 1 before the start of any program.
//
//                  If you really want to override the 'abort' check, you can continually modify this
//                  variable. The count of current EXIT clicks is reset whenever this variable is
//                  written.
//
//
// It may be easier to use the method of starting a task whenever a button is pressed. The user program
// can look at the 'nNxtButtonPressed' variable to determine which button has been pressed.
//
// How to Terminate/Abort A Program when Buttons are Hijacked
// ==========================================================
// When user program has gained control of the NXT buttons, the regular processing of the buttons is stopped.
// This means the EXIT button will not be recognized as a command to stop the end user program. So there
// must be an escape sequence to allow for this. Several different schemes were tried and the following
// was finally adopted for ROBOTC.
//
// A program is aborted by counting the number of "clicks" on the EXIT button. The normal default case is
// that a single click will abort a program. Setting this to a higher value (double or tripe clicks) will
// still allow user program to use EXIT button, but firmware will still have the capability to abort
// program
//
//
// Notes on NXT Hardware:
// ======================
//  1. Buttons on the NXT are not individually read. Instead there is are several resistors in series that
//     are shorted by pressing the individual keys. This is fed into a single input pin where an A-to-D
//     conversion is performed. Analyzing the result indicates which key is pressed.
//  2. A byproduct of this is that the NXT can only properly handle a single keypress at a time. If
//     multiple keys are pressed, then the firmware recognizes the key that generates the lowest resistance.
//     Only this key is reported to the user program.
//  3. A byproduct is that you cannot design a user interface that tries to use two keys depressed at the
//     same time.
//  4. The firmware recognizes button depresses in the following order(low to high): left, right, enter, exit.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Counter to keep track of the number of times a button is pressed.
//


int nLeftButton  = 0;
int nRightButton = 0;
int nEnterButton = 0;
int nExitButton  = 0;
int nNoButton    = 0;
int isProcessing = 0;

task main()
{
	//
	// "Hi-jack" buttons for user program control.
	//
	nNxtButtonTask  = -2;   // Grab control of the buttons. '-3' is invalid for task, so no task will be
	                        // executed when a button is pressed.

	//
	// Note: program cannot be terminated if we hijack the 'exit' button. So there has to be an escape sequence
	//       that will return buttons to system control! We'll use a triple click
	//
	nNxtExitClicks  = 3; // Triple clicking EXIT button will terminate program
	//
	// Do nothing. Just keep waiting
	//
	eraseDisplay();eraseDisplay();
  nxtDisplayBigTextLine(0, "Waiting");

  // Setup Motor
  bFloatDuringInactiveMotorPWM = false;
  bFloatDuringInactiveMotorPWM = false;

	while (true)
	{
		//
		// Some dummy code to do some work. In this case we'll simply display a count of the number of times
		// each button has been pressed.
		//
	  // string sTemp;
		TButtons nBtn;

	  //
	  // Update count of times each button is pushed. If using this program as a template, replace these
	  // statements with your own custom action.
	  //
	  while (true) // wait for button press
	  {
	  	nBtn = nNxtButtonPressed;
	  	if (nBtn != -1)
	  	{
	  	  break;
	  	}
	  }

	  switch (nBtn)
	  {
	  	case kLeftButton:	  isProcessing = 1;  	break;
			case kRightButton:	isProcessing = 2; 	break;
			default:            ++nNoButton;        break;
		}

		if (isProcessing == 1)
		{
			eraseDisplay();
			PlaySoundFile("! Click.rso");
			nxtDisplayTextLine(0, "Running");
			// process motor

			while (true)
			{
				nMotorEncoder[motorA] = 0;          	// reset the Motor Encoder of Motor A
				while(nMotorEncoder[motorA] < 1080)  	// while the Motor Encoder of Motor A has not yet reached 360 counts:
				{
  				motor[motorA] = 25;								// motor A is given a power level of 100

  				nBtn = nNxtButtonPressed;

  				if (nBtn == kRightButton)
	  			{
	  				motor[motorA] = 0;
	  				isProcessing = 2 ;
	  	  		break;
					}
				}

				motor[motorA] = 0;
				wait1Msec(500);

				if (isProcessing == 2)
				{
					break;
				}

				motor[motorA] = 0;

				nMotorEncoder[motorA] = 0;          	// reset the Motor Encoder of Motor A
				while(nMotorEncoder[motorA] > -1080)  	// while the Motor Encoder of Motor A has not yet reached 360 counts:
				{
					motor[motorA] = -25;

					nBtn = nNxtButtonPressed;// motor A is given a power level of -100
					if (nBtn == kRightButton)
  				{
  					motor[motorA] = 0;
  					isProcessing = 2;
  	  			break;
					}
				}
				motor[motorA] = 0;

				if (isProcessing == 2)
				{
					break;
				}
			}

			wait1Msec(500);
		}

		if (isProcessing == 2)
		{
			// stop motor
			motor[motorA] = 0;
			eraseDisplay();
			nxtDisplayTextLine(0, "Paused");
			PlaySoundFile("! Click.rso");
			isProcessing = 0;
		}
  }
	return;
}
