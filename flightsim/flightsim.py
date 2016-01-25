#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import telemetry_pb2
import time
import google
import sys
import math
import struct

packet = telemetry_pb2.TelemetryPacket()
packet.packet_id = 0
packet.status = 0xFFFFFF
packet.altitude = 0.0
packet.longitude = 27.561524
packet.latitude = 53.904540
packet.temperature_internal = 0
packet.temperature_external = 0
packet.pressure = 0
packet.bmp180_temperature = 0
packet.voltage = 5.0
packet.radiation = 10.0
start_longitude = 27.561524
start_latitude = 53.904540
cur_radius = 0.003
cur_angle = 0
curx = 0
run = True

while run:
    if curx < 100.0 or packet.altitude > 5.0:
        packet.altitude = -5 * (curx - 100.0)**2 + 50001.0
        packet.temperature_internal = -packet.altitude / 1000.0 + 20
        packet.temperature_external = -packet.altitude / 1000.0
        packet.pressure = int((100.0 - packet.altitude / 2500.0) * 1000)
        packet.bmp180_temperature = int(-packet.altitude / 1000.0 + 20 + 1.0)
        packet.longitude = start_longitude + cur_radius * math.cos(cur_angle)
        packet.latitude = start_latitude + cur_radius * math.sin(cur_angle)
        packet.radiation = 10.0 + packet.altitude / 1000.0
        packet.status = ((1 << (int(curx) % 20)) ^ 0xFFFFFF)
        cur_radius += 0.003
        cur_angle += math.pi / 4.0
    packet.packet_id += 1
    curx += 1.0
    packet.voltage -= 0.01
#    print("Length: {}".format(len(packet.SerializeToString())))
    curs = google.protobuf.text_format.MessageToString(packet, as_one_line=True)
    hashsum = 0
    for b in bytearray(curs, 'utf8'):
        hashsum = hashsum ^ b
    # Simulation of noise in a radio channel
    if curx % 2 == 0:
        hashsum = hashsum ^ 1
    curs += "*" + "%0.2X" % hashsum
    # curs = packet.SerializeToString();
    # curs += struct.pack('B', hashsum)
    # for b in curs:
    #     hashsum = hashsum ^ b
    # if curx % 2 == 0:
    #     hashsum = hashsum ^ 1
    for c in curs:
       # sys.stdout.buffer.write(struct.pack('B', c))
       # sys.stdout.buffer.flush()
        sys.stdout.write(c)
        sys.stdout.flush()
        time.sleep(0.01)
    sys.stdout.write('\n')
    sys.stdout.flush()
    time.sleep(0.001)
#    run = False
