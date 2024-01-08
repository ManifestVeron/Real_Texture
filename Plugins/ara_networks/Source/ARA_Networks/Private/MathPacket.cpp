// Fill out your copyright notice in the Description page of Project Settings.

#include "MathPacket.h"

FMathPacket& FMathPacket::operator= (MathPacket& math_packet)
{
    modelID = math_packet.body.modelID;

    latitude.make_doubleStruct(math_packet.body.camera.position.latitude);
    longitude.make_doubleStruct(math_packet.body.camera.position.longitude);
    altitude.make_doubleStruct(math_packet.body.camera.position.altitude);

    yaw.make_doubleStruct(math_packet.body.camera.orientation.yaw);
    pitch.make_doubleStruct(math_packet.body.camera.orientation.pitch);
    roll.make_doubleStruct(math_packet.body.camera.orientation.roll);

    goes_yaw.make_doubleStruct(math_packet.body.camera.cam_sighting.yaw);
	goes_pitch.make_doubleStruct(math_packet.body.camera.cam_sighting.pitch);
	goes_roll.make_doubleStruct(math_packet.body.camera.cam_sighting.roll);

    return *this;
}