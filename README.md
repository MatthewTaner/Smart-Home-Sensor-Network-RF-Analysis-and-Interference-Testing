# Smart Home Sensor Network: RF Analysis Project

This project builds a 2.4 GHz smart sensor network and uses a Software Defined Radio (SDR) to analyze its real-world performance, focusing on packet reliability and interference.

This project was developed as a case study in RF engineering, demonstrating the challenges of signal loss in a typical home environment.



---

## 1. Project Objective

The goal was to build a 2.4 GHz sensor network and use an RTL-SDR to:
* Visually identify the sensor's signal from background noise.
* Measure the impact of household interference (Wi-Fi, microwaves).
* Quantify signal reliability (packet loss) at different ranges and through obstructions.

## 2. Hardware Used

* **Sensor Node:** ESP32, nRF24L01+, PIR sensor, DHT11, Door sensor
* **Receiver Node:** ESP32, nRF24L01+
* **Analysis Station:** RTL-SDR, "Ham It Down" v3 Down-converter

## 3. Key Findings

The most critical finding was not from range, but from **Multipath Interference**.

While the sensor had 100% reliability at max range, a specific "medium-range" test through one wall created an RF dead zone with **13.2% packet loss**. This was due to the placement of the transmitter behind a refrigerator. The refrigerator had a large impact on the electromagnetic field, causing consistent interference between the transmitter and the receiver.

### Packet Loss Test Results
| Scenario | Average % Received | Min % Received (Worst Case) |
| :--- | :---: | :---: |
| 1. Baseline (1m) | 100% | 100% |
| 2. Range (10m) | 100% | 100% |
| **3. Obstruction (1 Wall)** | **97.32%** | **86.8%** |
| 4. Max Range (Multi-wall) | 100% | 100% |



### SDR Signal Analysis
Using an SDR, the sensor's signal (on Channel 76 / 2.476 GHz) was successfully identified at its down-converted frequency of **976 MHz**.



## 4. How to Run

1.  **Hardware:** Assemble the transmitter and receiver as shown.
2.  **Code:**
    * Upload `Transmitter_Sensor_Code.ino` to the transmitter ESP32.
    * Upload `Receiver_Code.ino` to the receiver ESP32.
3.  **SDR Analysis:**
    * Connect the RTL-SDR and "Ham It Down."
    * Open SDR# and tune to **976 MHz** (2476 MHz - 1500 MHz LO).
    * Use AM mode with a 10kHz bandwidth to see the signal spikes.

## 5. Project Conclusion

This project successfully demonstrates the #1 challenge in real-world RF engineering: **environment is more important than range.** A simple packet loss test, combined with SDR visualization, is a critical tool for identifying and solving these "dead zones" before deploying a product.
