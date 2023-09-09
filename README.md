# CoffeeRoasterEsp32
Code for ESP32 Coffee roaster. Includes servo motor,thermocouple, file upload/downloads. Pretty crude...

This is an amlagamation of lots of other peoples demos and sample code.
Started with zero knowledge of how to program an ESP32 and just kept adding bells and whistles.

The project was built with arduino ide, and uses several other libraries.
Autoconnect to handle wifi signon functions.
Webserver for well the webserver portion. SPIFFS, ArduinoJson ESP32Servo, max6675, ESM32PWM.

The code has capability to connect via wifi without needing to hardcode any credentials using Autoconnect.
It provides a web server used to control the roasting process. A preheat timer is available.
There is a process to get a configuration of the temperatures of the heat gun that must be run before you
can do a roast. It has to learn how to position the servo motor for the desired temperatures.
There is also a page to graph the configuration data as well as the data collected from a roast.


The Coffee Roaster itself is using:
A flour sifter.
HarborFreight $4 power screwdriver with nut driver to turn the flour sifter.
Harbor Freight Heat gun, the fancier version that has a rotary temperature adjustment control on the back. 
1  5v SPST relay to control the screwdriver Power. 
(I hacked the screwdriver and bypasssed the trigger switch so I can use the relays)
1 5v-DPDT relay to control the direction of the screwdriver.
1 Solid state relay to control power to the heatgun.
1 servo motor to adjust the temperature of the heat gun.
1 Max6675 and thermocouple to read temperature of the heat gun.

This was my first attempt at using an ESP32 and a rude awakening about generating web pages and javascript.
I apologize for how horrible the web interface is, I am not a web designer...
It was developed using the arduino ide. 

I am using a Esp32-wrover dev kit to control everything.
It has a web interface to control things and provides the ability to upload and download files via the web interface.
It starts off using AutoConnect Library so you don't have to add and wifi passwords to the code. And because it is using AutoConnect, it does not work with async webserver so I had to do everything with the standard esp32 server.
I added bells and whistles to log the roasting parameters, and be able to plot the results using HighCharts (https://api.highcharts.com/).

The setup uses a Bauer Heat Gun from Harbor Freight that has a rotary heat adjustment dial on the back.
(https://www.harborfreight.com/power-tools/heat-guns-knives-seamers/heat-guns/1600-watt-12-amp-14-temperature-dual-speed-heat-gun-64112.html)
I removed the dial and 3d printed a part to mate with the heat gun and a servo to adjust the temperature automatically.
I use a solid state relay to turn on/off the heat gun. Have not worked on any cooldown stuff for the heat gun it just gets turned off. 

For the hopper/mixer I am using the cheapest harbor freight powered screwdriver
(4.7.volts https://www.harborfreight.com/power-tools/drills-drivers/cordless-drills-drivers/screwdrivers/48v-cordless-14-in-screwdriver-kit-with-2-insert-bits-and-wall-charger-56544.html),
and a flour sifter.
I modified the flour sifter to be a wobble type from somewhere else on the internet ([add site](https://www.youtube.com/watch?v=79EXPgPf5Ew) )
I hacked into the screwdriver wiring to control the motor so it is controlled by 2 12v Relays. The relays control the direction
of the screwdriver, but I only run it in one direction so it doesn't unscrew the nut on the flour sifter.

I started using the internal temp sensor of the ESP-32 but this uses temperatures that are too high so I am now using a thermocouple to 
monitor the temperatures. The temp monitor stuff is not very sophisticated. I just have the thermocouple mounted in the air flow of the 
heat gun, and am just using it for reference mostly. As long as the temperature numbers are consistent the temperature of the beans
should be "the same-ish".


