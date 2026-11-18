#include <cstdlib>
#include <iostream>

#include "gnss_processor.h"

#define RUN_TEST(name, expr) \
    do { \
        if (expr) { \
            std::cout << "[PASS] " << name << std::endl; \
        } else { \
            std::cout << "[FAIL] " << name << std::endl; \
            return EXIT_FAILURE; \
        } \
    } while (0)

int main() {
    // Test 1: Normal frames should succeed.
    {
        GnssProcessor p;
        bool r1 = p.process({1000, 12.971600, 77.594600, 1.2f, 8, 1});
        bool r2 = p.process({1100, 12.971601, 77.594601, 1.1f, 9, 1});
        RUN_TEST("Normal Operation", r1 && r2);
    }

    // Test 2: HDOP above some threshold should fail.
    {
        GnssProcessor p;
        bool r = p.process({1000, 12.9716, 77.5946, 6.5f, 8, 1});
        RUN_TEST("High HDOP Rejection", !r);
    }

    // Test 3: A missing fix should fail.
    {
        GnssProcessor p;
        bool r = p.process({1000, 12.9716, 77.5946, 1.2f, 8, 0});
        RUN_TEST("No Fix Rejection", !r);
    }

    // Test 4: Coordinates outside the valid GNSS range should fail.
    {
        GnssProcessor p;
        bool r = p.process({1000, 91.0, 77.5946, 1.2f, 8, 1});
        RUN_TEST("Out-of-Range Coordinate Rejection", !r);
    }

    // Test 5: Fewer than four satellites should fail.
    {
        GnssProcessor p;
        bool r = p.process({1000, 12.9716, 77.5946, 1.2f, 3, 1});
        RUN_TEST("Low Satellite Rejection", !r);
    }

    // Test 6: A gap longer than 1.5 seconds should fail.
    {
        GnssProcessor p;
        bool r1 = p.process({1000, 12.9716, 77.5946, 1.2f, 8, 1});
        bool r2 = p.process({2600, 12.9717, 77.5947, 1.2f, 8, 1});
        RUN_TEST("Timestamp Gap Rejection", r1 && !r2);
    }

    // Test 7: Timestamps must be strictly increasing.
    {
        GnssProcessor p;
        bool r1 = p.process({1000, 12.9716, 77.5946, 1.2f, 8, 1});
        bool r2 = p.process({1000, 12.9717, 77.5947, 1.2f, 8, 1});
        RUN_TEST("Non-Monotonic Timestamp Rejection", r1 && !r2);
    }

    std::cout << "\nAll tests passed successfully!" << std::endl;
    return EXIT_SUCCESS;
}