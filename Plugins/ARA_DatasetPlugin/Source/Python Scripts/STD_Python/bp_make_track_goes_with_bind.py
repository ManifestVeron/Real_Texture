#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import json
import math
import os
import random


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
    raw = raw_str.split(';')
    return list(map(lambda x: int(x.split(':')[0]), raw))


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
    argument_parser.add_argument('--places', type=str, default="objects_places.csv")
    argument_parser.add_argument('--markups_dir', type=str, default=" ")
    argument_parser.add_argument('--obzor_track', type=str, default="track_cam_light.csv")
    argument_parser.add_argument('--offsets', type=str, default="objects_offsets.csv")

    argument_parser.add_argument('--track_goes', type=str, default="track_cam_light_goes_with_bind.csv")
    argument_parser.add_argument('--pairs', type=str, default="pairs.csv")
    argument_parser.add_argument('-r', '--radius_of_circle', type=float, default=2)
    argument_parser.add_argument('-d', '--displacement', type=float, default=0.5)
    argument_parser.add_argument('-m', '--in_meters', type=bool, default=True)
    argument_parser.add_argument('--verbose', type=bool, default=False)

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
        args.displacement = recalc_dim_ue(args.displacement)


    with open(args.track_goes, "w") as track_file:
        with open(args.pairs, "w") as pairs_file:
            with open(args.offsets, "r") as offsets_file:
                with open(args.obzor_track, "r") as obzor_track_file:

                    for idx, track_raw in enumerate(obzor_track_file):
                        print(">")
                        print("[POSITION IDX IN OBZOR TRACK FILE --> %d]" % idx)
                        track_raw = track_raw.split(';')

                        offsets_raw = offsets_file.readline()
                        places_in_offsets = get_places_from_offsets(offsets_raw)

                        if args.verbose:
                            print("offsets raw -> ", offsets_raw)
                            print("places in offsets -> ", places_in_offsets)

                        height = float(track_raw[2])

                        # if args.in_meters:
                        #    height = recalc_dim_ue(height)

                        lux, l_rx, l_ry, l_rz = tuple(map(float, track_raw[6:]))

                        markup_name = os.path.join(args.markups_dir, str(idx) + ".json")

                        try:
                            objects_in_markup = get_objects_from_markup(markup_name)
                        except FileNotFoundError:
                            print("Markup %s not found" % markup_name)
                            continue

                        print("[OPENED MARKUP --> %s]" % markup_name)
                        print("objects in markup --> ", objects_in_markup)


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

                            cam_pose = calc_cam_pose(height=height,
                                                     radius=args.radius_of_circle,
                                                     displacement=args.displacement,
                                                     place=object_place)

                            if args.verbose:
                                print("[OBJECT] WITH ID --> %d in [PLACE] WITH ID --> %d" % (object_idx, place_idx))
                                print("Coordinates:", cam_pose[0], cam_pose[1], cam_pose[2], cam_pose[3], cam_pose[4], cam_pose[5])
                                print("Light settings: intensity --> ", lux, " || angles --> ", l_rx, l_ry, l_rz)
                                print()

                            track_file.write(
                                "%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n" % (idx,
                                                                        cam_pose[0], cam_pose[1], cam_pose[2],  # x_c, y_c, z_c
                                                                        cam_pose[3], cam_pose[4], cam_pose[5],  # x_t, y_t, z_t
                                                                        lux,
                                                                        l_rx, l_ry, l_rz))
                            pairs_file.write("%d;%d\n" % (idx, object_idx))

    print("TRACK -= CAM AND LIGHT (GOES) =- IS READY")


if __name__ == "__main__":
    main()
