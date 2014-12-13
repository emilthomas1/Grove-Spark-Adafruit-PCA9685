== Adafruit 16-channel PWM & Servo driver

This library is for the [Adafruit 16-channel PWM & Servo board](http://www.adafruit.com/products/815) when being used with a Spark Core, and only Spark Core (sorry Arduino)


=== Connections

These displays use I2C to communicate, 2 pins are required to interface:

    SCL -> D1
    SDA -> D0

=== Things to Note

=== Default I2C Slave Address 
With no address jumpers, the default I2C slave address of the board is `100 0000` or `0x40`. This default is set in the header file. If you are altering the address with jumpers, make sure to set it in the software to match.

==== Turning off the LEDs
If you want to turn the LEDs totally off use `setPWM(pin, 4096, 0);` not `setPWM(pin, 4095, 0);`

=== Credits
Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried (Ladyada) for Adafruit Industries.  
BSD license, check LICENSE for more information
All text above must be included in any redistribution
