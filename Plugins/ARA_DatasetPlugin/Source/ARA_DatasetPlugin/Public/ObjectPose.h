#pragma once

#include "CoreMinimal.h"
#include "ObjectPose.generated.h"

struct ObjectsNum
{
    int index;
    int num;
};

struct ObjectPose
{
    //  
    int index;

    //
    int place_id;
    double xc;
    double yc;
    double zc;
    double ry;
    double rz;
    double rx;
};

/*

*/
USTRUCT(BluePrintType)
struct FObjectPose
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int index;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int place_id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float xc;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float yc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float zc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float ry;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float rz;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float rx;

    FObjectPose()
    {
        index = 0;
        place_id = 0;
        xc = 0;
        yc = 0;
        zc = 0;
        ry = 0;
        rz = 0;
        rx = 0;
    }

    FObjectPose(const ObjectPose& object_pose)
    {
        index = object_pose.index;
        place_id = object_pose.place_id;
        xc = object_pose.xc;
        yc = object_pose.yc;
        zc = object_pose.zc;
        ry = object_pose.ry;
        rz = object_pose.rz;
        rx = object_pose.rx;
    }

};