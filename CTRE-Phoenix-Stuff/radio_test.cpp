#include "radio.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <bitset>

using namespace std;
int SBUS_PIN = 14;
int * channel_data;
//SbusReader reader;

int main(){
    SbusReader reader(SBUS_PIN);
    reader.begin_listen(SBUS_PIN);
    this_thread::sleep_for(chrono::milliseconds(1000));
    //reader.__init__(SBUS_PIN);
    // while(!reader.is_connected()){
    //     reader.translate_latest_packet();
    //     this_thread::sleep_for(chrono::milliseconds(200));
    //     cout << "Not Connected" << endl;
    // }

    this_thread::sleep_for(chrono::milliseconds(100));

    while(true){
        try{
            bool is_connected = reader.is_connected();
            int packet_age = reader.get_latest_packet_age();
            channel_data = reader.translate_latest_packet();
            reader.display_latest_packet();
            cout << is_connected << endl;
            cout << packet_age << endl;
            cout << channel_data << endl;
        }
        catch(...){
            reader.end_listen();
            throw;
        }
    }
}
