# Take-Home-Assignment-GNSS-Processor
# GNSS Processor — Take-Home Assignment

**Candidate:** Sanskriti Vishwakarma

## Overview

This project implements a GNSS data processor in C++17 for an outdoor mobile robot. It validates incoming GNSS frames, rejects unreliable measurements, and prints valid positions.

## Features

- Validates latitude and longitude.
- Checks GNSS fix type, HDOP, and satellite count.
- Detects invalid timestamps and excessive timestamp gaps.
- Supports automatic recovery after rejected frames.
- Provides unit tests and CSV data replay.

## Project Files

- `gnss_processor.h` — GNSS frame structure and processor class.
- `gnss_processor.cpp` — Validation implementation.
- `test_gnss_processor.cpp` — Unit tests.
- `read.cpp` — CSV replay utility.
- `gnss.csv` — Recorded GNSS data.
- `sanskriti_vishwakarma_response.md` — Assignment answers, design assumptions, and AI usage disclosure.

## Requirements

- C++17 compatible compiler (G++)
- No external libraries required.

## Compile and Run

### Run Unit Tests

```bash
g++ -std=c++17 test_gnss_processor.cpp gnss_processor.cpp -o test_gnss
./test_gnss
```

### Replay GNSS Data

```bash
g++ -std=c++17 read.cpp gnss_processor.cpp -o gnss_reader
./gnss_reader gnss.csv
```

## Validation Criteria

| Parameter | Accepted Values |
|---|---|
| Latitude | -90° to +90° |
| Longitude | -180° to +180° |
| Fix Type | 1 or 2 |
| HDOP | Greater than 0 and at most 5.0 |
| Satellites | At least 4 |
| Timestamp | Strictly increasing |
| Maximum Timestamp Gap | 1500 ms |

Coordinates and HDOP must also be finite.

## Recorded GNSS Results

Using the corrected processor with the supplied 500-frame recording:

- Accepted frames: 459
- Rejected frames: 41
- Acceptance rate: 91.8%

## Additional Information

For implementation assumptions, validation rules, rejection recovery behavior, proposed improvements, and AI usage details, refer to `sanskriti_vishwakarma_response.md`.
