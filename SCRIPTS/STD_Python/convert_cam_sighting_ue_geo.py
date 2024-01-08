#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import pyproj
import os


class Transformer(object):
    def __init__(self, src_crs: int, dst_crs: int, lat0: float, lon0: float, z_fix: float = 195.0):
        self.__geo2utm = pyproj.Transformer.from_crs(src_crs, dst_crs)
        self.__utm2geo = pyproj.Transformer.from_crs(dst_crs, src_crs)

        self.__lat0 = lat0
        self.__lon0 = lon0
        self.__z_fix = z_fix

    def ue2geo(self, x: float, y: float, z: float = 0):
        x0, y0 = self.__geo2utm.transform(self.__lat0, self.__lon0)

        x_r = x / 100.0
        y_r = -y / 100.0

        r_lat, r_lon = self.__utm2geo.transform((x0 + x_r), (y0 + y_r))
        r_alt = (z / 100.0) + self.__z_fix

        return [r_lat, r_lon, r_alt]

    def geo2ue(self, v_lat: float, v_lon: float, v_alt: float):
        x0, y0 = self.__geo2utm.transform(self.__lat0, self.__lon0)
        x_r, y_r = self.__geo2utm.transform(v_lat, v_lon)

        x = 100.0 * (x_r - x0)
        y = -100.0 * (y_r - y0)
        z = 100 * (v_alt - self.__z_fix)

        return [x, y, z]


if __name__ == "__main__":

    arg = argparse.ArgumentParser()
    arg.add_argument("--track", type=str)
    arg.add_argument("--src_crs", type=int)
    arg.add_argument("--dst_crs", type=int)
    arg.add_argument("--lat0", type=float)
    arg.add_argument("--lon0", type=float)
    arg.add_argument("--z_fix", type=float)
    args = arg.parse_args()

    transformer = Transformer(args.src_crs, args.dst_crs, args.lat0, args.lon0, args.z_fix)

    if not os.path.exists(args.track):
        print(f"File {args.track} does not exists")
        exit(-1)

    dir_path, out_name = os.path.split(os.path.abspath(args.track))
    out_name = out_name.split(".")[0] + "_geo.csv"
    out_path = os.path.join(dir_path, out_name)

    with open(out_path, "w") as output_file:
        with open(args.track, "r") as track_file:
            for line in track_file:
                raw_data = line.split(";")
                
                coords = raw_data[0:3]
                del raw_data[0:3]
                angles = raw_data[0:]
                del raw_data[0:]
                
                coords = list(map(float, coords))
                coords = transformer.ue2geo(*coords)
                
                raw_data = raw_data + list(map(str, coords)) + angles
                raw_data = ";".join(raw_data)
                output_file.write(f"{raw_data}")

    print("Done!")
