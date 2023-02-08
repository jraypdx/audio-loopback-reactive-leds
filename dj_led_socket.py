import socket
import numpy as np
import time
import board
import neopixel
import signal
import sys
from random import randrange

PIN = board.D18
NUM_PIXELS = 96#60#9
ORDER = neopixel.GRB
BRIGHTNESS = 0.1

SLEEP_MS = 50 / 1000.0

pixels = neopixel.NeoPixel(pin=PIN, n=NUM_PIXELS, brightness=BRIGHTNESS, auto_write=False, pixel_order=ORDER)
pixels2 = neopixel.NeoPixel(pin=board.D21, n=60, brightness=0.4, auto_write=False, pixel_order=ORDER)



def eq_visual(values):
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

def eq_visual_49_ring(values):
    map_pixels_49_ring(8, [int(120 * values[0]), 0, int(255 * values[0])])
    map_pixels_49_ring(7, [int(80 * values[1]), 0, int(255 * values[1])])
    map_pixels_49_ring(6, [0, 0, int(255 * values[2])])
    map_pixels_49_ring(5, [0, int(200 * values[3]), int(255 * values[3])])
    map_pixels_49_ring(4, [0, int(255 * values[4]), int(120 * values[4])])
    map_pixels_49_ring(3, [0, int(255 * values[5]), 0])
    map_pixels_49_ring(2, [int(255 * values[6]), int(200 * values[6]), 0])
    map_pixels_49_ring(1, [int(255 * values[7]), int(80 * values[7]), 0])
    map_pixels_49_ring(0, [int(255 * values[8]), 0, 0])

    pixels.show()

def eq_visual_49_ring_24(values):
    map_pixels_49_ring_24(23, [int(255 * values[0]), 0, int(255 * values[0])])
    map_pixels_49_ring_24(22, [int(210 * values[1]), 0, int(255 * values[1])])
    map_pixels_49_ring_24(21, [int(170 * values[1]), 0, int(255 * values[1])])
    map_pixels_49_ring_24(20, [int(140 * values[1]), 0, int(255 * values[1])])
    map_pixels_49_ring_24(19, [int(100 * values[1]), 0, int(255 * values[1])])
    map_pixels_49_ring_24(18, [int(60 * values[1]), 0, int(255 * values[1])])
    map_pixels_49_ring_24(17, [int(20 * values[1]), 0, int(255 * values[1])])
    map_pixels_49_ring_24(16, [0, 0, int(255 * values[2])])
    map_pixels_49_ring_24(15, [0, int(200 * values[3]), int(255 * values[3])])
    map_pixels_49_ring_24(14, [0, int(255 * values[4]), int(200 * values[4])])
    map_pixels_49_ring_24(13, [0, int(255 * values[4]), int(170 * values[4])])
    map_pixels_49_ring_24(12, [0, int(255 * values[4]), int(120 * values[4])])
    map_pixels_49_ring_24(11, [0, int(255 * values[4]), int(90 * values[4])])
    map_pixels_49_ring_24(10, [0, int(255 * values[4]), int(50 * values[4])])
    map_pixels_49_ring_24(9, [0, int(255 * values[4]), int(30 * values[4])])
    map_pixels_49_ring_24(8, [0, int(255 * values[5]), 0])
    map_pixels_49_ring_24(7, [int(255 * values[6]), int(255 * values[6]), 0])
    map_pixels_49_ring_24(6, [int(255 * values[6]), int(220 * values[6]), 0])
    map_pixels_49_ring_24(5, [int(255 * values[7]), int(200 * values[7]), 0])
    map_pixels_49_ring_24(4, [int(255 * values[7]), int(180 * values[7]), 0])
    map_pixels_49_ring_24(3, [int(255 * values[7]), int(150 * values[7]), 0])
    map_pixels_49_ring_24(3, [int(255 * values[7]), int(120 * values[7]), 0])
    map_pixels_49_ring_24(2, [int(255 * values[7]), int(80 * values[7]), 0])
    map_pixels_49_ring_24(1, [int(255 * values[7]), int(30 * values[7]), 0])
    map_pixels_49_ring_24(0, [int(255 * values[8]), 0, 0])
    pixels.show()
    pixels2.show()

def eq_visual_mapped(values):
    map_pixels_60_strip(8, [int(120 * values[0]), 0, int(255 * values[0])])
    map_pixels_60_strip(7, [int(80 * values[1]), 0, int(255 * values[1])])
    map_pixels_60_strip(6, [0, 0, int(255 * values[2])])
    map_pixels_60_strip(5, [0, int(200 * values[3]), int(255 * values[3])])
    map_pixels_60_strip(4, [0, int(255 * values[4]), int(120 * values[4])])
    map_pixels_60_strip(3, [0, int(255 * values[5]), 0])
    map_pixels_60_strip(2, [int(255 * values[6]), int(200 * values[6]), 0])
    map_pixels_60_strip(1, [int(255 * values[7]), int(80 * values[7]), 0])
    map_pixels_60_strip(0, [int(255 * values[8]), 0, 0])

    pixels.show()

def map_pixels_49_ring(group, color):
    section = []

    if (group == 8):
        section = [0, 1, 29, 30]
    elif (group == 7):
        section = [2, 3, 27, 28, 31, 32]
    elif (group == 6):
        section = [4, 5, 25, 26, 33, 34]
    elif (group == 5):
        section = [6, 7, 23, 24, 35, 36]
    elif (group == 4):
        section = [8, 9, 21, 22, 37, 38]
    elif (group == 3):
        section = [10, 11, 19, 20, 39, 40, 48]
    elif (group == 2):
        section = [12, 17, 18, 41, 42, 47]
    elif (group == 1):
        section = [13, 16, 43, 46]
    elif (group == 0):
        section = [14, 15, 44, 45]

    for p in section:
        pixels[p] = color
        pixels[p + 47] = color

def map_pixels_49_ring_24(group, color):
    section = []

    if (group == 23):
        section = [1, 0, 47]
    elif (group == 22):
        section = [2, 46]
    elif (group == 21):
        section = [3, 45]
    elif (group == 20):
        section = [4, 44]
    elif (group == 19):
        section = [5, 43]
    elif (group == 18):
        section = [6, 42]
    elif (group == 17):
        section = [7, 41]
    elif (group == 16):
        section = [8, 40]
    elif (group == 15):
        section = [9, 39]
    elif (group == 14):
        section = [10, 38]
    elif (group == 13):
        section = [11, 37]
    elif (group == 12):
        section = [12, 36]
    elif (group == 11):
        section = [13, 35]
    elif (group == 10):
        section = [14, 34]
    elif (group == 9):
        section = [15, 33]
    elif (group == 8):
        section = [16, 32]
    elif (group == 7):
        section = [17, 31]
    elif (group == 6):
        section = [18, 30]
    elif (group == 5):
        section = [19, 29]
    elif (group == 4):
        section = [20, 28]
    elif (group == 3):
        section = [21, 27]
    elif (group == 2):
        section = [22, 26]
    elif (group == 1):
        section = [23, 25]
    elif (group == 0):
        section = [24]

    for s in section:
        s = s + 12
        if (s > 47):
            s = s - 48
        pixels[s] = color
        pixels[NUM_PIXELS - 1 - s] = color  # second ring

    map_pixels_60_strip_24(group, color)
    # pixels[group] = color
    # pixels[group + 24] = color
    # pixels[group + 48] = color
    # pixels[group + 72] = color


def map_pixels_60_strip(group, color):
    """
        Set a group/section of pixels for eq_visual_mapped() mapping when using more than the 9-led test strip
        Parameters:
            group: integer with 0 being the lowest bass and 8 being the highest frequency bin
            color: triplet array of integers in range 0-255 to set the color of the group to
    """
    section = []

    if (group == 8):
        section = [0, 1, 29, 30, 58, 59]
    elif (group == 7):
        section = [2, 3, 27, 28, 31, 32, 56, 57]
    elif (group == 6):
        section = [4, 5, 25, 26, 33, 34, 54, 55]
    elif (group == 5):
        section = [6, 7, 23, 24, 35, 36, 52, 53]
    elif (group == 4):
        section = [8, 9, 21, 22, 37, 38, 50, 51]
    elif (group == 3):
        section = [10, 11, 19, 20, 39, 40, 48, 49]
    elif (group == 2):
        section = [12, 17, 18, 41, 42, 47]
    elif (group == 1):
        section = [13, 16, 43, 46]
    elif (group == 0):
        section = [14, 15, 44, 45]

    for p in section:
        pixels[p] = color


def map_pixels_60_strip_24(group, color):
    """
        Set a group/section of pixels for eq_visual_mapped() mapping when using more than the 9-led test strip
        Parameters:
            group: integer with the highest being the lowest bass and 0 being the highest frequency bin (I did it backwards lol)
            color: triplet array of integers in range 0-255 to set the color of the group to
    """
    section = []

    if (group == 23):
        section = [28,29,30,31]
    elif (group == 22):
        section = [26,27,32,33]
    elif (group == 21):
        section = [24,25,34,35]
    elif (group == 20):
        section = [22,23,36,37]
    elif (group == 19):
        section = [20,21,38,39]
    elif (group == 18):
        section = [18,19,40,41]
    elif (group == 17):
        section = [17,42]
    elif (group == 16):
        section = [16,43]
    elif (group == 15):
        section = [15,44]
    elif (group == 14):
        section = [14,45]
    elif (group == 13):
        section = [13,46]
    elif (group == 12):
        section = [12,47]
    elif (group == 11):
        section = [11,48]
    elif (group == 10):
        section = [10,49]
    elif (group == 9):
        section = [9,50]
    elif (group == 8):
        section = [8,51]
    elif (group == 7):
        section = [7,52]
    elif (group == 6):
        section = [6,53]
    elif (group == 5):
        section = [5,54]
    elif (group == 4):
        section = [4,55]
    elif (group == 3):
        section = [3,56]
    elif (group == 2):
        section = [2,57]
    elif (group == 1):
        section = [1,58]
    elif (group == 0):
        section = [0,59]

    for p in section:
        pixels2[p] = color


def start_stream():
    UDP_IP = "0.0.0.0" #"127.0.0.1"
    UDP_PORT = 6969
    BUFFER_LENGTH = 24 * 8#9 * 8  # number of bytes we receive each time (array of 9 doubles)
 
    sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
    sock.bind((UDP_IP, UDP_PORT))

    while True:
        data, addr = sock.recvfrom(BUFFER_LENGTH)
        try:
            values = np.frombuffer(data, dtype=np.double)
        except Exception as e:
            print("exception: " + e)

        # print("received message: %s" % data)
        # eq_visual(values)
        #eq_visual_mapped(values)
        eq_visual_49_ring_24(values)




def signal_handler(sig, frame):
    pixels.fill((0, 0, 0))
    pixels.show()
    pixels2.fill((0, 0, 0))
    pixels2.show()
    print('Stopping pixel pattern')
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)


pixels.fill((0, 0, 0))
pixels.show()
start_stream()
