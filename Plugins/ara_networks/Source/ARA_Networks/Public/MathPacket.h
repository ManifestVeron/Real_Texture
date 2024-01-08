// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ARA_DOUBLE_STRUCT.h"

#include "CoreMinimal.h"
#include "MathPacket.generated.h"

#pragma pack(push, 1)

// Струкура положения БЛА
struct PlanePosition
{
	double latitude;
	double longitude;
	double altitude;
	
	PlanePosition()
	{
		latitude = longitude = altitude = 0.0;
	}
};

// Структура ориентации БЛА
struct PlaneOrientation
{
	float yaw;
	float pitch;
	float roll;
	
	PlaneOrientation()
	{
		yaw = pitch = roll = 0.0;
	}
};

// Структура ориентации камеры ГОЭС относительно строительной оси БЛА
struct CamSighting
{
	float yaw;
	float pitch;
	float roll;

	CamSighting()
	{
		yaw = pitch = roll = 0.0;
	}
};

// Структура описания камеры
struct Camera
{
	PlanePosition 		position;
	PlaneOrientation	orientation;
	CamSighting			cam_sighting;

	Camera():
		position(),
		orientation(),
		cam_sighting()
	{
	}
};

// Структура описания заголовка пакета, принимаемого от математической модели 
struct MathPacketHeader 
{
	char prefix[3];

	MathPacketHeader()
	{
		prefix[0] = static_cast<char>(0xBB);
		prefix[1] = static_cast<char>(0xAA);
		prefix[2] = static_cast<char>(0xCC);
	}
};

// Структура описания тела пакета, принимаемого от математической модели 
struct MathPacketBody
{
	unsigned char modelID;
	Camera 	camera;

	MathPacketBody():
		modelID(0),
		camera()
	{}
};

// Структура описания пакета, принимаемого от математической модели 
struct MathPacket
{
	MathPacketHeader header;
	MathPacketBody body;

	MathPacket():
		header(),
		body()
	{}
};

#pragma pack(pop)


// Структура для представления пакета в BP

USTRUCT(BluePrintType)
struct FMathPacket
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 modelID;

	// BLA Position
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT latitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT longitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT altitude;
	
	// BLA Orientation

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT yaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT roll;
	
	// GOES Cam Sighting
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT goes_yaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT goes_pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT goes_roll;

	FMathPacket()
	{
		modelID = 0;
	}

	FMathPacket(const MathPacket& math_packet)
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
	}

	FMathPacket& operator= (MathPacket& math_packet);
};
