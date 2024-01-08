// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <sstream>

#include "CoreMinimal.h"
#include "VF_DOUBLE_STRUCT.generated.h"

using namespace std;

// Double representation for usage in Blueprints
USTRUCT(BluePrintType)
struct FVF_DOUBLE_STRUCT
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FString double_string;

	FVF_DOUBLE_STRUCT()
	{
		double_string = TEXT("0.0");
	}

	FVF_DOUBLE_STRUCT(FString number_string)
	{
		double_string = number_string;
	}

	FVF_DOUBLE_STRUCT(double number)
	{
		double_string = FString(to_string(number).c_str());
	}

	double convert_to_double()
	{
		wstring w_double_string(&double_string.GetCharArray()[0]);
		string s_double_string(w_double_string.begin(), w_double_string.end());

		return (stod(s_double_string));
	}

	void make_doubleStruct(double number, int precision = 18)
	{
		ostringstream sout;
		sout.precision(precision);
		sout << number;
		double_string = FString(sout.str().c_str());
	}

};
