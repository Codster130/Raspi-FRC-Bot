#include <pigpio.h>
#include <bitset>
#include <string>

//minimum time between packets in microseconds (6000 microseconds is a typical gap, but the code looks for 5000 or *more*, in case there is some timing error)
int _PACKET_BOUNDRY_TIME = 5000;

int _PACKET_LENGTH = 298;
int _UART_FRAME_LENGTH =12;

std::bitset<12> _UART_FRAME_CONFORMANCE_BITMASK (std::string("100000000011"));
std::bitset<12> _FAILSAFE_STATUS_BITMASK (std::String("000000001100"));

int _last_tick = 0;
int _working_packet_ptr = 0;

std::bitset<_PACKET_LENGTH> _working_packet (std::string());
std::cout << _PACKET_LENGTH.reset() << '\n';

std::bitset<_PACKET_LENGTH> _latest_complete_packet (std::string());
int _latest_complete_packet_timestamp = 0;
bool _is_connected = false;

void _sanity_check_packet(packet){
    ret_val = (true, NULL, NULL)

    for packet_bits_ptr in range (_UART_FRAME_LENGTH, _UART_FRAME_LENGTH+23*_UART_FRAME_LENGTH, _UART_FRAME_LENGTH){
        cur_UART_frame = packet[packet_bits_ptr:packet_bits_ptr+_UART_FRAME_LENGTH]

        if 
    }
}