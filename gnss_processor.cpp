
#include "gnss_processor.h"

#include <cmath>
#include <iostream>

namespace {
constexpr uint64_t kMaxTimestampGapMs = 1500;
constexpr float kMaxHdop = 5.0f;
constexpr int kMinSatellites = 4;
}

GnssProcessor::GnssProcessor()
    : last_timestamp_(0), first_frame_(true) {}

bool GnssProcessor::process(const GnssFrame& frame) {

    // Reject duplicate or out-of-order timestamps
    if (!first_frame_ && frame.timestamp_ms <= last_timestamp_) {
        std::cout << "Rejected: Timestamp error\n";
        return false;
    }

    // Check timestamp gap before updating
    bool has_gap = !first_frame_ &&
        (frame.timestamp_ms - last_timestamp_ > kMaxTimestampGapMs);

    // Update timestamp even if the GNSS position is rejected
    last_timestamp_ = frame.timestamp_ms;
    first_frame_ = false;

    if (has_gap) {
        std::cout << "Rejected: Timestamp gap\n";
        return false;
    }

    // Latitude validation
    if (!std::isfinite(frame.lat) ||
        frame.lat < -90.0 || frame.lat > 90.0) {
        std::cout << "Rejected: Invalid latitude\n";
        return false;
    }

    // Longitude validation
    if (!std::isfinite(frame.lon) ||
        frame.lon < -180.0 || frame.lon > 180.0) {
        std::cout << "Rejected: Invalid longitude\n";
        return false;
    }

    // Fix validation
    if (frame.fix_type != 1 && frame.fix_type != 2) {
        std::cout << "Rejected: Invalid fix type\n";
        return false;
    }

    // HDOP validation
    if (!std::isfinite(frame.hdop) ||
        frame.hdop <= 0.0f ||
        frame.hdop > kMaxHdop) {
        std::cout << "Rejected: Invalid HDOP\n";
        return false;
    }

    // Satellite validation
    if (frame.satellites_used < kMinSatellites) {
        std::cout << "Rejected: Low satellites\n";
        return false;
    }

    // Accepted GNSS frame
    std::cout << "Valid Position: "
              << frame.lat << ", "
              << frame.lon << '\n';

    return true;
}
