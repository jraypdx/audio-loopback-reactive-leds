import socket
import numpy as np
import time
import board
import neopixel
import signal
import sys
from random import randrange

PIN = board.D18  # Audio has to be disabled in raspi-config for this pin to work
NUM_PIXELS = 9
ORDER = neopixel.GRB
BRIGHTNESS = 0.05

SLEEP_MS = 50 / 1000.0

pixels = neopixel.NeoPixel(pin=PIN, n=NUM_PIXELS, brightness=BRIGHTNESS, auto_write=False, pixel_order=ORDER)



def eq_visual(values):
    # for v in range(0, len(values)):  # ignore very quiet audio - need to find a good floor value
    #     if values[v] < 0.01:
    #         values[v] = 0.0
    # for v in range(0, len(values)):
    #     if (values[v] == 0.0 and np.sum(pixels[8 - v]) > 0): # doesnt work not writeable
    #         values[v] = np.max(pixels[8 - v])

    pixels[8] = [int(150 * values[0]), 0, int(255 * values[0])]
    pixels[7] = [int(80 * values[1]), 0, int(255 * values[1])]
    pixels[6] = [0, 0, int(255 * values[2])]
    pixels[5] = [0, int(200 * values[3]), int(255 * values[3])]
    pixels[4] = [0, int(255 * values[4]), int(120 * values[4])]
    pixels[3] = [0, int(255 * values[5]), 5]
    pixels[2] = [int(255 * values[6]), int(200 * values[6]), 5]
    pixels[1] = [int(255 * values[7]), int(80 * values[7]), 5]
    pixels[0] = [int(255 * values[8]), 0, 5]

    pixels.show()


def start_stream():
    UDP_IP = "0.0.0.0" #"127.0.0.1"
    UDP_PORT = 6969
    BUFFER_LENGTH = 9 * 8  # number of bytes we receive each time (array of 9 doubles)
 
    sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
    sock.bind((UDP_IP, UDP_PORT))

    while True:
        data, addr = sock.recvfrom(BUFFER_LENGTH) # buffer size is 1024 bytes
        try:
            values = np.frombuffer(data, dtype=np.double)
        except Exception as e:
            print("exception: " + e)

        # print("received message: %s" % data)
        eq_visual(values)




def signal_handler(sig, frame):
    pixels.fill((0, 0, 0))
    pixels.show()
    print('Stopping pixel pattern')
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)


pixels.fill((0, 0, 0))
pixels.show()
start_stream()
