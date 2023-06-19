//#ifndef RADIO
//#define RADIO
#include <iostream>
#include <bitset>
#include <string>
#include "pigpio.h"
using namespace std;

// PACKET REFERENCE

//  UART packet          UART packet 2
//                                Parity
// ┌─┬──────┬─┬──┐      ┌─┬──────┬─┬──┐
// │1│Data  │P│00├─────►│1│Data  │P│00├─────►
// └─┴──────┴─┴──┘      └─┴──────┴─┴──┘
//  0 1-8    9 10,11     0 1-8    9 10,11


//   │      │             │    │   │
//   │ 0-7  │             │8-10│0-4│

//   -All bits are inverted (UART bits are shown inverted)
//   -UART is 100k baud, two stop bits, even parity (odd after inversion)
//   -SBus Data 11 bits per channel
//   -SBus Data comes in Big-Endian (most significant bit first)
//   -But 11 Bit number is Little-Endian!
//   -Example:
//       Data Packet:
//           11000010 100
//       Now Invert:
//           00111101 011
//       Above is *little-endian*, so the '1' on the right is 1024 in decimal. To read little-endian, reverse the bits and read left-to-right
//            110 00111101
//       In Decimal
//            1597

//minimum time between packets in microseconds (6000 microseconds is a typical gap, but the code looks for 5000 or *more*, in case there is some timing error)
#define _PACKET_BOUNDRY_TIME 5000
#define _PACKET_LENGTH 298
#define _UART_FRAME_LENGTH 12
bitset<_UART_FRAME_LENGTH> _UART_FRAME_CONFORMANCE_BITMASK (string("100000000011"));
bitset<_UART_FRAME_LENGTH> _FAILSAFE_STATUS_BITMASK (string("000000001100"));

int _last_tick = 0;
int _working_packet_ptr = 0;
bitset<_PACKET_LENGTH> _working_packet;
bitset<_PACKET_LENGTH> _latest_complete_packet;
//_latest_complete_packet.reset();
int _latest_complete_packet_timestamp = 0;
bool _is_connected = false;
int ret_list [16] {0};

bool _sanity_check_packet(bitset<_PACKET_LENGTH> packet){ 
    bitset<_UART_FRAME_LENGTH> cur_UART_frame;
    for (int packet_bits_ptr = 0; packet_bits_ptr < _UART_FRAME_LENGTH+23*_UART_FRAME_LENGTH; packet_bits_ptr += _UART_FRAME_LENGTH) {
        
        for(int i = 0; i< _UART_FRAME_LENGTH; i++){
            cur_UART_frame[i] = packet[packet_bits_ptr + i];
        }

        if ((_UART_FRAME_CONFORMANCE_BITMASK & cur_UART_frame).to_ulong() != 2048){
            cout << "Failed Conformance bitmask thing." << endl;
            return false;
        }

        bool parity_check = cur_UART_frame.count() % 2;
        if (parity_check == cur_UART_frame[9]){
            cout << "Failed Parity Check." << endl;
            return false;
        }
    }  
    cout << "Sanity check packet passed." << endl;
    return true;
}

void on_change (int level, uint32_t tick) {
    int time_elapsed = tick - _last_tick;

    if (time_elapsed < 0) {
        time_elapsed = 4294967295 - _last_tick + tick;
    }

    if (time_elapsed >= _PACKET_BOUNDRY_TIME) {
        if (_sanity_check_packet(_working_packet)) {
            bitset<_PACKET_LENGTH> temp = _latest_complete_packet;
            _latest_complete_packet = _working_packet;
            _working_packet = temp;
            _latest_complete_packet_timestamp = tick;
            bitset<2> tmp;
            tmp[0] = _latest_complete_packet[279];
            tmp[1] = _latest_complete_packet[280]; 
            _is_connected = ((tmp.to_ulong()) == 3);
        }

        _working_packet.reset();
        _working_packet_ptr = 0;
        _last_tick = tick;
        return;
    }

    int num_bits = (int)(time_elapsed/10);
    bool bit_val = bool(-level+1);
    
    int new_working_packet_ptr = _working_packet_ptr + num_bits; 
    for (int i = _working_packet_ptr; i < new_working_packet_ptr; i++){
        _working_packet[i] = bit_val;
    }

    _working_packet_ptr = new_working_packet_ptr;
    _last_tick = tick;
}

class SbusReader{
    public:
        SbusReader(int gpio_pin) {
            PIGPIO_H::gpioInitialise();
            if (gpioInitialise() < 0){
                cout << "pigpio initialistaion failed." << endl;
            }
            else{
                cout << "pigpio initalised okay." << endl;
            }
            gpioSetMode(gpio_pin, 0);
        }

        // void __init__(int gpio_pin){
        //     PIGPIO_H::gpioInitialise();
        //     gpioSetMode(gpio_pin, PI_OUTPUT);
        // }

        void begin_listen(int gpio_pin){
            //PIGPIO_H::gpioSetAlertFunc(gpio_pin, on_change(PIGPIO_H::gpioRead(14), PIGPIO_H::gpioTick()));
            _latest_complete_packet_timestamp = PIGPIO_H::gpioTick();
        }

        void end_listen(){
            PIGPIO_H::gpioTerminate();
        }

        int * translate_packet(bitset<_PACKET_LENGTH> packet){
            bitset<_PACKET_LENGTH> channel_bits;
            channel_bits.reset();
            int channel_bits_ptr = 0;

            for (int packet_bits_ptr = 0; packet_bits_ptr < _UART_FRAME_LENGTH + 22 * _UART_FRAME_LENGTH; packet_bits_ptr += _UART_FRAME_LENGTH){
                for (int i = 0; i < 8; i++){
                    channel_bits[channel_bits_ptr + i] = !packet[packet_bits_ptr + 1 + i];
                }
            channel_bits_ptr += 8;
            }

            bitset<_UART_FRAME_LENGTH> temp;
            for(int ret_list_ptr = 0; ret_list_ptr < 11; ret_list_ptr++){
                for(int channel_ptr = 0; channel_ptr < 16*11; channel_ptr += 11){
                    
                    for(int i = 0; i < 11; i++){
                        int temp2 = (i + channel_ptr);
                        temp[i] = channel_bits[temp2];
                    }
                    int data = temp.to_ulong();
                    ret_list[ret_list_ptr] = data;
                    temp.reset();
                }
            }
            return ret_list;
        }

        bitset<_PACKET_LENGTH> retrieve_latest_packet(){
            return _latest_complete_packet;
        }

        int * translate_latest_packet(){
            return translate_packet(_latest_complete_packet);
        }

        void display_latest_packet(){
            int * channel_val_list;
            channel_val_list = translate_latest_packet();
            for (int i = 0; i < 16; i++){
                int channel_ptr = i * 11;
                uint16_t channel_val = 0;
                for (int j = 0; j < 11; j++)
                {
                    channel_val |= channel_val_list[channel_ptr + j] << j;
                }
                cout << "Channel #" << i + 1 << ": " << channel_val << endl;
            }
            cout << "Packet Age (milliseconds): " << get_latest_packet_age() << endl;

            string transmission_status;
            if (_is_connected){
                transmission_status = "Connected";
            }
            else{
                transmission_status = "Disconnected";
            }
            cout << "Transmission Status: " << transmission_status << endl;
        }

        int get_latest_packet_age(){
            int output = PIGPIO_H::gpioTick();
            output = ((output - _latest_complete_packet_timestamp)/1000);
            return output;
        } 

        bool is_connected(){
            return _is_connected;
        }
};

//#endif