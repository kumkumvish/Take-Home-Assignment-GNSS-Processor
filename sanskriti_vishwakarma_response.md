# GNSS Processor — Candidate Response

## 1. What assumptions did you have to make?

- **Message rate and timing:** The receiver nominally publishes at 10 Hz, corresponding to 100 ms between frames. I allow gaps of up to **1500 ms** to tolerate occasional missed packets; a larger gap rejects the frame immediately after the gap. This is an engineering assumption for the assignment, not a requirement implied by 10 Hz.
- **Timestamps:** `timestamp_ms` refers to measurements from a consistent time source. Normally frames arrive in chronological order; duplicated and out-of-order timestamps must be rejected. The first frame establishes the timestamp baseline.
- **GNSS quality:** A usable measurement requires at least **4 satellites** and a finite, positive **HDOP no greater than 5.0**. These limits are deliberately simple and would need tuning against the receiver and navigation requirements.
- **Fix codes:** Only `fix_type == 1` (standard GPS) and `fix_type == 2` (mixed) indicate usable fixes, as described in the assignment.
- **Position representation:** Latitude and longitude are decimal degrees. Their legal bounds are latitude **[-90, 90]** and longitude **[-180, 180]**. A coordinate inside these ranges is syntactically valid but not necessarily physically plausible.

## 2. Which validation rules did you implement?

For each incoming frame, the processor:

1. Rejects duplicate or non-increasing timestamps.
2. Rejects the frame following a timestamp interval greater than **1500 ms**.
3. Rejects non-finite latitude/longitude and coordinates outside their legal ranges.
4. Requires `fix_type` to equal **1 or 2**.
5. Requires HDOP to be finite, greater than zero, and **<= 5.0**.
6. Requires `satellites_used >= 4`.

On acceptance, `process()` prints the position and returns `true`. On rejection, it prints a diagnostic reason and returns `false`. The implementation uses the C++17 standard library only.

## 3. How should the processor behave after a rejected frame, and why?

A rejected **position measurement** should not prevent future valid measurements from being accepted. To support recovery, the processor separately tracks the timestamp of the most recent **chronologically newer received frame**, whether or not that frame passes the GNSS quality checks. It rejects duplicates or out-of-order frames **without** moving the timestamp backward.

This is important when satellite reception deteriorates: if the stored timestamp were updated only after a fully accepted fix, the next good fix could appear to be more than 1500 ms late. Repeated rejections could then continue indefinitely. Updating the timestamp baseline for newer received frames lets the stream recover once quality and timing return to normal. A gap still rejects the affected frame, but subsequent normally timed frames can pass.

**Trade-off:** This simple approach assumes trustworthy, consistently scaled sensor timestamps. In production I would explicitly handle receiver clock resets and distinguish sensor timestamps from host arrival times.

## 4. What would you change before using this component on a real robot?

Before deploying this GNSS processor on a real robot, I would introduce the following improvements to enhance reliability, accuracy, and robustness:

1. **Position-jump detection:** Implement a distance and speed plausibility check between consecutive trusted GNSS positions to reject unrealistic position changes caused by GNSS errors or multipath interference.
2. **Configurable validation parameters:** Make thresholds such as minimum satellite count, maximum HDOP, and allowed timestamp gaps configurable to accommodate different receivers and operating environments.
3. **Sensor timeout monitoring:** Detect when GNSS data stops arriving and notify the navigation system instead of continuing to rely on outdated position information.
4. **Sensor fusion:** Integrate GNSS measurements with IMU and wheel odometry through an Extended Kalman Filter (EKF) for improved localization accuracy and continuity during temporary GNSS signal loss.
5. **Improved quality validation:** Use additional GNSS information such as position covariance, estimated accuracy, and RTK status when supported by the receiver.
6. **Diagnostics and fault handling:** Introduce structured logging, rejection statistics, and fault reporting to help monitor sensor health and diagnose problems during operation.
7. **Timestamp and recovery handling:** Distinguish sensor timestamps from system arrival timestamps and handle receiver restarts, delayed messages, and timestamp resets safely.

These improvements would make the processor more suitable for real-world robotic navigation, where GNSS measurements may be affected by signal obstruction, multipath interference, communication delays, and temporary sensor failures.

## Verification and optional CSV replay

- **Unit tests:** 13 passed (7 supplied tests plus 6 additional tests covering recovery, out-of-order handling, mixed/unknown fixes, invalid numerical data, and dropped-frame tolerance).
- **Recorded replay (`gnss.csv`):** 500 processed frames; **459 accepted**, **41 rejected**, **0 malformed** (acceptance rate **91.8%**).
- **Replay findings:** The corrected timestamp tracking avoids the persistent timestamp-gap rejection loop following low-quality GNSS frames. The recorded track also contains an implausible position discontinuity, demonstrating why a maximum-speed or position-jump check is a useful future improvement rather than a claim of present functionality.

### Build and run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic test_gnss_processor.cpp gnss_processor.cpp -o test_gnss
./test_gnss

g++ -std=c++17 -Wall -Wextra -pedantic read.cpp gnss_processor.cpp -o gnss_reader
./gnss_reader gnss.csv
```

## AI Usage Disclosure

I am more familiar with Python and have limited experience with C++, so I used ChatGPT (OpenAI) as a support tool during this assignment.

1. **C++ Code Structure and Syntax:** Used AI to understand C++ class structure, syntax, and correct compilation errors.
2. **Code Review and Debugging:** Used AI to review my implementation, identify logical issues, and suggest corrections, especially in timestamp handling.
3. **CSV Analysis:** Used AI to analyze the GNSS data and understand the reasons for rejected frames.
4. **Documentation:** Used AI to improve the wording and structure of my assignment report.

AI also suggested code improvements that I incorporated into the implementation. I reviewed the changes and used testing to evaluate the results.
