#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
#   Скрипт предназначен для рандомного сокрытия объектов по существующему файлу
#   размещения объектов - objects_offsets.csv
#

import argparse
import random

def randomise_objects_hiding(objects_count: int) -> list:
    """
    Prepare list with actual idxs
    :param objects_count:
    :return:
    """
    a: int = random.randint(0, objects_count-1)
    b: int = random.randint(0, objects_count-1)

    if a > b:
        a, b = b, a

    if a == b:
        b += 1

    modulo: int = 1 + random.randint(a, b)
    control_value: int = random.randint(0, (b-a))

    actual_idxs = list()

    for idx in range(objects_count):
        if idx % modulo >= control_value:
            actual_idxs.append(idx)

    return actual_idxs


if __name__ == "__main__":
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument('-i', '--in_file', type=str, default="objects_offsets.csv")
    argument_parser.add_argument('-o', '--out_file', type=str, default="objects_offsets_out.csv")
    args = argument_parser.parse_args()

    try:
        with open(args.in_file, "r") as in_offsets_file:
            with open(args.out_file, "w") as out_offsets_file:
                for raw_data in in_offsets_file:
                    offsets_row = raw_data.split(";")

                    actual_idxs = randomise_objects_hiding(len(offsets_row))
                    for i, offset_data in enumerate(offsets_row):
                        place_idx, offset_vector = offset_data.split(":")
                        if int(place_idx) in actual_idxs:
                            place_idx = str(-1)
                        out_offsets_file.write(place_idx + ":" + offset_vector)
                        if i < len(offsets_row) - 1:
                            out_offsets_file.write(";")

    except FileNotFoundError:
        print(f"File {args.in_file} not found!")
        print("Program terminated")
        exit(1)
