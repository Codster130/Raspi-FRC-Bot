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
#include "SBUS.h"

using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;
using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::string;
using std::chrono::steady_clock;
using std::chrono::milliseconds;

static SBUS sbus;

/* make some talons for drive train */
std::string interface = "can0";
TalonFX talLeft(2); //Use the specified interface
TalonFX talRght(3); //Use the default interface (can0)
TalonFX talRght2(4);
TalonFX talLeft2(5);

double y = 0;
double turn = 0;

void initDrive()
{
	/* both talons should blink green when driving forward */
	talRght.SetInverted(true);
	talRght2.SetInverted(true);
}

void drive(double fwd, double turn)
{
	double left = fwd - turn;
	double rght = fwd + turn; /* positive turn means turn robot LEFT */

	talLeft.Set(ControlMode::PercentOutput, left);
	talRght.Set(ControlMode::PercentOutput, rght);
	talLeft2.Set(ControlMode::PercentOutput, left);
	talRght2.Set(ControlMode::PercentOutput, rght);
}
/** simple wrapper for code cleanup */
void sleepApp(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

static void onPacket(const sbus_packet_t &packet)
{
    static auto lastPrint = steady_clock::now();
    auto now = steady_clock::now();

    if (now - lastPrint > milliseconds(100))
    {
        for (int i = 0; i < 16; ++i)
            cout << "ch" << i + 1 << ": " << packet.channels[i] << "\t";

        cout << "ch17: " << (packet.ch17 ? "true" : "false") << "\t"
             << "ch18: " << (packet.ch18 ? "true" : "false");
			 
		if ((packet.channels[2] - 980) > 50 || (packet.channels[1] - 980) > 50 || (packet.channels[2] - 980) < -50 || (packet.channels[1] - 980) < -50 ){	 
			y = ((double)packet.channels[2] - 980) / 1000;
			turn = ((double)packet.channels[1] - 980) / 1000;
			drive(-y, turn);
		}
		else {
			drive(0,0);
		}

		if(int (packet.channels[4]) > 900){
			ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
			cout << "ENABLED" << endl;
		}

        if (packet.frameLost){
            cout << "\tFrame lost";
			drive(0,0);
		}
        if (packet.failsafe){
            cout << "\tFailsafe active";
			drive(0,0);
		}
        cout << endl;

        lastPrint = now;
    }
}

int main(int argc, char **argv) {	
	// Comment out the call if you would rather use the automatically running diag-server, note this requires uninstalling diagnostics from Tuner. 
	// c_SetPhoenixDiagnosticsStartTime(-1); // disable diag server, instead we will use the diag server stand alone application that Tuner installs

	/* setup drive */
	initDrive();
	drive(0,0);
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	cout << "SBUS blocking receiver example" << endl;

    string ttyPath;

    if (argc > 1)
        ttyPath = argv[1];
    else
    {
        cout << "Enter tty path: ";
        //cin >> ttyPath;
    }

    sbus.onPacket(onPacket);

    sbus_err_t err = sbus.install("/dev/ttyAMA0", true);  // true for blocking mode
    if (err != SBUS_OK)
    {
        cerr << "SBUS install error: " << err << endl;
        return err;
    }

    cout << "SBUS installed" << endl;

    // blocks until data is available
    while ((err = sbus.read()) != SBUS_FAIL)
    {
        // desync means a packet was misaligned and not received properly
        if (err == SBUS_ERR_DESYNC)
        {
            cerr << "SBUS desync" << endl;
			drive(0,0);
        }
    }

    cerr << "SBUS error: " << err << endl;
	drive(0,0);
    return err;
}