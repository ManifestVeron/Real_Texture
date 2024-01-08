#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import json
import math
import os
import random
import numpy as np


def vector_length(v) -> float:
    """
    Вычисляем длину вектора
    :param v:
    :return:
    """
    return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])


def normalize(v):
    """
    Вычисляем нормализованный вектор
    :param v:
    :return:
    """
    l = vector_length(v)
    return [v[0] / l, v[1] / l, v[2] / l]


def recalc_dim_ue(val: float):
    return 100*val


def read_places(places_path: str):
    if not os.path.exists(places_path):
        print("File with objects places '%s' does not exists" % places_path)
        print("You have to build this file")
        exit(1)

    places_list = dict()
    with open(places_path, "r") as places_file:
        for raw in places_file:
            separated_raw = raw.split(';')
            idx = int(separated_raw[0])
            places_list[idx] = tuple(map(float, separated_raw[1:4]))  # we use only (x, y, z)

    return places_list


def get_objects_from_markup(markup_path: str):
    with open(markup_path) as markup_file:
        markup = json.load(markup_file)

    objects_in_markup = list()
    for idx in range(len(markup)):
        objects_in_markup.append(int(markup[idx]['object_id']))

    return objects_in_markup


def get_places_from_offsets(raw_str: str):
    places_list = list()
    if len(raw_str):
        raw = raw_str.split(';')
        places_list = list(map(lambda x: int(x.split(':')[0]), raw))

    return places_list


def calc_cam_pose(**kwargs):
    h = kwargs.get("height")
    r = kwargs.get("radius")
    d = kwargs.get("displacement")
    place = kwargs.get("place")     # place = (x_place, y_place, z_place)

    alpha = random.uniform(-180, 180)

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
    argument_parser.add_argument('--places', type=str, default="objects_places.csv")
    argument_parser.add_argument('--markups_dir', type=str, default="JSON")
    argument_parser.add_argument('--obzor_track', type=str, default="track_obzor.csv")
    argument_parser.add_argument('--obzor_env', type=str, default="env_obzor.csv")
    argument_parser.add_argument('--offsets', type=str, default="objects_offsets.csv")

    # specific params
    argument_parser.add_argument('--num_points', type=int, default=60)
    argument_parser.add_argument('--radius_of_circle', type=float, default=2)
    argument_parser.add_argument('--stop_distance', type=float, default=100)
    argument_parser.add_argument('--step_count', type=int, default=20)
    argument_parser.add_argument('--displacement', type=float, default=0.5)
    argument_parser.add_argument('--in_meters', type=bool, default=True)
    argument_parser.add_argument('--verbose', type=bool, default=False)

    # output files
    argument_parser.add_argument('--out_track_glide', type=str, default="track_glide_with_bind.csv")
    argument_parser.add_argument('--out_env_glide', type=str, default="env_glide_with_bind.csv")
    argument_parser.add_argument('--out_pairs', type=str, default="pairs_glide.csv")
    argument_parser.add_argument('--out_cam_sighting', type=str, default='cam_sighting_glide_with_bind.csv')

    args = argument_parser.parse_args()

    if not os.path.exists(args.obzor_track):
        print("Obzor track '%s' does not exists" % args.obzor_track)
        print("You have to build this file")
        exit(1)

    if not os.path.exists(args.offsets):
        print("File with objects offsets '%s' does not exists" % args.offsets)
        print("You have to build this file")
        exit(1)

    if not os.path.exists(args.markups_dir):
        print("Directory with markups '%s' does not exists" % args.markups_dir)
        print("Check this !!!")
        exit(1)

    places = read_places(args.places)

    if not len(places):
        print("File with objects places '%s' is empty" % args.places)
        exit(1)

    # By default all parameters in UE units. If flag in_meters is set,
    # We recalculate all parameters in meters
    if args.in_meters:
        print("!!! Recalculate in meters !!!")
        print()
        args.radius_of_circle = recalc_dim_ue(args.radius_of_circle)
        args.stop_distance = recalc_dim_ue(args.stop_distance)
        args.displacement = recalc_dim_ue(args.displacement)

    # Open Input files
    offsets_file = open(args.offsets, "r")
    obzor_track_file = open(args.obzor_track, "r")
    obzor_env_file = open(args.obzor_env, "r")

    # Read headers of input files
    obzor_track_file.readline()
    obzor_env_file.readline()

    # Open output files
    track_file = open(args.out_track_glide, "w")
    env_file = open(args.out_env_glide, "w")
    cam_sighting_file = open(args.out_cam_sighting, "w")
    pairs_file = open(args.out_pairs, "w")

    # Write headers to output files
    track_file.write("xc;yc;zc;rx;ry;rz\n")
    env_file.write("weather_code;lux;l_rx;l_ry;l_rz\n")
    cam_sighting_file.write("x_t;y_t;z_t;rx;ry;rz\n")

    # Идем по трек-файлу обзорного датчика
    # Линейные координаты представлены в единицах UE
    for idx, track_raw in enumerate(obzor_track_file):
        print(">")
        print(f"[POSITION IDX IN OBZOR TRACK FILE --> {idx}]")
        track_raw = track_raw.split(';')
        env_raw = obzor_env_file.readline()
        env_raw = env_raw.split(';')

        offsets_raw = offsets_file.readline()
        places_in_offsets = get_places_from_offsets(offsets_raw)

        if args.verbose:
            print("offsets raw -> ", offsets_raw)
            print("places in offsets -> ", places_in_offsets)

        # Находим высоту обзорного датчика
        height = float(track_raw[2])

        # Находим параметры окружающей среды
        weather_code = int(env_raw[0])
        lux, l_rx, l_ry, l_rz = tuple(map(float, env_raw[1:]))

        # Находим объекты, размещенные на снимке обзорного датчика
        # По файлу с разметкой
        markup_name = os.path.join(args.markups_dir, str(idx) + ".json")
        try:
            objects_in_markup = get_objects_from_markup(markup_name)
        except FileNotFoundError:
            print(f"Markup {markup_name} not found")
            continue

        print(f"[OPENED MARKUP --> {markup_name}]")
        print("objects in markup --> ", objects_in_markup)

        # Проходим по объектам, попавшим в кадр обзорного датчика
        print("----")
        for object_idx in objects_in_markup:
            try:
                place_idx = places_in_offsets[object_idx]
            except IndexError:
                if args.verbose:
                    print("WE HAVE NOT SUCH OBJECT IN OFFSETS [IDX]: ", object_idx)
                    print()
                continue

            try:
                object_place = places[place_idx]
            except IndexError:
                if args.verbose:
                    print("WE HAVE NOT SUCH PLACE IDX IN PLACES: ", place_idx)
                    print()
                continue

            # Строим траекторию подлета
            for circle_point_idx in range(args.num_points):

                # Вычисляем позицию камеры и точку осмотра
                cam_x, cam_y, cam_z, target_x, target_y, target_z = calc_cam_pose(
                    height=height,
                    radius=args.radius_of_circle,
                    displacement=args.displacement,
                    place=object_place)

                # Определяем шаг подлета
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

                if args.verbose:
                    print(f"[OBJECT] WITH ID --> {object_idx} in [PLACE] WITH ID --> {place_idx}")

                # Движемся вдоль вектора движения
                # определенное число раз
                for glide_idx in range(args.step_count):

                    cam_x = Point_S[0] - glide_idx * vec[0]
                    cam_y = Point_S[1] - glide_idx * vec[1]
                    cam_z = Point_S[2] - glide_idx * vec[2]

                    if args.verbose:
                        print("Coordinates:", cam_x, cam_y, cam_z, rx_deg, ry_deg, rz_deg)
                        print("Light settings: intensity --> ", lux, " || angles --> ", l_rx, l_ry, l_rz)
                        print()

                    track_file.write(f"{idx};{cam_x};{cam_y};{cam_z};{rx_deg};{ry_deg};{rz_deg}\n")
                    env_file.write(f"{weather_code};{lux};{l_rx};{l_ry};{l_rz}\n")
                    cam_sighting_file.write(f"{target_x};{target_y};{target_z};{rx_deg};{ry_deg};{rz_deg}\n")
                    pairs_file.write(f"{idx};{object_idx}\n")

    obzor_env_file.close()
    obzor_track_file.close()
    offsets_file.close()

    pairs_file.close()
    cam_sighting_file.close()
    env_file.close()
    track_file.close()

    print("TRACK -= CAM AND LIGHT (GOES) =- IS READY")


if __name__ == "__main__":
    main()
