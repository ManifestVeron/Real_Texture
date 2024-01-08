#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import struct
import time
import os
import argparse
import copy


class UDPClient(object):
    '''
    Класс UDP-лиента для UE4 для передачи всех полезных данных
    '''
    def __init__(self, ip: str = "127.0.0.1", port: int = 9898, verbose: bool = True, timeout: float = 1.0):
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.__addr = (ip, port)
        self.__answer_size = 3
        self.__verbose = verbose
        self.__timeout = timeout
    
    def __set_cam_pose(self, index, *raw_cam_pose):
        sent_status = False
        answer = None

        cam_pose = struct.pack('idddddd', index, *raw_cam_pose)
        self.__sock.sendto(cam_pose, self.__addr)
        try:
            answer = self.__sock.recvfrom(self.__answer_size)[0].decode()

            if self.__verbose:
                print("--> CAM POSE [ANSWER] = ", answer)

        except ConnectionResetError:
            print("WE HAVE NO ANSWER FROM SERVER (CAM POSE)")

        if answer == "OK":
            sent_status = True

        return sent_status

    def __set_lighting_data(self, index, *raw_lighting_data):
        sent_status = False
        answer = None

        lighting_data = struct.pack('idddd', index, *raw_lighting_data)

        self.__sock.sendto(lighting_data, self.__addr)

        try:
            answer = self.__sock.recvfrom(self.__answer_size)[0].decode()

            if self.__verbose:
                print("--> LIGHTING DATA [ANSWER] = ", answer)

        except ConnectionResetError:
            print("WE HAVE NO ANSWER FROM SERVER (LIGHTING DATA)")

        if answer == "OK":
            sent_status = True

        return sent_status

    def __set_objects_num(self, index, num):
        sent_status = False
        answer = None

        objects_num = struct.pack('ii', index, num)

        self.__sock.sendto(objects_num, self.__addr)

        try:
            answer = self.__sock.recvfrom(self.__answer_size)[0].decode()

            if self.__verbose:
                print("--> OBJECTS NUMBER [ANSWER] = ", answer)

        except ConnectionResetError:
            print("WE HAVE NO ANSWER FROM SERVER (OBJECTS NUM)")

        if answer == "OK":
            sent_status = True

        return sent_status

    def __set_object_pose(self, index, *raw_object_pose):
        sent_status = False
        answer = None

        object_pose = struct.pack('iidddddd', index, *raw_object_pose)

        self.__sock.sendto(object_pose, self.__addr)

        try:
            answer = self.__sock.recvfrom(self.__answer_size)[0].decode()
            if self.__verbose:
                print("--> OBJECT POSE [ANSWER] = ", answer)
        except ConnectionResetError:
            print("WE HAVE NO ANSWER FROM SERVER (OBJECT POSE)")

        if answer == "OK":
            sent_status = True

        return sent_status

    def __set_data(self, index, cam_pose, light, obj_poses, place_indexes):
        time.sleep(self.__timeout)
        status = self.__set_cam_pose(index, *cam_pose)
        if not status:
            print("Not sent camera pose")
            status = self.__set_data(index, cam_pose, light, obj_poses, place_indexes)
            return status

        status = self.__set_lighting_data(index, *light)
        if not status:
            print("Not sent lighting data")
            status = self.__set_data(index, cam_pose, light, obj_poses, place_indexes)
            return status

        num_poses = len(obj_poses)

        if self.__verbose:
            print("NUMBER OF OBJECT POSES --> ", num_poses)

        status = self.__set_objects_num(index, num_poses)
        if not status:
            print("Not sent objects num")
            status = self.__set_data(index, cam_pose, light, obj_poses, place_indexes)
            return status

        for obj_idx, place_idx in enumerate(place_indexes):
            if self.__verbose:
                print("Packet idx --> ", index)
                print("Object idx --> ", obj_idx)
                print("Place idx (1) --> ", place_idx)
                print("Place idx (2) --> ", obj_poses[place_idx][0])
                print("Object pose (raw) --> ", *obj_poses[place_idx])
                print("----")

            # отправлять по индексу из obj_indexes
            status = self.__set_object_pose(index, *obj_poses[place_idx])

            if not status:
                print("Not sent object pose #%d" % index)
                status = self.__set_data(index, cam_pose, light, obj_poses, place_indexes)
                break

        return status

    def send_data(self, index, cam_pose, light, obj_poses, obj_indexes):
        status = self.__set_data(index, cam_pose, light, obj_poses, obj_indexes)
        if not status:
            status = self.__set_data(index, cam_pose, light, obj_poses, obj_indexes)
            return status
        return status

    def __del__(self):
        self.__sock.close()
        print("Client socket closed")


# Общие полезные функции

def read_cam_and_light_track(track_path: str):
    track = list()

    if not os.path.exists(track_path):
        print("File with camera and light track '%s' does not exists" % track_path)
        print("You have to build this file")
        exit(1)

    with open(track_path, "r") as track_file:
        for row in track_file:
            row_data = row.split(";")
            track.append(tuple(map(float, row_data)))

    return track


def thin_out_cam_and_light_track(idxs_path: str, track_list: list) -> dict:
    idxs_list = list()
    new_track_list = dict()

    if not os.path.exists(idxs_path):
        print("File with idxs does not exists")
        exit(1)

    with open(idxs_path, "r") as idxs_file:
        for row in idxs_file:
            idxs_list.append(int(row))

    for i, track_point in enumerate(track_list):
        if i in idxs_list:
            new_track_list[i] = track_point

    return new_track_list


def read_places(places_path: str):
    places = list()

    if not os.path.exists(places_path):
        print("File with objects places '%s' does not exists" % places_path)
        print("You have to build this file")
        exit(2)

    with open(places_path, "r") as places_file:
        for row in places_file:
            row_data = row.split(";")
            place_item = list(map(float, row_data))
            place_item[0] = int(place_item[0])
            places.append(place_item)

    return places


def read_offsets(offsets_path: str):
    offsets = list()

    if not os.path.exists(offsets_path):
        print("File with places indexes and displacements '%s' does not exists" % offsets_path)
        print("You have to build this file")
        exit(3)

    with open(offsets_path, "r") as offsets_file:
        for row in offsets_file:
            row_data = row.split(";")
            offset = {}

            for offset_row in row_data:
                place_idx, obj_offsets = offset_row.split(":")
                place_idx = int(place_idx)
                obj_offsets = obj_offsets.split(",")
                obj_offsets = list(map(float, obj_offsets))
                offset[place_idx] = obj_offsets
            offsets.append(offset)

    return offsets


def main():
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument('--track', type=str, default=os.path.join(".", "track_cam_light.csv"))
    argument_parser.add_argument('--places', type=str, default=os.path.join(".", "objects_places.csv"))
    argument_parser.add_argument('--offsets', type=str, default=os.path.join(".", "objects_offsets.csv"))
    argument_parser.add_argument('--idx_start', type=str, default="0")
    argument_parser.add_argument('--mode', type=str, default="track")
    argument_parser.add_argument('--verbose', type=bool, default=False)
    args = argument_parser.parse_args()

    cam_and_light_track_path = args.track
    objects_places_path = args.places
    objects_offsets_path = args.offsets

    udp_client = UDPClient(verbose=args.verbose)

    object_places = read_places(objects_places_path)
    object_offsets = read_offsets(objects_offsets_path)
    cam_and_light_track = read_cam_and_light_track(cam_and_light_track_path)

    idx_start = 0

    if args.mode == "track":
        idx_start = int(args.idx_start)
    elif args.mode == "sequence":
        cam_and_light_track = thin_out_cam_and_light_track(args.idx_start, cam_and_light_track)
    else:
        print("Unknown mode!")
        exit(1)

    for i, cam_and_light_item in enumerate(cam_and_light_track):

        if i < idx_start:
            continue

        if args.mode == "sequence":
            i = cam_and_light_item
            cam_and_light_item = cam_and_light_track[i]

        camera_coords = cam_and_light_item[:6]
        lighting_data = cam_and_light_item[6:]

        print("== [PACKAGE START] == ")
        print("DATA PACKAGE [idx -- %s]" % i)
        print()
        print("> Camera coordinates:", camera_coords)
        print("> Light settings: ", lighting_data)
        print("> Number of places:", len(object_places))

        places_offsets_item = dict()
        try:
            places_offsets_item = object_offsets[i]
        except IndexError:
            print("Ended objects offsets list")
            exit(0)

        curr_object_places = copy.deepcopy(object_places)

        for place_idx, obj_offset in places_offsets_item.items():

            curr_object_places[place_idx][1] += obj_offset[0]    # delta_xc
            curr_object_places[place_idx][2] += obj_offset[1]    # delta_yc
            curr_object_places[place_idx][3] += obj_offset[2]    # delta_zc
            curr_object_places[place_idx][4] += obj_offset[3]    # angle_ry
            curr_object_places[place_idx][5] += obj_offset[4]    # angle_rz
            curr_object_places[place_idx][6] += obj_offset[5]    # angle_rx

        places_idx = list(places_offsets_item.keys())

        udp_client.send_data(i, camera_coords, lighting_data, curr_object_places, places_idx)

        print()
        print("DATA PACKAGE [idx -- %s] SENT" % i)
        print("== [PACKAGE END] ==")
        print()


if __name__ == "__main__":
    main()
