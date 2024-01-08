#pragma once

#include "CoreMinimal.h"
#include "CamAndLight.generated.h"

//  
struct CamAndLight
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

    //   
    double lux;
    double l_rx;
    double l_ry;
    double l_rz;

};

/**       **/
USTRUCT(BlueprintType)
struct FCamAndLight
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float lux;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float l_rx;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float l_ry;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float l_rz;

    FCamAndLight()
    {
        index = 0;

        xc = 0;
        yc = 0;
        zc = 0;

        rx = 0;
        ry = 0;
        rz = 0;

        lux = 0;
        l_rx = 0;
        l_ry = 0;
        l_rz = 0;
    }

	FCamAndLight(const CamAndLight& v_data)
	{
		index = v_data.index;

        xc = v_data.xc;
        yc = v_data.yc;
        zc = v_data.zc;

        rx = v_data.rx;
        ry = v_data.ry;
        rz = v_data.rz;

        lux = v_data.lux;
        l_rx = v_data.l_rx;
        l_ry = v_data.l_ry;
        l_rz = v_data.l_rz;
	}



    FCamAndLight& operator=(const CamAndLight& v_data);

};
