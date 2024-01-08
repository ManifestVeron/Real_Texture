#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import random
import argparse


def main():
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument('-o', '--output', default="track_cam_light.csv")
    argument_parser.add_argument('-p', '--points', type=int, default=30)
    args = argument_parser.parse_args()

    track_name = args.output

    with open(track_name, "w") as track_file:
        for i in range(1, args.points + 1):

            # границы зоны по x
            xc = random.uniform(-151100.0, -70460.0)

            # границы зоны по y
            yc = random.uniform(-264300.0, -163800.0)

            zc = 78620.0

            rx = 0.0
            ry = 270.0
            rz = random.uniform(0.0, 360.0)

            lux = random.uniform(2.0, 18.0)
            l_rx = -149.0
            l_ry = random.uniform(-30.0, -150.0)
            l_rz = random.uniform(0.0, 360.0)

            print("Coordinates:", xc, yc, zc, rx, ry, rz)
            print("Light settings: intensity --> ", lux, " || angles --> ", l_rx, l_ry, l_rz)
            print()
            track_file.write("%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n" % (xc, yc, zc,
                                                                  rx, ry, rz,
                                                                  lux,
                                                                  l_rx, l_ry, l_rz
                                                                  ))
    print("TRACK -= CAM AND LIGHT (OBZOR) =- IS READY")


if __name__ == "__main__":
    main()
