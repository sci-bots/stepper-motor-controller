# stepper-motor-controller #

Package for controlling a stepper motor via remote procedure calls (RPC) utilizing
[`base-node-rpc`][3].

## Overview ##

This package contains:

 - Firmware compatible with Arduino Uno.
 - Installable Python package for interfacing with Arduino firmware through
   serial port or i2c (through a serial-to-i2c proxy).

## Install ##

The Python package can be installed through `pip` using the following command:

    pip install stepper-motor-controller

## Upload firmware ##

To upload the pre-compiled firmware included in the Python package, run the
following command:

    python -m stepper_motor_controller.bin.upload uno

This will attempt to upload the firmware by automatically discovering the
serial port.  On systems with multiple serial ports, use the `-p` command line
argument to specify the serial port to use.  For example:

    python -m stepper_motor_controller.bin.upload -p COM3 uno

--------------------------------------------------

## Usage ##

After uploading the firmware to the board, the `stepper_motor_controller.Proxy` class can be
used to interact with the Arduino device.

See the session log below for example usage.

### Example interactive session ###

    >>> from stepper_motor_controller import SerialProxy

Initialize a serial proxy (the computer will scan all available serial ports until it finds a match).

    >>> proxy = SerialProxy()

Query the number of bytes free in device RAM.

    >>> proxy.ram_free()
    325 

Query descriptive properties of device.

    >>> proxy.properties
    base_node_software_version
    package_name                                           stepper-motor-controller
    display_name                                           stepper-motor-controller
    manufacturer                                                      Sci-Bots Inc.
    url                           http://github.com/sci-bots/stepper-motor-contr...
    software_version                                                      0.1.post3
    hardware_version                                                            0.1
    dtype: object

Use Arduino API methods interactively.

    >>> # Set pin 13 as output
    >>> proxy.pin_mode(13, 1)
    >>> # Turn led on
    >>> proxy.digital_write(13, 1)
    >>> # Turn led off
    >>> proxy.digital_write(13, 0)

### Configuration and state ###

The device stores a *configuration* and a *state*.  The configuration is
serialized and stored in EEPROM, allowing settings to persist across device
resets.  The state is stored in device memory and is reinitialized each time
the device starts up.

Print (non-default) configuration values.

    >>> proxy.config
    steps_per_revolution    200
    i2c_address              10
    microstep_setting         1
    dtype: object

Configuration settings can be set by updating the configuration.

    >>> result_code = proxy.update_config(i2c_address=32)

### Other methods ###

Below is a list of the attributes of the `stepper_motor_controller.Proxy` Python class.  Note
that many of the [Arduino API][1] functions (e.g., `pin_mode`, `digital_write`,
etc.) are exposed through the RPC API.

    >>> proxy.
    analog_read                     max_i2c_payload_size
    analog_write                    max_serial_payload_size
    array_length                    microseconds
    base_node_software_version      milliseconds
    begin                           move
    delay_ms                        on_config_i2c_address_changed
    delay_us                        on_config_microstep_setting_changed
    digital_read                    package_name
    digital_write                   pin_mode
    display_name                    ram_free
    echo_array                      read_eeprom_block
    eeprom_e2end                    reset
    get_buffer                      reset_config
    hardware_version                reset_state
    help                            save_config
    i2c_address                     serialize_config
    i2c_available                   serialize_state
    i2c_buffer_size                 set_clock
    i2c_disable_broadcast           set_i2c_address
    i2c_enable_broadcast            software_version
    i2c_packet_reset                stop
    i2c_read                        str_echo
    i2c_read_byte                   terminate
    i2c_request                     update_config
    i2c_request_from                update_eeprom_block
    i2c_scan                        update_state
    i2c_write                       url

--------------------------------------------------

## Firmware development ##

The Arduino firmware/sketch is located in the `stepper_motor_controller/Arduino/stepper_motor_controller`
directory.  The key functionality is defined in the `stepper_motor_controller::Node` class in
the file `Node.h`.

Running the following command will build the firmware using [SCons][2]
for Arduino Uno, and will package the resulting firmware in a Python package,
ready for distribution.

    paver bdist_wheel 

### Adding new remote procedure call (RPC) methods ###

New methods may be added to the RPC API by adding new methods to the
`stepper_motor_controller::Node` class in the file `Node.h`.

# Authors #

Christian Fobel <christian@fobel.net>
Ryan Fobel <ryan@fobel.net>

[1]: https://www.arduino.cc/en/Reference/HomePage
[2]: http://www.scons.org/
[3]: https://github.com/wheeler-microfluidics/base_node_rpc
