#pragma once

#include "CoreMinimal.h"
#include "EnvironmentData.generated.h"

//  
struct EnvironmentData
{
    //  
    int index;

	//
	int weather_code;

    //   
    double lux;
    double l_rx;
    double l_ry;
    double l_rz;

};

/**       **/
USTRUCT(BlueprintType)
struct FEnvironmentData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int index;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int weather_code;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float lux;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float l_rx;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float l_ry;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float l_rz;

    FEnvironmentData()
    {
        index = 0;

		weather_code = 0;

        lux = 0;
        l_rx = 0;
        l_ry = 0;
        l_rz = 0;
    }

    FEnvironmentData(const EnvironmentData& env)
    {
        index = env.index;

		weather_code = env.weather_code;

        lux = env.lux;
        l_rx = env.l_rx;
        l_ry = env.l_ry;
        l_rz = env.l_rz;
    }

};