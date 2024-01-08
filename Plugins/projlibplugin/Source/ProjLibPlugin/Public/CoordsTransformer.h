// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


THIRD_PARTY_INCLUDES_START
//PRAGMA_PUSH_PLATFORM_DEFAULT_PACKING
#include <proj.h>
//PRAGMA_POP_PLATFORM_DEFAULT_PACKING
THIRD_PARTY_INCLUDES_END


/**
 * 
 */
class PROJLIBPLUGIN_API CoordsTransformer
{
private:
    PJ_CONTEXT* C;
    PJ* P;
    PJ_COORD a, b;

public:
    CoordsTransformer(const char* src_crs, const char* dst_crs);
    CoordsTransformer() = delete;
    ~CoordsTransformer();
    void transform(double src_x, double src_y, double& dst_x, double& dst_y);
};
