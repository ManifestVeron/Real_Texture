// Fill out your copyright notice in the Description page of Project Settings.


#include "InnerCoordinatesConvertion.h"


void UInnerCoordinatesConvertion::Orlan_GEO_UE
(
    FString src_crs,
    FString dst_crs,
    FString lat0,
    FString lon0,
    FString z_fix,
    FMathPacket math_packet,
    float& X,
    float& Y,
    float& Z,
    float& YAW, 
    float& PITCH,
    float& ROLL 
)
{
    UCoordsTransformersLibrary::GEO_UE_Transformer
    (
        src_crs, //"EPSG:4326", 
        dst_crs, //"EPSG:32638",
        FGEO_DOUBLE_STRUCT(lat0),//"55.000000"),
        FGEO_DOUBLE_STRUCT(lon0),//"37.000000"),
        UInnerDataTypesConversion::INNER_CONVERT_ARA_GEO(math_packet.latitude),
        UInnerDataTypesConversion::INNER_CONVERT_ARA_GEO(math_packet.longitude),
        UInnerDataTypesConversion::INNER_CONVERT_ARA_GEO(math_packet.altitude),
        FGEO_DOUBLE_STRUCT(z_fix),//184.0),
        X,
        Y,
        Z
    );

    YAW = UCommonOperations::ARA_DOUBLE_STRUCT_As_Float(math_packet.yaw);
    PITCH = UCommonOperations::ARA_DOUBLE_STRUCT_As_Float(math_packet.pitch);
    ROLL = UCommonOperations::ARA_DOUBLE_STRUCT_As_Float(math_packet.roll);
}

void UInnerCoordinatesConvertion::Target_GEO_UE
(
    FString src_crs,
    FString dst_crs,
    FString lat0,
    FString lon0,
    FString z_fix,
    FTarget target_packet,
    uint8& class_id,
    float& X,
    float& Y,
    float& Z,
    float& YAW, 
    float& PITCH,
    float& ROLL 
)
{

    class_id = target_packet.class_id;

    UCoordsTransformersLibrary::GEO_UE_Transformer
    (
        src_crs, //"EPSG:4326", 
        dst_crs, //"EPSG:32638",
        FGEO_DOUBLE_STRUCT(lat0),//"55.000000"),
        FGEO_DOUBLE_STRUCT(lon0),//"37.000000"),
        UInnerDataTypesConversion::INNER_CONVERT_ARA_GEO(target_packet.latitude),
        UInnerDataTypesConversion::INNER_CONVERT_ARA_GEO(target_packet.longitude),
        UInnerDataTypesConversion::INNER_CONVERT_ARA_GEO(target_packet.altitude),
        FGEO_DOUBLE_STRUCT(z_fix),//184.0),
        X,
        Y,
        Z
    );

    YAW = UCommonOperations::ARA_DOUBLE_STRUCT_As_Float(target_packet.pitch);
    PITCH = UCommonOperations::ARA_DOUBLE_STRUCT_As_Float(target_packet.roll);
    ROLL = UCommonOperations::ARA_DOUBLE_STRUCT_As_Float(target_packet.course);

}
