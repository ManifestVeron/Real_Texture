#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
#   Скрипт предназначен для генерации файла размещений объектов в сцене
#

import argparse
import numpy as np


def main():
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument('-o', '--output', default="objects_offsets.csv")
    argument_parser.add_argument('-c', '--places', type=int, default=39)
    argument_parser.add_argument('-p', '--points', type=int, default=240)
    args = argument_parser.parse_args()

    objects_offsets_path = args.output

    count_places = args.places
    count_track_points = args.points

    answer = input("'objects_offsets.csv' is constructed for 'OBZOR'?[y/n]: ")

    if answer not in ('y', 'Y'):
        print("'objects_offsets.csv' cannot be constructed for GOES or for other sensors !!!")
        exit(1)

    # idx
    # place_idx:offsets
    with open(objects_offsets_path, "w") as offsets_file:
        for i in range(count_track_points):
            print("Track point idx #%s" % i)
            places = np.arange(count_places)
            np.random.shuffle(places)
            places = places.tolist()

            for obj_idx, place_idx in enumerate(places):

                delta_xc = 0
                delta_yc = 0
                delta_zc = 0
                angle_ry = 0
                angle_rz = np.random.randint(-5, 5)
                angle_rx = 0

                offsets_file.write("%s:%s,%s,%s,%s,%s,%s" %
                                   (str(place_idx),
                                    str(delta_xc), str(delta_yc), str(delta_zc),
                                    str(angle_ry), str(angle_rz), str(angle_rx)))

                print("Object with id -- %s was placed on place with id -- %s" % (obj_idx, place_idx))

                if obj_idx < len(places) - 1:
                    offsets_file.write(";")

            offsets_file.write("\n")

            print("----")


if __name__ == "__main__":
    main()
