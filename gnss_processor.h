#pragma once

#include <cstdint>

struct GnssFrame
{
    uint64_t timestamp_ms;
    double lat;
    double lon;
    float hdop;
    int satellites_used;
    int fix_type;
};


class GnssProcessor
{
public:

    GnssProcessor();

    // Returns true if frame is accepted
    bool process(const GnssFrame& frame);


private:

    uint64_t last_timestamp_;
    bool first_frame_;
};