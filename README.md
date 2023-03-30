# pi-loopback-reactive-leds
Audio loopback from Windows to a Raspberry Pi network socket to control ws2812b LEDs

A big thank you to Quozul for their Audio-Visualizer project in C#, which finally got me to understand FFT and audio frequency processing: https://github.com/Quozul/Audio-Visualizer


This project is three parts:
1. A C# program that captures loopback audio, processes it, and sends the data over a UDP socket
2. A python script that runs on a Raspberry Pi, receives data on a UDP socket, and controls ws2812b LEDs
3. C++ (?) code for Arduino, receives simplified data over serial, controls multiple ws2812b strips of LEDs

So far it is very basic and I've just been using a 9-LED test strip

DEPENDENCIES:
 - C# (nuget): NAudio.Core, NAudio.Wasapi, MathNet.Numerics
 - Python (pip): numpy, rpi_ws281x, adafruit-circuitpython-neopixel, adafruit-blinka
 - Arduino: Adafruit Neopixel library, board configuration if not a default board in Arduino IDE

TODO:
 - GUI to select loopback device (if needed?), toggle serial/udp socket, and settings for udp and selection for serial
 - Various types of reactive patterns in the Python script to choose from
 - Clean up and comment code - will do once I'm "done" tweaking it and redoing parts
 - Write up a quick demo and guide on how to easily get it working and make changes
 
 Demo gif of LEDs in action and mostly assembled, running off the Arduino implementation over USB-C:
 
![Alt Text](https://github.com/jraypdx/audio-loopback-reactive-leds/blob/main/leds-early-demo.gif)
