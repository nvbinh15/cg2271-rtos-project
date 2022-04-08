# CG2271-Project AY21/22 Sem 2

The project involved assembling and programming a robot based on Real-Time Operating System concepts taught throughout the semester and complete 2 different challenge runs

## Main components

* **FRDM-KL25Z** (Brain)
* **ESP32** (For command transmission via WiFi)
* **Motors** and **Wheels** (For movement)
* **Green Led** strip and 1x **Red Led** strip
* **Buzzer** (For playing audio)
* **HMC5883** (Compass)
* **MPU-6050** (Gyroscope, accelorometer)
* **HC-SR04** (Ultrasound)
* **LiPo** batteries (To power components)

## Main features

Serial communications over WiFi were used to remotely control a robot which would be able to simultaneously carry out various features with a multi-threaded environment

* **Green and Red LEDs**
    * Moving: Green LEDs cycle pattern, Red LEDs flash at 500ms intervals
    * Stationary: Green LEDs fully lighted, Red LEDs flash at 250ms intervals
* **Movement**
    * Forward/Backward
    * Left/Right static turn 
    * Left/Right Forward and Backward curved turns
* **Audio**
    * Audio tune during the challenge
    * Audio tune at the end of the challenge
* **Autorun**
    * Moving straight until detecting an obstacle and navigating around it back to the starting point automatically
