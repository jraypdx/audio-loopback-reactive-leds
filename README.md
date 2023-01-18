# pi-loopback-reactive-leds
Audio loopback from Windows to a Raspberry Pi network socket to control ws2812b LEDs

A big thank you to Quozul for their Audio-Visualizer project in C#, which finally got me to understand FFT and audio frequency processing: https://github.com/Quozul/Audio-Visualizer


This project is two parts:
1. A C# program that captures loopback audio, processes it, and sends the data over a UDP socket
2. A python script that runs on a Raspberry Pi, receives data on a UDP socket, and controls ws2812b LEDs

So far it is very basic and I've just been using a 9-LED test strip

TODO:
 - GUI to select loopback device (if needed?)
 - Various types of reactive patterns in the Python script to choose from
 - Clean up and comment code
 - Try out getting it to work by capturing audio output from a virtual loopback device on Apple Macs
 
