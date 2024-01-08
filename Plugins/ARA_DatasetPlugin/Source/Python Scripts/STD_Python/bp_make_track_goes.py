#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import math
import os
import random


def recalc_dim_ue(val: float):
    return 100*val


def read_places(places_path: str):
    if not os.path.exists(places_path):
        print("File with objects places does not exists")
        print("You have to build this file")
        exit(1)

    places_list = list()
    with open(places_path, "r") as places_file:
        for raw in places_file:
            separated_raw = raw.split(';')
            places_list.append(tuple(map(float, separated_raw[1:4])))  # we use only (x, y, z)

    return places_list


def calc_cam_pose(**kwargs):
    h = kwargs.get("height")
    r = kwargs.get("radius")
    d = kwargs.get("displacement")
    place = kwargs.get("place")     # place = (x_place, y_place, z_place)

    alpha = random.uniform(-math.pi, math.pi)

    x_cam = r * math.cos(alpha) + place[0]
    y_cam = r * math.sin(alpha) + place[1]
    z_cam = h

    x_t = place[0] + random.randint(-d, d)
    y_t = place[1] + random.randint(-d, d)
    z_t = place[2] + random.randint(-d, d)

    return x_cam, y_cam, z_cam, x_t, y_t, z_t


def main():
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument('-i', '--input', type=str, default="objects_places.csv")
    argument_parser.add_argument('-o', '--output', type=str, default="track_cam_light_goes.csv")
    argument_parser.add_argument('-p', '--num_points', type=int, default=240)
    argument_parser.add_argument('-H', '--uav_height', type=float, default=180)
    argument_parser.add_argument('-r', '--radius_of_circle', type=float, default=100)
    argument_parser.add_argument('-d', '--displacement', type=float, default=80)
    argument_parser.add_argument('-m', '--in_meters', type=bool, default=False)
    args = argument_parser.parse_args()

    places = read_places(args.input)

    if not len(places):
        print("File with objects places is empty")
        exit(0)

    # By default all parameters in UE units. If flag in_meters is set,
    # We recalculate all parameters in meters
    if args.in_meters:
        print("!!! Recalculate in meters !!!")
        print()
        args.uav_height = recalc_dim_ue(args.uav_height)
        args.radius_of_circle = recalc_dim_ue(args.radius_of_circle)
        args.displacement = recalc_dim_ue(args.displacement)

    track_name = args.output

    with open(track_name, "w") as track_file:
        for i in range(args.num_points):
            random_place_idx = random.randint(0, len(places) - 1)

            lux = random.uniform(2.0, 18.0)
            l_rx = -149.0
            l_ry = random.uniform(-30.0, -150.0)
            l_rz = random.uniform(0.0, 360.0)

            cam_pose = calc_cam_pose(height=args.uav_height,
                                     radius=args.radius_of_circle,
                                     displacement=args.displacement,
                                     place=places[random_place_idx])

            print("Coordinates:", cam_pose[0], cam_pose[1], cam_pose[2], cam_pose[3], cam_pose[4], cam_pose[5])
            print("Light settings: intensity --> ", lux, " || angles --> ", l_rx, l_ry, l_rz)
            print()

            track_file.write("%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n" % (cam_pose[0], cam_pose[1], cam_pose[2],  # x_c, y_c, z_c
                                                                  cam_pose[3], cam_pose[4], cam_pose[5],  # x_t, y_t, z_t
                                                                  lux,
                                                                  l_rx, l_ry, l_rz))

    print("TRACK -= CAM AND LIGHT (GOES) =- IS READY")


if __name__ == "__main__":
    main()
