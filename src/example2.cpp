#define Phoenix_No_WPI // remove WPI dependencies
#include "ctre/Phoenix.h"
#include "ctre/phoenix/platform/Platform.hpp"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <SDL2/SDL.h>

using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;

/* make some talons for drive train */
std::string interface = "can0";
TalonFX talLeft(3, interface); //Use the specified interface
TalonFX talRght(2); //Use the default interface (can0)

void initDrive()
{
	/* both talons should blink green when driving forward */
	talRght.SetInverted(true);
}

void drive(double fwd, double turn)
{
	double left = fwd - turn;
	double rght = fwd + turn; /* positive turn means turn robot LEFT */

	talLeft.Set(ControlMode::PercentOutput, left);
	talRght.Set(ControlMode::PercentOutput, rght);
}
/** simple wrapper for code cleanup */
void sleepApp(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {	
	// Comment out the call if you would rather use the automatically running diag-server, note this requires uninstalling diagnostics from Tuner. 
	// c_SetPhoenixDiagnosticsStartTime(-1); // disable diag server, instead we will use the diag server stand alone application that Tuner installs

	/* setup drive */
	initDrive();

	while (true) {
		/* we are looking for gamepad (first time or after disconnect),
			neutral drive until gamepad (re)connected. */
		drive(0, 0);

		// wait for gamepad
		printf("Waiting for gamepad...\n");
		while (true) {
			/* SDL seems somewhat fragile, shut it down and bring it up */
			SDL_Quit();
			SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); //so Ctrl-C still works
			SDL_Init(SDL_INIT_JOYSTICK);

			/* poll for gamepad */
			int res = SDL_NumJoysticks();
			if (res > 0) { break; }
			if (res < 0) { printf("Err = %i\n", res); }

			/* yield for a bit */
			sleepApp(20);
		}
		printf("Waiting for gamepad...Found one\n");

		// Open the joystick for reading and store its handle in the joy variable
		SDL_Joystick *joy = SDL_JoystickOpen(0);
		if (joy == NULL) {
			/* back to top of while loop */
			continue;
		}

		// Get information about the joystick
		const char *name = SDL_JoystickName(joy);
		const int num_axes = SDL_JoystickNumAxes(joy);
		const int num_buttons = SDL_JoystickNumButtons(joy);
		const int num_hats = SDL_JoystickNumHats(joy);
		printf("Now reading from joystick '%s' with:\n"
			"%d axes\n"
			"%d buttons\n"
			"%d hats\n\n",
			name,
			num_axes,
			num_buttons,
			num_hats);

		/* I'm using a logitech F350 wireless in D mode.
		If num axis is 6, then gamepad is in X mode, so neutral drive and wait for D mode.
		[SAFETY] This means 'X' becomes our robot-disable button.
		This can be removed if that's not the goal. */
		if (num_axes >= 6) {
			/* back to top of while loop */
			continue;
		}

		// Keep reading the state of the joystick in a loop
		while (true) {
			/* poll for disconnects or bad things */
			SDL_Event event;
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) { break; }
				if (event.jdevice.type == SDL_JOYDEVICEREMOVED) { break; }
			}

			/* grab some stick values */
			double y = ((double)SDL_JoystickGetAxis(joy, 1)) / -32767.0;
			double turn = ((double)SDL_JoystickGetAxis(joy, 2)) / -32767.0;
			drive(y, turn);

			/* [SAFETY] only enable drive if top left shoulder button is held down */
			if (SDL_JoystickGetButton(joy, 4)) {
				ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
			}

			/* loop yield for a bit */
			sleepApp(20);
		}

		/* we've left the loop, likely due to gamepad disconnect */
		drive(0, 0);
		SDL_JoystickClose(joy);
		printf("gamepad disconnected\n");
	}

	SDL_Quit();
	return 0;
}