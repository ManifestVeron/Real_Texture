#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import random
import argparse


def main():
    argument_parser = argparse.ArgumentParser()

    # Параметры зоны полета БЛА
    argument_parser.add_argument("--x_range", type=str, default="[0;100]")
    argument_parser.add_argument("--y_range", type=str, default="[0;100]")
    argument_parser.add_argument("--uav_height", type=float, default=1000.0)

    # Количественные параметры
    argument_parser.add_argument('--num_points', type=int, default=30)
    argument_parser.add_argument('--num_weathers', type=int, default=6)

    # Параметры настройки света
    argument_parser.add_argument("--lux_range", type=str, default="[0;10]")
    argument_parser.add_argument("--lrx", type=float, default=0.0)
    argument_parser.add_argument("--lry_range", type=str, default="[0;360]")
    argument_parser.add_argument("--lrz_range", type=str, default="[0;360]")

    argument_parser.add_argument('--out_track', default="obzor_track_cam.csv")
    argument_parser.add_argument('--out_env', default="obzor_env.csv")

    args = argument_parser.parse_args()

    # Парсинг параметров зоны полета БЛА
    x_range = list(map(float, args.x_range[1:-1].split(";")))
    y_range = list(map(float, args.y_range[1:-1].split(";")))

    # Парсинг параметров настройки света
    lux_range = list(map(float, args.lux_range[1:-1].split(";")))

    lry_range = list(map(float, args.lry_range[1:-1].split(";")))
    lrz_range = list(map(float, args.lrz_range[1:-1].split(";")))

    # Open output files
    track_file = open(args.out_track, "w")
    env_file = open(args.out_env, "w")

    # Write headers to output files
    track_file.write("xc;yc;zc;rx;ry;rz\n")
    env_file.write("weather_code;lux;l_rx;l_ry;l_rz\n")

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

    # Params calculation
    idx = 0
    weather_code = 0
    while idx < args.num_points:

        # Позиция БЛА
        xc = random.uniform(*x_range)
        yc = random.uniform(*y_range)
        zc = args.uav_height
        rx = 0.0
        ry = 270.0
        rz = random.uniform(0.0, 360.0)

        # Параметры источника света
        lux = random.uniform(*lux_range)
        l_rx = args.lrx
        l_ry = random.uniform(*lry_range)
        l_rz = random.uniform(*lrz_range)

        # Параметры погоды
        try:
            weather_code = random.choice(list_of_weathers_interest)
            weathers_count_map[weather_code] += 1

            if weathers_count_map[weather_code] > weather_limit:
                weathers_count_map.pop(weather_code)
                list_of_weathers_interest = list(weathers_count_map.keys())
                continue
        except IndexError:
            print("Will used last weather code")

        print("Coordinates:", xc, yc, zc, rx, ry, rz)
        print("Light settings: intensity --> ", lux, " || angles --> ", l_rx, l_ry, l_rz)
        print()
        track_file.write(f"{xc};{yc};{zc};{rx};{ry};{rz}\n")
        env_file.write(f"{weather_code};{lux};{l_rx};{l_ry};{l_rz}\n")

        idx += 1

    track_file.close()
    env_file.close()

    print("TRACK -= CAM AND LIGHT (OBZOR) =- IS READY")


if __name__ == "__main__":
    main()
