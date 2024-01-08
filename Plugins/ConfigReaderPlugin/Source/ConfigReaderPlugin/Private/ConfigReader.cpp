// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigReader.h"

#include "Engine.h"

bool UConfigReader::SimpleConfigLoader(TMap<FString, FString>& parametersMap, FString configPath)
{
	if (IFileManager::Get().FileExists(*configPath))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, TEXT("Config exists"));

		FJsonSerializableArray result;
		TFunctionRef<bool(const FString& str)> strFilter = [](const FString& str) {return !(str.Contains("#")); };

		FFileHelper::LoadFileToStringArrayWithPredicate(result, *configPath, strFilter);

		for (FString& line : result)
		{
			FString key;
			FString val;
			if (line.Split("=", &key, &val))
				parametersMap.Add(key, val);
		}

		return true;
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, TEXT("Config not found"));

	return false;
}

bool UConfigReader::ConfigLoader(TMap<FString, FParametersList>& parametersMap, FString configPath)
{
	if (IFileManager::Get().FileExists(*configPath))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, TEXT("Config exists"));

		FJsonSerializableArray result;
		TFunctionRef<bool(const FString& str)> strFilter = [](const FString& str) {return !(str.Contains("#")); };

		FFileHelper::LoadFileToStringArrayWithPredicate(result, *configPath, strFilter);

		FString SectionName;
		FParametersList params;

		for (FString& line : result)
		{
			if (line.IsEmpty())
				continue;

			line = line.TrimEnd();

			// Section finded
			if (line[0] == '[' && line[line.Len() - 1] == ']')
			{
				if (!SectionName.IsEmpty())
				{
					parametersMap.Add(SectionName, params);
					SectionName.Empty();
				}

				params.Clear();
				SectionName = line.Mid(1, line.Len() - 2);
				continue;
			}
			
			// Read Section Info
			if (!SectionName.IsEmpty())
			{
				FString key;
				FString value;

				if (line.Split("=", &key, &value))
					params.Add(FConfigParameter(key, value));
			}
		}
		if (!SectionName.IsEmpty())
		{
			parametersMap.Add(SectionName, params);
		}

		return true;
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, TEXT("Config not found"));

	return false;
}
