#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import math
import os
import random
import numpy as np


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

    x_cam = r * math.cos(math.radians(alpha)) + place[0]
    y_cam = r * math.sin(math.radians(alpha)) + place[1]
    z_cam = h

    x_t = place[0] + random.randint(-d, d)
    y_t = place[1] + random.randint(-d, d)
    z_t = place[2] + random.randint(-d, d)

    return [x_cam, y_cam, z_cam, x_t, y_t, z_t]


def main():
    argument_parser = argparse.ArgumentParser()

    # input files
    argument_parser.add_argument('--input', type=str, default="objects_places.csv")

    # specific params
    argument_parser.add_argument('--uav_height', type=float, default=180)
    argument_parser.add_argument('--radius_of_circle', type=float, default=100)
    argument_parser.add_argument('--num_points', type=int, default=240)
    argument_parser.add_argument('--num_weathers', type=int, default=6)
    argument_parser.add_argument('--displacement', type=float, default=80)
    argument_parser.add_argument('--in_meters', type=bool, default=False)

    # light params
    argument_parser.add_argument("--lux_range", type=str, default="[0;10]")
    argument_parser.add_argument("--lrx", type=float, default=0.0)
    argument_parser.add_argument("--lry_range", type=str, default="[0;360]")
    argument_parser.add_argument("--lrz_range", type=str, default="[0;360]")

    # output files
    argument_parser.add_argument('--out_track', type=str, default="goes_track_cam.csv")
    argument_parser.add_argument('--out_env', type=str, default="goes_env.csv")
    argument_parser.add_argument('--out_cam_sighting', type=str, default='cam_sighting.csv')

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

    # light params parsing
    lux_range = list(map(float, args.lux_range[1:-1].split(";")))

    lry_range = list(map(float, args.lry_range[1:-1].split(";")))
    lrz_range = list(map(float, args.lrz_range[1:-1].split(";")))

    # Open output files
    track_file = open(args.out_track, "w")
    env_file = open(args.out_env, "w")
    cam_sighting_file = open(args.out_cam_sighting, "w")

    # Write headers to output files
    track_file.write("xc;yc;zc;rx;ry;rz\n")
    env_file.write("weather_code;lux;l_rx;l_ry;l_rz\n")
    cam_sighting_file.write("x_t;y_t;z_t;rx;ry;rz\n")

    # Predefine weathers limit
    weather_limit = args.num_points // args.num_weathers
    weathers_count_map = dict()

    if weather_limit == 0:
        print("Weather Limit == 0")
        print("Some of params predefined not correct")
        print("Abort task")
        exit(-1)

    print(f"weather limit = {weather_limit}")
    print()

    for weather_idx in range(args.num_weathers):
        weathers_count_map[weather_idx+1] = 0

    list_of_weathers_interest = list(weathers_count_map.keys())

    # Задаем позицию и ориентацию камеры для режима наведения
    circle_point_idx = 0
    weather_code = 0
    while circle_point_idx < args.num_points:
        # Рандомно выбираем площадку объекта
        random_place_idx = random.randint(0, len(places) - 1)

        # Определяем параметры погоды
        try:
            weather_code = random.choice(list_of_weathers_interest)
            weathers_count_map[weather_code] += 1

            if weathers_count_map[weather_code] > weather_limit:
                weathers_count_map.pop(weather_code)
                list_of_weathers_interest = list(weathers_count_map.keys())
                continue
        except IndexError:
            print("Will used last weather code")

        # Определяем параметры света
        lux = random.uniform(*lux_range)
        l_rx = args.lrx
        l_ry = random.uniform(*lry_range)
        l_rz = random.uniform(*lrz_range)

        # Вычисляем позицию камеры и точку осмотра
        cam_x, cam_y, cam_z, target_x, target_y, target_z = calc_cam_pose(
            height=args.uav_height,
            radius=args.radius_of_circle,
            displacement=args.displacement,
            place=places[random_place_idx])

        # Определяем углы наведения
        Point_S = np.array((cam_x, cam_y, cam_z))
        Point_F = np.array((target_x, target_y, target_z))

        dist = np.sqrt(np.power(Point_F[0] - Point_S[0], 2)
                       + np.power(Point_F[1] - Point_S[1], 2))

        dh = np.abs(Point_F[2] - Point_S[2])

        dir_vector = Point_F - Point_S
        rz_p = np.arctan2(dir_vector[1], dir_vector[0])

        rx_deg = 0
        ry_deg = (-1) * (np.arctan2(dh, dist) * 180 / np.pi)
        rz_deg = (rz_p * 180 / np.pi)


        print("Coordinates:", cam_x, cam_y, cam_z, rx_deg, ry_deg, rz_deg)
        print("Light settings: intensity --> ", lux, " || angles --> ", l_rx, l_ry, l_rz)
        print()

        track_file.write(f"{cam_x};{cam_y};{cam_z};{rx_deg};{ry_deg};{rz_deg}\n")
        env_file.write(f"{weather_code};{lux};{l_rx};{l_ry};{l_rz}\n")
        cam_sighting_file.write(f"{target_x};{target_y};{target_z};{rx_deg};{ry_deg};{rz_deg}\n")

        circle_point_idx += 1

    cam_sighting_file.close()
    env_file.close()
    track_file.close()

    print("TRACK -= CAM AND LIGHT (GOES) =- IS READY")


if __name__ == "__main__":
    main()
