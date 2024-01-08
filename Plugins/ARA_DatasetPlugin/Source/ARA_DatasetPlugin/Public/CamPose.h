#pragma once

#include "CoreMinimal.h"
#include "CamPose.generated.h"

//  
struct CamPose
{
    //  
    int index;

    //  
    double xc;
    double yc;
    double zc;

    double rx;
    double ry;
    double rz;

};

/**       **/
USTRUCT(BlueprintType)
struct FCamPose
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int index;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float xc;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float yc;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float zc;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float rx;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float ry;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float rz;

    FCamPose()
    {
        index = 0;

        xc = 0;
        yc = 0;
        zc = 0;

        rx = 0;
        ry = 0;
        rz = 0;
    }

    FCamPose(const CamPose& cam_pose)
    {
        index = cam_pose.index;

        xc = cam_pose.xc;
        yc = cam_pose.yc;
        zc = cam_pose.zc;

        rx = cam_pose.rx;
        ry = cam_pose.ry;
        rz = cam_pose.rz;
    }

};

