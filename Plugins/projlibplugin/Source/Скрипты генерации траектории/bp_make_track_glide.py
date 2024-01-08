#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import math
import os
import random
import numpy as np

from typing import List, Tuple, Any


def vector_length(v) -> float:
    return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])


def normalize(v):
    l = vector_length(v)
    return [v[0] / l, v[1] / l, v[2] / l]


def recalc_dim_ue(val: float) -> float:
    return 100 * val


def read_places(places_path: str) -> List[Tuple[float, ...]]:
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
    # угол поворота камеры вокруг объекта
    alpha = kwargs.get("alpha")

    # высота
    h = kwargs.get("height")

    # радиус окружности
    r = kwargs.get("radius")

    # смещение центра кадра относительно объекта, зануляется пользователем в начале работы
    d = kwargs.get("displacement")

    # place = (x_place, y_place, z_place)
    place = kwargs.get("place")

    x_cam = r * math.cos(math.radians(alpha)) + place[0]
    y_cam = r * math.sin(math.radians(alpha)) + place[1]
    z_cam = h

    x_t = place[0] + random.randint(-d, d)
    y_t = place[1] + random.randint(-d, d)
    z_t = place[2] + random.randint(-d, d)

    return [x_cam, y_cam, z_cam, x_t, y_t, z_t]


def main() -> None:
    argument_parser = argparse.ArgumentParser()

    # input files
    argument_parser.add_argument('--input', type=str, default="objects_places.csv")

    # specific params
    argument_parser.add_argument('--uav_height', type=float, default=350)
    argument_parser.add_argument('--radius_of_circle', type=float, default=3000)
    argument_parser.add_argument('--num_points', type=int, default=15)
    argument_parser.add_argument('--num_weathers', type=int, default=1)
    argument_parser.add_argument('--stop_distance', type=float, default=300)
    argument_parser.add_argument('--step_count', type=int, default=20)
    argument_parser.add_argument('--displacement', type=float, default=0.0)
    argument_parser.add_argument('--in_meters', type=bool, default=True)

    # light params
    argument_parser.add_argument("--lux_range", type=str, default="[0;10]")
    argument_parser.add_argument("--lrx", type=float, default=0.0)
    argument_parser.add_argument("--lry_range", type=str, default="[0;360]")
    argument_parser.add_argument("--lrz_range", type=str, default="[0;360]")

    # output files
    argument_parser.add_argument('--out_track', type=str, default="glide_track_cam.csv")
    argument_parser.add_argument('--out_env', type=str, default="glide_env.csv")
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
        args.stop_distance = recalc_dim_ue(args.stop_distance)
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

    # Идем по зачитанному input-файлу (файл площадок)
    # place представлена в единицах UE
    for idx_place, place in enumerate(places):

        print(f"PLACE -- {idx_place} : {place}")

        # Стартовая инициализация подсчитывающей погодной карты
        for weather_idx in range(args.num_weathers):
            weathers_count_map[weather_idx + 1] = 0

        list_of_weathers_interest = list(weathers_count_map.keys())

        # Для каждой строки input-файла делаем num_points кадров
        circle_point_idx = 0
        weather_code = 0
        while circle_point_idx < args.num_points:
            # Вычисляем угол для подсчета координаты на окружности на каждую итерацию
            alpha = 90 - (180 // args.num_points) * circle_point_idx

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

            print()
            print(f"Alpha -- {alpha}")

            # Определяем параметры света
            lux = random.uniform(*lux_range)
            l_rx = args.lrx
            l_ry = random.uniform(*lry_range)
            l_rz = random.uniform(*lrz_range)

            # Вычисляем позицию камеры и точку осмотра
            cam_x, cam_y, cam_z, target_x, target_y, target_z = calc_cam_pose(
                alpha=alpha,
                height=args.uav_height,
                radius=args.radius_of_circle,
                displacement=args.displacement,
                place=place
            )

            # Строим траекторию подлета
            step = (args.radius_of_circle - args.stop_distance) / args.step_count
            if step < 0:
                print("Step of glide path < 0")
                exit(-1)

            # Определяем вектор движения
            delta_x = cam_x - target_x
            delta_y = cam_y - target_y
            delta_z = cam_z - target_z

            vec = np.asarray([delta_x, delta_y, delta_z])
            vec = normalize(vec)
            vec[:] = [v * step for v in vec]

            # Определяем углы наведения
            Point_S = np.array((cam_x, cam_y, cam_z))
            Point_F = np.array((target_x, target_y, target_z))

            distance = np.sqrt(np.power(delta_x, 2) + np.power(delta_y, 2))
            dir_vector = Point_F - Point_S

            rx_deg = 0
            ry_deg = (-1) * (np.arctan2(np.abs(delta_z), distance) * 180 / np.pi)
            rz_deg = (np.arctan2(dir_vector[1], dir_vector[0]) * 180 / np.pi)

            # Движемся вдоль вектора движения
            # определенное число раз
            for glide_idx in range(args.step_count):
                cam_x = Point_S[0] - glide_idx * vec[0]
                cam_y = Point_S[1] - glide_idx * vec[1]
                cam_z = Point_S[2] - glide_idx * vec[2]

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
