# CO3009 - Two-Road Traffic Light System

## 1. Project Description
This is an intersection traffic light system project utilizing the STM32F103RB microcontroller on the XNUCLEO-F103RB board. The system integrates an $O(1)$ scheduler and includes the following main features:
* **Automatic mode:** The Red - Green - Amber lights of the two roads cycle alternately according to configured durations.
* **Manual mode:** The user changes the light states using push buttons.
* **Tuning mode:** The durations of the Red - Green - Amber lights are adjusted using push buttons, with configuration info displayed on the LCD.
* **Environment display:** Reads the DHT20 sensor and shows the current temperature and humidity on the LCD.

**Team Members:**
* Nguyen Thanh Luan (Student ID: 2452711)
* Doan Anh Kiet (Student ID: )

## 2. Folder Structure
* `Core/Inc/`: Contains shared header files and modules (`global.h`, `scheduler.h`, `software_timer.h`, `button.h`, `traffic_light.h`, `dht20.h`, `lcd.h`, `fsm_automatic.h`, `fsm_manual.h`, `fsm_tuning.h`).
* `Core/Src/`: Contains the corresponding `.c` source files that implement the logic.

## 3. Build and Flashing Instructions
1. Clone the repository to your local machine.
2. Open STM32CubeIDE and import the project from the cloned folder. Ensure the project target is configured for the **NUCLEO-F103RB** board and the Debug setting is set to **Serial Wire**.
3. Click **Build**.
4. Connect the board to your computer and click **Run** to flash the program.

## 4. How to Operate