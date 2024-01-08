// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonOperations.h"

float UCommonOperations::ARA_DOUBLE_STRUCT_As_Float(FARA_DOUBLE_STRUCT value)
{
    return (float)value.convert_to_double();
}