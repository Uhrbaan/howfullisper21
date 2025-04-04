> This document is AI-generated, with vers sparse human changes

#   Room Occupancy Estimation System

##   1.  Summary of "Room Occupancy Estimation System"

This system estimates room occupancy by counting Bluetooth-enabled devices using M5Stack Atom Lite microcontrollers. The Atom Lites scan for Bluetooth signals and transmit the counts to a central Python server. The server aggregates data from multiple Atom Lite devices deployed in different rooms or zones and displays the occupancy information on a basic webpage. 

Orignial report: <https://docs.google.com/document/d/1PzAQhCicZ0qW3xPBm9tZAtWYsYnuiY9oKLjmStXJKho/edit?usp=sharing>

**Suggested technology stack:**

* **M5Stack Atom Lite:** A compact development board with an ESP32-PICO-D4 system-on-a-chip, Wi-Fi, and Bluetooth capabilities. It is used for Bluetooth scanning and data transmission. 
* **Python Server (Flask):** A lightweight Python web framework used to create the server for receiving data from the Atom Lites, storing it, and hosting the occupancy webpage. 
* **Webpage:** A simple HTML page served by the Flask server to display the room occupancy information. 

**Important Links:**

* **M5Stack Atom Lite Documentation:**
    * [https://docs.m5stack.com/en/core/ATOM%20Lite](https://docs.m5stack.com/en/core/ATOM%20Lite)
* **M5Stack Arduino Library:** [https://github.com/m5stack/M5Atom](https://github.com/m5stack/M5Atom)
* **Flask Documentation:** [https://flask.palletsprojects.com/en/stable/quickstart/](https://flask.palletsprojects.com/en/stable/quickstart/)
* **ESP32 HTTP POST with Arduino IDE:** [https://randomnerdtutorials.com/esp32-http-post-ifttt-thingspeak-arduino/](https://randomnerdtutorials.com/esp32-http-post-ifttt-thingspeak-arduino/)

##   2.  Sub-Tasks for the Project

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

##   3.  Workload Distribution Suggestion (4 Team Members)

To facilitate collaboration and knowledge sharing, the workload is distributed among the four team members as follows:

* **Members 1 & 2: M5Stack Atom Lite Development**

    * Task: Atom Lite Setup, Bluetooth Scanning Implementation, Data Transmission to Server
    * Responsibilities:
        * Setting up the Arduino environment and configuring it for ESP32.
        * Installing and managing necessary libraries (M5Atom, M5Unified).
        * Writing and debugging C code for Bluetooth scanning, including device discovery and MAC address handling.
        * Implementing Wi-Fi connectivity and constructing HTTP POST requests to send data to the server.
        * Ensuring data is correctly formatted in JSON and that each Atom Lite is uniquely identified.
        * Testing the Atom Lite's hardware and software functionality, including Bluetooth scanning range and Wi-Fi communication.
    * Rationale: These tasks are closely related, requiring embedded systems programming skills and familiarity with the Atom Lite's capabilities. Collaboration allows for code reviews, problem-solving, and shared learning, which is crucial for complex hardware-software integration.

* **Members 3 & 4: Server and Webpage Development**

    * Task: Python Server Setup, Webpage Development, Occupancy Estimation Logic
    * Responsibilities:
        * Setting up the Python server environment and installing the Flask framework.
        * Designing and implementing server-side routes to receive and process data from the Atom Lites.
        * Implementing data storage mechanisms (initially a Python dictionary, potentially a database later).
        * Developing the HTML webpage to visualize occupancy data, focusing on clear and informative display.
        * Implementing the logic to translate raw Bluetooth device counts into meaningful occupancy levels.
        * Testing the server's data handling, webpage display, and the accuracy of the occupancy estimation.
    * Rationale: These tasks focus on web development, server-side programming, and data presentation. Working together enables effective design of the user interface, robust server logic, and a well-defined data flow from the Atom Lites to the user.