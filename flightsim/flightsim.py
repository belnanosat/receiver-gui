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

def calc_checksum(a):
    checksum = 0
    for b in a:
        checksum = checksum ^ b
    return checksum

while run:
    if curx < 100.0 or packet.altitude > 5.0:
        packet.altitude = -5 * (curx - 100.0)**2 + 50001.0
        packet.temperature_internal = -packet.altitude / 1000.0 + 20
        packet.temperature_external = -packet.altitude / 1000.0
        packet.pressure = int((100.0 - packet.altitude / 2500.0) * 1000)
        packet.bmp180_temperature = 10#int(-packet.altitude / 1000.0 + 20 + 1.0)
        packet.longitude = start_longitude + cur_radius * math.cos(cur_angle)
        packet.latitude = start_latitude + cur_radius * math.sin(cur_angle)
        packet.radiation = 10.0 + packet.altitude / 1000.0
        packet.status = ((1 << (int(curx) % 20)) ^ 0xFFFFFF)
        cur_radius += 0.003
        cur_angle += math.pi / 4.0
    packet.packet_id += 1
    curx += 1.0
    packet.voltage -= 0.01

    curs = packet.SerializeToString();
    ress = struct.pack('B', len(curs))
    ress += curs
    ress += struct.pack('B', calc_checksum(ress))
    while len(ress) < 59:
        ress += struct.pack('B', 0)
    # print("Length: {}".format(len(packet.SerializeToString())))
    # print(google.protobuf.text_format.MessageToString(packet, as_one_line=True))
    for c in ress:
        sys.stdout.buffer.write(struct.pack('B', c))
        sys.stdout.buffer.flush()
        time.sleep(0.01)
    time.sleep(0.1)
