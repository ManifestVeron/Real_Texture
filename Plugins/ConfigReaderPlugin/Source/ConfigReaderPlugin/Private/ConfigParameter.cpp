// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigParameter.h"

void FParametersList::Add(FConfigParameter vParameter)
{
	parameters.Add(vParameter);
}

void FParametersList::Clear()
{
	parameters.Empty();
}