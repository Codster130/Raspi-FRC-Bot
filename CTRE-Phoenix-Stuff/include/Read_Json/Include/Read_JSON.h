#ifndef Read_JSON
#define Read_JSON

#include <iostream>

struct LLTrackingData
{
    int fID;
    float tx;
    float ty;
};

namespace TrackData
{
    class LLTD
    {
    public:
        LLTrackingData getLLData(std::string LLIP);
    };
}

#endif