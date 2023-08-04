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
#include <curl/curl.h>
#include <json/json.h>
#include "SBUS.h"

struct LLTrackingData
{
    int fID;
    float tx;
    float ty;
};

namespace
{
    std::size_t callback(
            const char* in,
            std::size_t size,
            std::size_t num,
            std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}

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
LLTrackingData Data;
CURL* curl;
CURLcode res;

/* make some talons for drive train */
std::string interface = "can0";
TalonFX talLeft(2); //Use the specified interface
TalonFX talRght(3); //Use the default interface (can0)
TalonFX talRght2(4);
TalonFX talLeft2(5);

double y = 0;
double turn = 0;

LLTrackingData getLLData(std::string LLIP){

    curl_global_init(CURL_GLOBAL_DEFAULT);

    while(1)
    {
        curl = curl_easy_init();
        // Set remote URL.
        const std::string url(LLIP);

        // Response information.
        long httpCode(0);
        std::unique_ptr<std::string> httpData(new std::string());

        // Don't bother trying IPv6, which would increase DNS resolution time.
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

        // Don't wait forever, time out after 10 seconds.
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        // Hook up data handling function.
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Hook up data container (will be passed as the last parameter to the
        // callback handling function).  Can be any pointer type, since it will
        // internally be passed as a void pointer.
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

        // Run our HTTP GET command, capture the HTTP response code, and clean up.
        curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);   
        
        if (httpCode == 200)
        {
            //std::cout << "\nGot successful response from " << url << std::endl;

            // Response looks good - done using Curl now.  Try to parse the results
            // and print them out.
            Json::Value jsonData;
            Json::Reader jsonReader;

            if (jsonReader.parse(*httpData.get(), jsonData))
            {
                //std::cout << "Successfully parsed JSON data" << std::endl;
                //std::cout << "\nJSON data received:" << std::endl;
                //std::cout << jsonData.toStyledString() << std::endl;

                Data.fID = jsonData["Results"]["Fiducial"][0]["fID"].asInt();
                // const std::string Results(jsonData["Results"].toStyledString());
                // const std::string Bardcode(jsonData["Bardcode"].toStyledString());
                // const std::string Classifier(jsonData["Classifier"].toStyledString());
                // const std::string Detector(jsonData["Detector"].toStyledString());
                //const std::string fID(jsonData["Fiducial"].asString());
                Data.tx = jsonData["Results"]["Fiducial"][0]["tx"].asFloat();
                Data.ty = jsonData["Results"]["Fiducial"][0]["ty"].asFloat();

                const Json::Value& Fiducial = jsonData["Fiducial"]; // array of characters

                std::cout << "Natively parsed:" << std::endl;
                //std::cout << "\tResults: " << Results << std::endl;
                //std::cout << "\tfID: " << Data.fID << std::endl;
                //std::cout << "\ttx: " << Data.tx << std::endl;
                //std::cout << "\tty: " << Data.ty << std::endl;
                std::cout << std::endl;
                return Data;
            }
            else
            {
                std::cout << "Could not parse HTTP data as JSON" << std::endl;
                std::cout << "HTTP data was:\n" << *httpData.get() << std::endl;
                return Data;
            }
        }
        else
        {
            std::cout << "Couldn't GET from " << url << " - exiting" << std::endl;
            return Data;
        }

    httpCode = 0;
    curl_easy_cleanup(curl);
    //return 0;
    }
}

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
			 
		if ((packet.channels[2] - 980) > 100 || (packet.channels[1] - 980) > 100 || (packet.channels[2] - 980) < -100 || (packet.channels[1] - 980) < -100 ){	 
			y = ((double)packet.channels[2] - 980) / 1000;
			turn = ((double)packet.channels[1] - 980) / 1000;
			drive(-y, turn);
		} else if ((packet.channels[2] - 980) < 100 || (packet.channels[1] - 980) < 100 || (packet.channels[2] - 980) > -100 || (packet.channels[1] - 980) > -100 )
        {
            drive(0,0);
        }

		if(int (packet.channels[4]) > 900){
			ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
			cout << "ENABLED" << endl;
		}

        if(int (packet.channels[5]) > 900){
			ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
			cout << "VISON ENABLED" << endl;
		}

        if(int (packet.channels[12]) > 900){
            getLLData("http://169.254.5.141:5807/results");
            if (Data.tx > 5)
            {
                drive(0,0.1);
            }
            else if (Data.tx < -5)
            {
                drive(0,-0.1);
            }
            // else
            // {
            //     drive(0,0);
            // }
            cout << Data.fID << endl;
            cout << Data.tx << endl;
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