# sop25-9999
## Bluetooth
Idea: use multiple Atom lites to detect how many bluetooth devices are active in a given room.
The information from multiple builidings is syncronized to a central database.
A user will be able to load a web page and click on a room to get the near-real-time occupation of said room.

**Requirements:**

- List current near bluetooth devices
- Remember bluetooth devices to not count twice
- Updates every 5 minutes
- Low power
- Sends number of connections over wifi to webserver
- Webserver has to get data from multiple atoms
- Webserver must sense if atoms are alive, notify if dead
- Atoms must automatically reboot if error
- ... ?

### Steps (mostly AI-generated)
1.  **M5Stack Atom Lite Setup:**
    * Install and configure the Arduino IDE for ESP32 development. 
    * Install necessary libraries such as M5Atom and M5Unified. 
2.  **Bluetooth Scanning Implementation:**
    * Develop C code to scan for Bluetooth Low Energy (BLE) devices using libraries like M5Atom or M5Unified. 
    * Implement logic to extract and store unique device MAC addresses to avoid counting the same device multiple times. 
    * Determine and set an appropriate scanning interval, balancing responsiveness and resource usage. 
3.  **Data Transmission to Server:**
    * Establish a Wi-Fi connection from the Atom Lite to the local network. 
    * Implement HTTP POST requests to send the Bluetooth device count to the Python server. 
    * Format the data in JSON format, including a unique identifier for each Atom Lite. 
    * Handle the server's response and implement error checking. 
4.  **Python Server Setup:**
    * Install Python and the Flask framework. 
    * Create a Flask application and define routes to handle incoming HTTP POST requests. 
    * Implement data storage on the server, initially using an in-memory Python dictionary. 
5.  **Webpage Development:**
    * Create a basic HTML webpage to display the room occupancy information. 
    * Use Flask to serve the webpage and pass the occupancy data to it. 
    * Implement data placeholders in the HTML to dynamically display the occupancy count. 
    * Since manual refresh is acceptable, no complex real-time update techniques are required. 
6.  **Occupancy Estimation Logic:**
    * Define occupancy levels (e.g., Empty, Low, Medium, High) based on predefined ranges or thresholds of detected Bluetooth devices. 
    * Consider calibrating the system by manually counting people and correlating it with Bluetooth device counts to improve accuracy. 
    * Acknowledge the limitations of Bluetooth-based estimation, such as variations in device usage and potential over/underestimation. 
7.  **Testing and Integration:**
    * Test individual components (Atom Lite, server, webpage) to ensure they function correctly.
    * Integrate the components and perform end-to-end testing of the complete system.
    * Deploy the system in the actual environment and conduct final testing and calibration.

## Documentation
| Website | URL |
|-|--|
| ATOM Lite | <https://docs.m5stack.com/en/core/atom_lite> |
| Arduino | <https://www.arduino.cc/en/Guide> |
| Arduino Libraries | <https://www.arduino.cc/en/Hacking/Libraries> |
| Arduino M5stack | <https://docs.m5stack.com/en/products> |
| Arduino Bluetooth functions | <https://docs.arduino.cc/libraries/arduinoble/> |
| Arduino HTTP request (tutorial) | <https://arduinogetstarted.com/tutorials/arduino-http-request>

## Build and run the project
To build and run the project, you will have to have `make` and `gcc` installed. To install them, do:

```bash
sudo apt install make
sudo apt install gcc
```

Or simply run the following to install essentials (same outcome)

```bash
sudo apt install build-essential
```

Once you have everything installed, you can run:

```bash
make
```

to build the project, and to run the project, type:

```bash
make run
```

> Note: will have to add a command to upload the code to the controller with `make upload`.
> It still depends on what tool we will use to upload the code to the chip. Currently I have experienced with [PlatformIO](https://platformio.org/). To use it, you will have to have python envrionments installed: `sudo apt install python3-venv` (conda does not seem to be supported at the moment), and then you can install the tool either through pip: `python3 -m pip install -U platformio` (you can find other installation procedures [here](https://docs.platformio.org/en/latest/core/installation/index.html) or you can install everything more easily with their [VScode extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)).

## Project structure

> Will have to be revised to divide the work into server and chip. 

The project is structured as follow:
```
./
└ bin/
└ build/
└ docs/
└ include/
└ lib/
└ src/
└ test/
└ main.c
└ makefile
└ README.md
```

- The `/` folder is the root folder of the project. 
- The `build/` folder contains all binary files produced during compilation. 
- The `docs/` folder contains documentation for our code (in the future will probably automatically be generated by [doxygen](https://www.doxygen.nl/)). 
- The `include/` folder contains header files for the corresponding `.c` files.
- The `lib/` folder may contain external libraries.
- The `src/` folder contains source files for the correspononding `.h` files.
- The `test/` folder may contain tests (if we use any).
- The `main.c` file is the entrypoint to the program.
- The `README.md` is the file you are reading.

**********************

Websensor
=========

Introduction
------------

The meta goal of this project is to get accommodated with the C language and different targets it can be compiled to.

Learning goals are:

-   Creating a C project from scratch.
-   Working with C language concepts. (Arrays, Pointers, Data Structures)
-   Incorporating resources from the internet.
-   Autonomously figuring out how to compile C code to different targets (microprocessor).

Your task is to implement a working solution of a websensor based on a low cost microprocessor (ESP8266).

Deliverables to be ready before the final course on Wed, 21. May 2025:

-   Preparation of a final presentation per group (~12min).
-   The runnable code, including testing scripts provided on GitLab with working automated build scripts.
-   A short two pages documentation per group.
-   An individual one page report per participant (Problems encountered, Lessons learned.)

Groups
------

For the project you build up groups with 4 (four) students.

1.  Head on [Moodle](https://moodle.unifr.ch/mod/choicegroup/view.php?id=1487139) to join a group.
2.  Head to the [Gitlab group for the projects](https://diuf-gitlab.unifr.ch/sop2025-projects).
3.  Create a new GitLab Project. Use your own, more or less creative `GROUPNAME`.
4.  Finally add all group members to the GitLab Project.

Minimal Features
----------------

![](https://sop.course-unifr.ch/projet/atom_lite_01.png)

-   Aquisision of the sensor value of your chosen sensor.
-   Either send (push) the aquired values to a chosen or implemented web API, or provide a web API on the device itself.
-   Keeping the maximum amount of past values in memory of the microprocessor. Think about a datastructure optimizing for maximum amount of data.

Implementation decisions
------------------------

-   Usage of a library, or implementation of own code for data aquisision.
-   Which data structure to hold the pointers / data. Array, Linked-List, ...
-   Create a server API vs a client API.
-   The architecture of your code, structure of files.

Report the advantages and disadvantages of your implementation decisions regarding API decision and memory usage in the report mentioned in the deliverables.

Resources
---------

[ATOM Lite](https://docs.m5stack.com/en/core/atom_lite)\
[Sensors](https://shop.m5stack.com/collections/m5-sensor)\
[Arduino](https://www.arduino.cc/en/Guide)\
[Include Libraries](https://www.arduino.cc/en/Hacking/Libraries)\
[Arduino M5Stack](https://docs.m5stack.com/en/quick_start/atom/arduino)

Evaluation Key
--------------

The following aspects of your work will be taken into account for the final mark with the according weights.

### Implemented Features (60%)

-   Is the minimal feature set implemented and can be tested ?
-   Does the code compiles and run on the other targets ?
-   Is a testing program / script present ?

### Code Quality (20%)

-   Is the code simple and concise, easy to read ?
-   Is the code well structured and the necessary helperfiles for building present (Makefile).

### Documentation & Presentation (20%)

-   Final 3 min/Person Presentation
    -   The time limit is properly respected.
    -   The content of the persentation (show something exciting!)
-   Short Project Documentation (max. 2 pages per Group)
    -   What is the architecture and which decisions did you take.
    -   Describe the usage of store and show it can be tested.
-   Individual Lessons Learned (max. 1 pages per Person)
    -   What were the most challenging bits of the course including project?
    -   Any never seen concepts learned?