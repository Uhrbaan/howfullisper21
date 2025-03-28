# sop25-9999

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