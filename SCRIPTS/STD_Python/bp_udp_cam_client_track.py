#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import struct
import time
import random
import os
import argparse


class UDPCamClient(object):
    def __init__(self, ip: str = "127.0.0.1", port: int = 9898):
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.__addr = (ip, port)
    
    def set_pose(self, index, *data_to_send):
        cam_pose = struct.pack('idddddddddd', index, *data_to_send)
        self.__sock.sendto(cam_pose, self.__addr)
        time.sleep(1.0)     # 1.0)

    def __del__(self):
        self.__sock.close()
        print("client socket closed")


def main():
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument('-i', '--input', default=os.path.join(".", "track_ue.csv"))
    args = argument_parser.parse_args()

    track_name = args.input

    cam_client = UDPCamClient()

    i = 1

    with open(track_name, "r") as track_file:
        for row in track_file:
            row_data = row.split(";")
            coords_and_light = list(map(float, row_data))
            print("Camera coordinates:", *coords_and_light[:6])
            print("Light settings: ", *coords_and_light[6:])
            cam_client.set_pose(i, *coords_and_light)
            print("#%s Camera pose sent" % i)
            i += 1


if __name__ == "__main__":
    main()
