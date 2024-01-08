// Fill out your copyright notice in the Description page of Project Settings.


#include "InnerDataTypesConversion.h"


FGEO_DOUBLE_STRUCT UInnerDataTypesConversion::INNER_CONVERT_ARA_GEO(FARA_DOUBLE_STRUCT value)
{
    return FGEO_DOUBLE_STRUCT(value.double_string);
}


FVF_DOUBLE_STRUCT UInnerDataTypesConversion::INNER_CONVERT_ARA_VF(FARA_DOUBLE_STRUCT value)
{
    return FVF_DOUBLE_STRUCT(value.double_string);
}

FARA_DOUBLE_STRUCT UInnerDataTypesConversion::INNER_CONVERT_GEO_ARA(FGEO_DOUBLE_STRUCT value)
{
    return FARA_DOUBLE_STRUCT(value.double_string);
}

FARA_DOUBLE_STRUCT UInnerDataTypesConversion::INNER_CONVERT_VF_ARA(FVF_DOUBLE_STRUCT value)
{
    return FARA_DOUBLE_STRUCT(value.double_string);
}

FGEO_DOUBLE_STRUCT UInnerDataTypesConversion::INNER_CONVERT_VF_GEO(FVF_DOUBLE_STRUCT value)
{
    return FGEO_DOUBLE_STRUCT(value.double_string);
}

FVF_DOUBLE_STRUCT UInnerDataTypesConversion::INNER_CONVERT_GEO_VF(FGEO_DOUBLE_STRUCT value)
{
    return FVF_DOUBLE_STRUCT(value.double_string);
}
