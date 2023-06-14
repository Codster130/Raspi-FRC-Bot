#include <pigpio.h>
#include <bitset>
#include <string>

using namespace std;

//minimum time between packets in microseconds (6000 microseconds is a typical gap, but the code looks for 5000 or *more*, in case there is some timing error)
int _PACKET_BOUNDRY_TIME = 5000;
int _PACKET_LENGTH = 298;
int _UART_FRAME_LENGTH =12;
int _latest_complete_packet_timestamp;
bool _is_connected;
int _latest_complete_packet_timestamp = 0;
bool _is_connected = false;
int _last_tick;
int _last_tick = 0;
int _working_packet_ptr = 0;
bitset<12> _UART_FRAME_CONFORMANCE_BITMASK (string("100000000011"));
bitset<12> _FAILSAFE_STATUS_BITMASK (string("000000001100"));
bitset<_PACKET_LENGTH> _working_packet (string("0"));
bitset<_PACKET_LENGTH> _latest_complete_packet (string("0"));
bitset<_PACKET_LENGTH> _working_packet (string("0"));
bitset<_PACKET_LENGTH> _working_packet_ptr (string("0"));
bitset<_PACKET_LENGTH> _latest_complete_packet (string("0"));

// bool _sanity_check_packet(bitset<_PACKET_LENGTH> packet){ 
//     bool ret_val = true;

//     for (int packet_bits_ptr = _UART_FRAME_LENGTH; packet_bits_ptr < _UART_FRAME_LENGTH+23*_UART_FRAME_LENGTH; packet_bits_ptr += _UART_FRAME_LENGTH) {
//         int cur_UART_frame = packet[packet_bits_ptr:packet_bits_ptr+_UART_FRAME_LENGTH];
//     }  

// }

void on_change (int level, int tick) {
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

            _is_connected = latest_complete_packet[279:281].to_ulong() == 3;
        }

        working_packet.reset();
        _working_packet_ptr = 0;
        _last_tick = tick;
        return;
    }

    int num_bits = (int)(time_elapsed/10);
    bool bit_val = bool(-level+1);
    
    int new_working_packet_ptr = _working_packet_ptr + num_bits; 
    _working_packet[_working_packet_ptr : new_working_packet_ptr] = bit_val;
    _working_packet_ptr = new_working_packet_ptr;

    _last_tick = tick;
}