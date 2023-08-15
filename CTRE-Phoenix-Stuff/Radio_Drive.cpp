#define Phoenix_No_WPI // remove WPI dependencies
#include "ctre/Phoenix.h"
#include "ctre/phoenix/platform/Platform.hpp"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <memory>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <json/json.h>
#include "SBUS.h"
#include "navXTimeSync/AHRS.h"
#include <signal.h>
#include "Read_Json/Read_JSON.h"

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

volatile sig_atomic_t sflag = 0;

void handle_sig(int sig)
{
    sflag = 1;
}

static SBUS sbus;
LLTD::ReadJSON jsonReader;
sbus_packet_t sbus_data;

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
        for (int i = 0; i < 16; ++i){
            cout << "ch" << i + 1 << ": " << packet.channels[i] << "\t";
            sbus_data.channels[i] = packet.channels[i];
        }

        cout << "ch17: " << (packet.ch17 ? "true" : "false") << "\t"
             << "ch18: " << (packet.ch18 ? "true" : "false");
        sbus_data.ch17 = packet.ch17;
        sbus_data.ch18 = packet.ch18;

        if (packet.frameLost){
            cout << "\tFrame lost";
		}
        sbus_data.frameLost = packet.frameLost;
        if (packet.failsafe){
            cout << "\tFailsafe active";
		}
        sbus_data.failsafe = packet.failsafe;
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
    signal(SIGINT, handle_sig);
    AHRS com = AHRS("/dev/ttyACM0");
	sleepApp(2000);

    string ttyPath;

    if (argc > 1)
        ttyPath = argv[1];
        //cout << "Enter tty path: ";
        //cin >> ttyPath;

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
        sleepApp(50);
        // desync means a packet was misaligned and not received properly
        if (err == SBUS_ERR_DESYNC)
        {
            cerr << "SBUS desync" << endl;
			drive(0,0);
        }
        else
        {
            sbus.onPacket(onPacket);
            if ((sbus_data.channels[2] - 980) > 100 || (sbus_data.channels[1] - 980) > 100 || (sbus_data.channels[2] - 980) < -100 || (sbus_data.channels[1] - 980) < -100 ){	 
                y = ((double)sbus_data.channels[2] - 980) / 1000;
                turn = ((double)sbus_data.channels[1] - 980) / 1000;
                drive(-y, turn);
            } else if ((sbus_data.channels[2] - 980) < 100 || (sbus_data.channels[1] - 980) < 100 || (sbus_data.channels[2] - 980) > -100 || (sbus_data.channels[1] - 980) > -100 )
            {
                drive(0,0);
            }

            if(int (sbus_data.channels[4]) > 900){
                ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
                cout << "ENABLED" << endl;
            }

            if(int (sbus_data.channels[11]) > 900){
                LLTD::LLTrackingData Data = jsonReader.getLLData("http://169.254.5.141:5807/results");
                if ((Data.fIDtx > 5) & (Data.fIDty < -5) & (Data.fID == 1))
                {
                    drive(-0.25,0.1);
                }
                else if ((Data.fIDtx < -5) & (Data.fIDty < -5) & (Data.fID == 1))
                {
                    drive(-0.25,-0.1);
                }
                else if ((Data.fIDtx > 5) & (Data.fID == 1))
                {
                    drive(0,0.1);
                }
                else if ((Data.fIDtx < -5) & (Data.fID == 1))
                {
                    drive(0,-0.1);
                }
                else if ((Data.fIDty < -5) & (Data.fID == 1))
                {
                    drive(-0.25,0);
                }
                else if ((Data.fIDtx < 5) & (Data.fIDty < 5) & (Data.fIDty > -5) & (Data.fIDtx > -5) & (Data.fID == 1))
                {
                    drive(0,0);
                }
                cout << Data.fID << endl;
                cout << Data.fIDtx << endl;
                cout << Data.fIDty << endl;
                cout << Data.neuralClass << endl;
                cout << Data.conf << endl;
                cout << Data.neuraltx << endl;
                cout << Data.neuralty << endl;
            }

            if(int (sbus_data.channels[6]) > 900)
            {
                std::cout << std::fixed << std::setprecision(2) << com.GetPitch() << "      " << com.GetRoll() << "   " << com.GetYaw() << "     " <<com.GetWorldLinearAccelX() << "     " << com.GetWorldLinearAccelY() << "       " << com.GetWorldLinearAccelZ() << "      " << com.GetLastSensorTimestamp() << "      " << '\r' << std::flush;
            }

            if (sbus_data.failsafe){
                drive(0,0);
		    }

            if (sbus_data.frameLost){
                drive(0,0);
		    }
        }       
    }
    cerr << "SBUS error: " << err << endl;
	drive(0,0);
    return err;
}