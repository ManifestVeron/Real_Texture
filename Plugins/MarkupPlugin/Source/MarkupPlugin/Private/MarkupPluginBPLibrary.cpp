// Copyright Epic Games, Inc. All Rights Reserved.

#include "MarkupPluginBPLibrary.h"
#include "MarkupPlugin.h"
#include "json.hpp"
#include <string>
#include "Math/Vector.h"
#include <fstream>
#include <Models_Group.h>

using json = nlohmann::json;

TArray<FVector> TraceShift;


UMarkupPluginBPLibrary::UMarkupPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TraceShift.Add(FVector(0, 0, 0));
	for (int x = 0; x <= 25; x += 25) //x = -25; x <= 25; x += 25
	{
		for (int y = 0; y <= 25; y += 25) //y = -25; y <= 25; y += 25
		{
			for (int z = 75; z <= 100; z += 25) //z = 50; z <= 100; z += 25
			{
				TraceShift.Add(FVector(x, y, z));
			}
		}
	}
}


TArray<FMarkup> UMarkupPluginBPLibrary::MakeMarkup
(
	USceneCaptureComponent2D* Camera,
	TArray<AActor*> actors,
	TMap<UObject*, FVector> map,
	float hitThreshold,
	ECollisionChannel collisionChannel,
	EDrawDebugTrace::Type drawDebugType,
	bool bTraceComplex,
	bool relativeCoordinates,
	bool center
) {
	TArray<FMarkup> resultArrayMarkup;
	FMarkup objectMarkup;
	int id = 1, type = 1, objectId = 1;
	for (AActor* Actor : actors)
	{
		AModels_Group* one = Cast<AModels_Group>(Actor);
		type = one->Class_id;
		objectId = one->Unique_object_id;
		for (auto element : map)
		{
			AModels_Group* two = Cast<AModels_Group>(element.Key);
			if (EqualUnique_object_id(one->Unique_object_id, two->Unique_object_id))
			{
				if (GenerateObjectMarkup(
					Camera,
					Actor,
					type,
					objectMarkup,
					relativeCoordinates,
					UEngineTypes::ConvertToTraceType(collisionChannel),
					drawDebugType,
					bTraceComplex,
					hitThreshold
				))
				{
					objectMarkup.id = id++;
					objectMarkup.objectId = objectId;
					objectMarkup.isRelativeCoordinates = relativeCoordinates;

					if (!center)
					{
						objectMarkup.ScreenPosition.X -= objectMarkup.ObjectScreenSize.X / 2;
						objectMarkup.ScreenPosition.Y -= objectMarkup.ObjectScreenSize.Y / 2;
					}

					resultArrayMarkup.Push(objectMarkup);
				}
			}
		};
	}

	return resultArrayMarkup;
}


void UMarkupPluginBPLibrary::GenerateJsonFile(TArray<FMarkup> array, FString nameFile) {
	std::fstream file;
	std::string way = static_cast<std::string>(TCHAR_TO_UTF8(*(FPaths::ProjectDir() + nameFile)));
	FString path = FPaths::ProjectDir() + nameFile;
	while (path.Len() > 0 && path[path.Len() - 1] != '\\' && path[path.Len() - 1] != '/') {
		path.RemoveAt(path.Len() - 1);
	}

	if (path.Len() > 0)
		path.RemoveAt(path.Len() - 1);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FileManager.CreateDirectoryTree(*path);

	file.open(way, std::ios::out);
	json j;
	file << "[\n";
	int step = 0;
	for (FMarkup objectMarkup : array) {

		j["id"] = std::to_string(objectMarkup.id);
		j["object_id"] = std::to_string(objectMarkup.objectId);
		j["objectClass"] = std::to_string(objectMarkup.objectClass);

		if (objectMarkup.isRelativeCoordinates) {
			j["region"]["origin"]["x"] = std::to_string((objectMarkup.ScreenPosition.X));
			j["region"]["origin"]["y"] = std::to_string((objectMarkup.ScreenPosition.Y));
			j["region"]["size"]["width"] = std::to_string((objectMarkup.ObjectScreenSize.X));
			j["region"]["size"]["height"] = std::to_string((objectMarkup.ObjectScreenSize.Y));
		}
		else {
			j["region"]["origin"]["x"] = std::to_string((int)(objectMarkup.ScreenPosition.X));
			j["region"]["origin"]["y"] = std::to_string((int)(objectMarkup.ScreenPosition.Y));
			j["region"]["size"]["width"] = std::to_string((int)(objectMarkup.ObjectScreenSize.X));
			j["region"]["size"]["height"] = std::to_string((int)(objectMarkup.ObjectScreenSize.Y));

		}

		if ((step++) == array.Num() - 1) {
			file << std::setw(4) << j << std::endl;

		}
		else {
			file << std::setw(4) << j << "," << std::endl;
		}

		j.clear();
	}

	file << "]";
}

FMarkupStruct UMarkupPluginBPLibrary::GenerateJsonStrings(TArray<FMarkup> array)
{
	TArray<FString> array_string;
	json j;
	int step = 0;
	for (FMarkup objectMarkup : array) {

		j["id"] = std::to_string(objectMarkup.id);
		j["object_id"] = std::to_string(objectMarkup.objectId);
		j["objectClass"] = std::to_string(objectMarkup.objectClass);
		if (objectMarkup.isRelativeCoordinates) {
			j["region"]["origin"]["x"] = std::to_string((objectMarkup.ScreenPosition.X));
			j["region"]["origin"]["y"] = std::to_string((objectMarkup.ScreenPosition.Y));
			j["region"]["size"]["width"] = std::to_string((objectMarkup.ObjectScreenSize.X));
			j["region"]["size"]["height"] = std::to_string((objectMarkup.ObjectScreenSize.Y));
		}
		else {
			j["region"]["origin"]["x"] = std::to_string((int)(objectMarkup.ScreenPosition.X));
			j["region"]["origin"]["y"] = std::to_string((int)(objectMarkup.ScreenPosition.Y));
			j["region"]["size"]["width"] = std::to_string((int)(objectMarkup.ObjectScreenSize.X));
			j["region"]["size"]["height"] = std::to_string((int)(objectMarkup.ObjectScreenSize.Y));

		}

		array_string.Add(FString(j.dump(4).c_str()));
		j.clear();
	}

	//transform from TArray<FString> to TArray<uint8>
	FMarkupStruct markup_container;
	for (FString line : array_string) {
		for (char symbol : line) {
			markup_container.markup_bytes.Add(symbol);
		}
	}

	markup_container.type = E_PR_MarkupType::JSON;

	return markup_container;
}



void UMarkupPluginBPLibrary::GenerateKittyFile
(
	TArray<FMarkup> array,
	FString nameFile
)
{
	std::fstream file;
	std::string way = static_cast<std::string>(TCHAR_TO_UTF8(*(FPaths::ProjectDir() + nameFile)));
	FString path = FPaths::ProjectDir() + nameFile;
	while (path.Len() > 0 && path[path.Len() - 1] != '\\' && path[path.Len() - 1] != '/') {
		path.RemoveAt(path.Len() - 1);
	}

	if (path.Len() > 0)
		path.RemoveAt(path.Len() - 1);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FileManager.CreateDirectoryTree(*path);

	file.open(way, std::ios::out);
	for (FMarkup objectMarkup : array) {

		file << objectMarkup.objectClass << " ";

		if (objectMarkup.isRelativeCoordinates) {
			file << std::to_string((objectMarkup.ScreenPosition.X)) + " " << std::to_string((objectMarkup.ScreenPosition.Y)) + " ";
			file << std::to_string(objectMarkup.ObjectScreenSize.X) << " " << std::to_string(objectMarkup.ObjectScreenSize.Y) << std::endl;
		}
		else {
			file << std::to_string((int)(objectMarkup.ScreenPosition.X)) + " " << std::to_string((int)(objectMarkup.ScreenPosition.Y)) + " ";
			file << std::to_string((int)objectMarkup.ObjectScreenSize.X) << " " << std::to_string((int)objectMarkup.ObjectScreenSize.Y) << std::endl;
		}

	}

}

FMarkupStruct UMarkupPluginBPLibrary::GenerateKittyStrings
(
	TArray<FMarkup> array,
	int32 trajectory_index,
	float CanDistance,
	bool TrajectoryStart,
	int32 WeatherCode
)
{
	TArray<FString> array_string;
	FString line;
	FMarkupMeta meta;
	meta.CanDistance = CanDistance;
	meta.TrajectoryStart = TrajectoryStart;
	meta.trajectory_index = trajectory_index;
	meta.WeatherCode = WeatherCode;

	for (FMarkup objectMarkup : array) {

		//line = "|" + FString::FromInt(objectMarkup.objectClass) + "| ";
		line = FString::FromInt(objectMarkup.objectId) + " ";
		if (objectMarkup.isRelativeCoordinates) {
			line += FString(std::to_string(objectMarkup.ScreenPosition.X).c_str()) + " " + FString(std::to_string(objectMarkup.ScreenPosition.Y).c_str()) + " ";
			line += FString(std::to_string(objectMarkup.ObjectScreenSize.X).c_str()) + " " + FString(std::to_string(objectMarkup.ObjectScreenSize.Y).c_str()) + " ";
		}
		else {
			line += FString::FromInt((int)(objectMarkup.ScreenPosition.X)) + " " + FString::FromInt((int)(objectMarkup.ScreenPosition.Y)) + " ";
			line += FString::FromInt((int)objectMarkup.ObjectScreenSize.X) + " " + FString::FromInt((int)objectMarkup.ObjectScreenSize.Y) + " ";
		}

		array_string.Add(line);
	}

	//transform from TArray<FString> to FMarkupBytesContainer
	FMarkupStruct markup_container;
	for (FString i_line : array_string) {
		for (char symbol : i_line) {
			markup_container.markup_bytes.Add(symbol);
		}
	}

	markup_container.type = E_PR_MarkupType::KITTY;
	markup_container.markup_meta = meta;
	return markup_container;
}

bool UMarkupPluginBPLibrary::GenerateObjectMarkup
(
	USceneCaptureComponent2D* Camera,
	AActor* Object,
	int Type,
	FMarkup& objectMarkup,
	bool relativeCoordinates,
	ETraceTypeQuery channelType,
	EDrawDebugTrace::Type drawDebugType,
	bool bTraceComplex,
	float hitThreshold
)
{
	bool onVisible = VisibilityCheckMultitrace(Camera, Object, channelType, drawDebugType, bTraceComplex, hitThreshold);

	if (!onVisible)
		return false;

	FVector2D ScreenPosition;
	FVector2D ObjectScreenSize;

	GetObjectAxisAlignBoxQuad(Camera, Object, ScreenPosition, ObjectScreenSize);

	int ScreenWidth = Camera->TextureTarget->SizeX;
	int ScreenHight = Camera->TextureTarget->SizeY;

	if ((ScreenPosition.X / ScreenWidth > 0.0 && ScreenPosition.X / ScreenWidth < 1.0) &&
		(ScreenPosition.Y / ScreenHight > 0.0 && ScreenPosition.Y / ScreenHight < 1.0))
	{
		if (relativeCoordinates)
		{
			ScreenPosition.X /= ScreenWidth;
			ScreenPosition.Y /= ScreenHight;
		}

		objectMarkup.ScreenPosition = ScreenPosition;
		objectMarkup.ObjectScreenSize = ObjectScreenSize;
		objectMarkup.objectClass = Type;
		objectMarkup.distance = UKismetMathLibrary::Vector_Distance(Camera->GetComponentLocation(), Object->GetActorLocation());

		return true;
	}

	return false;
}

bool UMarkupPluginBPLibrary::VisibilityCheckMultitrace
(
	USceneCaptureComponent2D* Camera,
	AActor* Object,
	ETraceTypeQuery channelType,
	EDrawDebugTrace::Type drawDebugType,
	bool bTraceComplex,
	float hitThreshold
)
{
	FVector ObjectLocation, Start, End;
	FHitResult OutHit;
	TArray<AActor*> array;

	volatile int countHit = 0;

	for (int i = 0; i < TraceShift.Num(); i++) 
	{
		ObjectLocation = Object->GetActorLocation();
		Start = Camera->GetComponentLocation();
		FVector normalCameraVector = 1000 * Camera->GetForwardVector();
		Start += normalCameraVector;

		End.X = TraceShift[i].X + ObjectLocation.X;
		End.Y = TraceShift[i].Y + ObjectLocation.Y;
		End.Z = TraceShift[i].Z + ObjectLocation.Z;


		UKismetSystemLibrary::LineTraceSingle
		(
			GEngine->GetCurrentPlayWorld(),
			Start,
			End,
			channelType,
			bTraceComplex,
			array,
			drawDebugType,
			OutHit,
			true
		);

		AActor* result_actor = OutHit.GetActor();
		if (result_actor == nullptr)
			continue;
		
		UClass* result_actor_class = result_actor->GetClass();
		UClass* object_class = Object->GetClass();
		if (result_actor != nullptr && UKismetMathLibrary::EqualEqual_ObjectObject(result_actor_class, object_class))
		{
			countHit += 1;
		}
	}

	// Threshold
	float hitsPercentage = static_cast<float>(countHit) / TraceShift.Num();
	if (hitsPercentage >= hitThreshold)
	{
		UE_LOG(LogTemp, Warning, TEXT("Object %s is visible!"), *Object->GetFName().ToString());
		return true;
	}

	return false;
}

void UMarkupPluginBPLibrary::GetObjectAxisAlignBoxQuad
(
	USceneCaptureComponent2D* Camera,
	AActor* Object,
	FVector2D& ScreenPosition,
	FVector2D& ObjectScreenSize
)
{
	TArray<FVector4> array;
	FVector vectorObject;
	FVector vectorObjectBoxExtent;


	Object->GetActorBounds(true, vectorObject, vectorObjectBoxExtent);

	array.Push(FVector4(vectorObject.X - vectorObjectBoxExtent.X, vectorObject.Y - vectorObjectBoxExtent.Y, vectorObject.Z + vectorObjectBoxExtent.Z));
	array.Push(FVector4(vectorObject.X - vectorObjectBoxExtent.X, vectorObject.Y + vectorObjectBoxExtent.Y, vectorObject.Z + vectorObjectBoxExtent.Z));
	array.Push(FVector4(vectorObject.X + vectorObjectBoxExtent.X, vectorObject.Y + vectorObjectBoxExtent.Y, vectorObject.Z + vectorObjectBoxExtent.Z));
	array.Push(FVector4(vectorObject.X + vectorObjectBoxExtent.X, vectorObject.Y - vectorObjectBoxExtent.Y, vectorObject.Z + vectorObjectBoxExtent.Z));
	array.Push(FVector4(vectorObject.X + vectorObjectBoxExtent.X, vectorObject.Y - vectorObjectBoxExtent.Y, vectorObject.Z - vectorObjectBoxExtent.Z));
	array.Push(FVector4(vectorObject.X - vectorObjectBoxExtent.X, vectorObject.Y - vectorObjectBoxExtent.Y, vectorObject.Z - vectorObjectBoxExtent.Z));
	array.Push(FVector4(vectorObject.X - vectorObjectBoxExtent.X, vectorObject.Y + vectorObjectBoxExtent.Y, vectorObject.Z - vectorObjectBoxExtent.Z));
	array.Push(FVector4(vectorObject.X + vectorObjectBoxExtent.X, vectorObject.Y + vectorObjectBoxExtent.Y, vectorObject.Z - vectorObjectBoxExtent.Z));

	UMarkupPluginBPLibrary::RelativeRotation(Object, array);
	TArray<FVector2D> vectors;
	UMarkupPluginBPLibrary::ProjectBoundingBoxCorners(Camera, array, vectors);
	float XMax = vectors[0].X, XMin = vectors[0].X, YMax = vectors[0].Y, YMin = vectors[0].Y;

	for (int i = 1; i < vectors.Num(); i++) {
		if (vectors[i].X > XMax)
			XMax = vectors[i].X;

		if (vectors[i].X < XMin)
			XMin = vectors[i].X;

		if (vectors[i].Y > YMax)
			YMax = vectors[i].Y;

		if (vectors[i].Y < YMin)
			YMin = vectors[i].Y;
	}

	if (XMin < 0) XMin = 0;
	if (XMax < 0) XMax = 0;
	if (YMin < 0) YMin = 0;
	if (YMax < 0) YMax = 0;

	ScreenPosition.X = (XMax - XMin) / 2 + XMin;
	ScreenPosition.Y = (YMax - YMin) / 2 + YMin;

	ObjectScreenSize.X = XMax - XMin;
	ObjectScreenSize.Y = YMax - YMin;

}

void UMarkupPluginBPLibrary::RelativeRotation(AActor* Object, TArray<FVector4>& array)
{
	FVector ObjectOrigin;
	FVector vectorObjectBoxExtent;
	FTransform transform;
	Object->GetActorBounds(false, ObjectOrigin, vectorObjectBoxExtent);
	FPlane plane1(ObjectOrigin.X * (-1), ObjectOrigin.Y * (-1), ObjectOrigin.Z * (-1), 1.0);
	FPlane plane2(ObjectOrigin.X, ObjectOrigin.Y, ObjectOrigin.Z, 1.0);

	FMatrix matrix1(
		FPlane(1.0, 0.0, 0.0, 0.0),
		FPlane(0.0, 1.0, 0.0, 0.0),
		FPlane(0.0, 0.0, 1.0, 0.0),
		plane1
	);

	transform = UKismetMathLibrary::MakeTransform(
		FVector(0.0, 0.0, 0.0),
		Object->GetActorRotation(),
		FVector(1.0, 1.0, 1.0)
	);

	FMatrix matrix2 = UKismetMathLibrary::Conv_TransformToMatrix(transform);

	FMatrix matrix3(
		FPlane(1.0, 0.0, 0.0, 0.0),
		FPlane(0.0, 1.0, 0.0, 0.0),
		FPlane(0.0, 0.0, 1.0, 0.0),
		plane2
	);

	for (int i = 0; i < array.Num(); i++) {
		array[i] = UKismetMathLibrary::TransformVector4(matrix1, array[i]);
		array[i] = UKismetMathLibrary::TransformVector4(matrix2, array[i]);
		array[i] = UKismetMathLibrary::TransformVector4(matrix3, array[i]);
	}

}


void UMarkupPluginBPLibrary::ProjectBoundingBoxCorners(USceneCaptureComponent2D* Camera, TArray<FVector4>& array, TArray<FVector2D>& vectors) {
	for (int i = 0; i < array.Num(); i++) {
		vectors.Push(ProjectWorldToScreen(Camera, array[i]));
	}
}

FVector2D UMarkupPluginBPLibrary::ProjectWorldToScreen(USceneCaptureComponent2D* Camera, FVector4 vector)
{
	FVector2D resultVector2d;
	float sizeX, sizeY;
	UTextureRenderTarget2D* textureTargetCamera = Camera->TextureTarget;
	FVector CameraLocation;
	FRotator CameraRotation;
	sizeX = static_cast<float>(textureTargetCamera->SizeX);
	sizeY = static_cast<float>(textureTargetCamera->SizeY);

	CameraLocation = Camera->GetComponentToWorld().GetLocation();
	CameraRotation = Camera->GetComponentToWorld().GetRotation().Rotator();

	FMinimalViewInfo DesiredView;
	DesiredView.Location = CameraLocation;
	DesiredView.Rotation = CameraRotation;
	DesiredView.FOV = Camera->FOVAngle;
	DesiredView.OrthoWidth = textureTargetCamera->GetSurfaceWidth();
	DesiredView.OrthoNearClipPlane = -1.0;
	DesiredView.OrthoFarClipPlane = 0.0;
	DesiredView.AspectRatio = sizeX / sizeY;
	DesiredView.bConstrainAspectRatio = true;
	DesiredView.bUseFieldOfViewForLOD = true;
	DesiredView.ProjectionMode = ECameraProjectionMode::Type::Perspective;
	DesiredView.PostProcessBlendWeight = 0.0;
	DesiredView.OffCenterProjectionOffset.X = 0.0;
	DesiredView.OffCenterProjectionOffset.Y = 0.0;

	FMatrix ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;
	UGameplayStatics::GetViewProjectionMatrix(DesiredView, ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);

	vector = UKismetMathLibrary::TransformVector4(ViewProjectionMatrix, vector);
	resultVector2d.X = (sizeX / 2) * (vector.X / vector.W + 1);
	resultVector2d.Y = (sizeY / 2) * (1 - vector.Y / vector.W);

	return resultVector2d;
}


