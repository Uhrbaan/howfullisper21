# 999
The goal of this project is to estimate room occupancy in PER21.

## Firmware
The project uses the [M5Stack Atom-Lite](https://docs.m5stack.com/en/core/ATOM%20Lite) 
boards to measure the number of Bluetooth connections.

### Building
The firmware is configured using [PlatformIO](https://platformio.org/). To build
and load the code on the esp32, you can run:

```sh
make firmware
```

Which will use the PlatformIO-CLI to build and load the project on the chip, or,
alternatively, you can open the `firmware` folder with the PlatformIO IDE.

## Server
The project uses [Flask](https://flask.palletsprojects.com/en/stable/), a python
web-server to handle the data collected from the chips. To run the server 
locally, you can run:

```sh
make server
```

## Documentation
The code uses [Doxygen](https://doxygen.nl/) to generate its documentation. To
generate the documentation, run:

```sh
doxygen doxyfile
```

The documentation will generate in the `docs` folder.