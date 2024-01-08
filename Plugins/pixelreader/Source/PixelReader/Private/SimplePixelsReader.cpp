// Fill out your copyright notice in the Description page of Project Settings.


#include "SimplePixelsReader.h"

// Sets default values
ASimplePixelsReader::ASimplePixelsReader():
	bReadPixelsStarted(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ColorBuffer.Empty();
}

// Called when the game starts or when spawned
void ASimplePixelsReader::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASimplePixelsReader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASimplePixelsReader::CaptureImage
(
	FImageStructDelegate Out, 
	E_PR_ImageFormat imFormat,
	int imQuality
)
{
	FRenderTarget* rt = static_cast<FTextureRenderTargetResource*>(RenderTarget->Resource);
	if (rt == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Render Target is Empty!!!"));
		return;
	}

	int32 img_width = rt->GetSizeXY().X;
	int32 img_height = rt->GetSizeXY().Y;
	int32 img_array_size = img_width * img_height;

	if (UpdateBuffer(rt) && ColorBuffer.Num() == img_array_size)
	{
		EImageFormat im_format = EImageFormat::PNG;
		FString img_extension = TEXT(".png");

		
		if (imFormat == E_PR_ImageFormat::JPEG)
		{
			im_format = EImageFormat::JPEG;
			img_extension = TEXT(".jpg");
		}

		if (imFormat == E_PR_ImageFormat::PNG)
		{
			for (int i = 0; i < ColorBuffer.Num(); ++i)
			{
				ColorBuffer[i].A = 255;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("IMG WAS CAPTURED"));

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(im_format);

		TArray<uint8> result_img_array;

		if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&ColorBuffer[0],
		img_array_size * sizeof(FColor), img_width, img_height, ERGBFormat::BGRA, 8))
		{
			result_img_array = ImageWrapper->GetCompressed(imQuality);

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				Out.ExecuteIfBound(FImageStruct(result_img_array, img_width, img_height, imFormat));
			});

		}

		ColorBuffer.Empty();
	} 
	UE_LOG(LogTemp, Log, TEXT("CAPTURED FUNC WAS CALLED"));
}

void ASimplePixelsReader::CaptureImageAsRaw
(
	FImageStructDelegate Out
)
{
	FRenderTarget* rt = static_cast<FTextureRenderTargetResource*>(RenderTarget->Resource);
	if (rt == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Render Target is Empty!!!"));
		return;
	}

	int32 img_width = rt->GetSizeXY().X;
	int32 img_height = rt->GetSizeXY().Y;
	int32 img_array_size = img_width * img_height;

	if (UpdateBuffer(rt) && ColorBuffer.Num() == img_array_size)
	{
		TArray<uint8> result_img_array;
		for (int i = 0; i < ColorBuffer.Num(); ++i)
		{
			result_img_array.Add(ColorBuffer[i].R);
		}
		
		UE_LOG(LogTemp, Log, TEXT("IMG WAS CAPTURED"));

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			Out.ExecuteIfBound(FImageStruct(result_img_array, img_width, img_height, E_PR_ImageFormat::RAW_RED));
		});

		ColorBuffer.Empty();
	}
	UE_LOG(LogTemp, Log, TEXT("CAPTURED FUNC WAS CALLED"));
}

void ASimplePixelsReader::CaptureImageAndSave
(
	FString FileName,
	FString SubFolder,
	E_PR_ImageFormat imFormat,
	int imQuality
)
{
	FRenderTarget* rt = static_cast<FTextureRenderTargetResource*>(RenderTarget->Resource);
	if (rt == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Render Target is Empty!!!"));
		return;
	}

	int32 img_width = rt->GetSizeXY().X;
	int32 img_height = rt->GetSizeXY().Y;
	int32 img_array_size = img_width * img_height;

	if (UpdateBuffer(rt) && ColorBuffer.Num() == img_array_size)
	{
		EImageFormat im_format = EImageFormat::PNG;
		FString img_extension = TEXT(".png");

		if (imFormat == E_PR_ImageFormat::JPEG)
		{
			im_format = EImageFormat::JPEG;
			img_extension = TEXT(".jpg");
		}

		if (imFormat == E_PR_ImageFormat::PNG)
		{
			for (int i = 0; i < ColorBuffer.Num(); ++i)
			{
				ColorBuffer[i].A = 255;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("IMG WAS CAPTURED"));

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(im_format);

		TArray<uint8> result_img_array;

		if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&ColorBuffer[0],
		img_array_size * sizeof(FColor), img_width, img_height, ERGBFormat::BGRA, 8))
		{
			result_img_array = ImageWrapper->GetCompressed(imQuality);

			bool bSubFolderMaked = true;

			if (!FPaths::DirectoryExists(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)))
			{
				bSubFolderMaked = IFileManager::Get().MakeDirectory(*(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)));
			}

			if (bSubFolderMaked)
			{
				FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder, FileName + img_extension);
				FFileHelper::SaveArrayToFile(result_img_array, *(PathToFile));

				UE_LOG(LogTemp, Log, TEXT("WE SAVE Image as FILE... %s"), *PathToFile);
			
			}

		}

		ColorBuffer.Empty();
	} 
	UE_LOG(LogTemp, Log, TEXT("CAPTURED FUNC WAS CALLED"));
}

void ASimplePixelsReader::CaptureImageWithSave
(
	FImageStructDelegate Out,
	FString FileName,
	FString SubFolder,
	E_PR_ImageFormat imFormat,
	int imQuality,
	bool bNeedSaved
)
{
	FRenderTarget* rt = static_cast<FTextureRenderTargetResource*>(RenderTarget->Resource);
	if (rt == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Render Target is Empty!!!"));
		return;
	}

	int32 img_width = rt->GetSizeXY().X;
	int32 img_height = rt->GetSizeXY().Y;
	int32 img_array_size = img_width * img_height;

	if(UpdateBuffer(rt) && ColorBuffer.Num() == img_array_size)
	{
		EImageFormat im_format = EImageFormat::PNG;
		FString img_extension = TEXT(".png");

		if (imFormat == E_PR_ImageFormat::JPEG)
		{
			im_format = EImageFormat::JPEG;
			img_extension = TEXT(".jpg");
		}

		if (imFormat == E_PR_ImageFormat::PNG)
		{
			for (int i = 0; i < ColorBuffer.Num(); ++i)
			{
				ColorBuffer[i].A = 255;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("IMG WAS CAPTURED"));

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(im_format);

		TArray<uint8> result_img_array;

		if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&ColorBuffer[0],
		img_array_size * sizeof(FColor), img_width, img_height, ERGBFormat::BGRA, 8))
		{
			result_img_array = ImageWrapper->GetCompressed(imQuality);

			if(bNeedSaved)
			{
				bool bSubFolderMaked = true;

				if (!FPaths::DirectoryExists(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)))
				{
					bSubFolderMaked = IFileManager::Get().MakeDirectory(*(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)));
				}

				if (bSubFolderMaked)
				{
					FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder, FileName + img_extension);
					FFileHelper::SaveArrayToFile(result_img_array, *(PathToFile));

					UE_LOG(LogTemp, Log, TEXT("WE SAVE Image as FILE... %s"), *PathToFile);
				
				}
			}

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				Out.ExecuteIfBound(FImageStruct(result_img_array, img_width, img_height, imFormat));
			});

		}

		ColorBuffer.Empty();
	} 
	UE_LOG(LogTemp, Log, TEXT("CAPTURED FUNC WAS CALLED"));
}

void ASimplePixelsReader::CaptureImageAsRawWithSave
(
	FImageStructDelegate RawOut,
	FString FileName,
	FString SubFolder,
	E_PR_ImageFormat imFormat,
	bool bNeedSaved
)
{
	FRenderTarget* rt = static_cast<FTextureRenderTargetResource*>(RenderTarget->Resource);
	if (rt == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Render Target is Empty!!!"));
		return;
	}

	int32 img_width = rt->GetSizeXY().X;
	int32 img_height = rt->GetSizeXY().Y;
	int32 img_array_size = img_width * img_height;

	if (UpdateBuffer(rt) && ColorBuffer.Num() == img_array_size)
	{
		EImageFormat im_format = EImageFormat::PNG;
		FString img_extension = TEXT(".png");

		if (imFormat == E_PR_ImageFormat::JPEG || imFormat == E_PR_ImageFormat::RAW_RED)
		{
			im_format = EImageFormat::JPEG;
			img_extension = TEXT(".jpg");
		}

		if (imFormat == E_PR_ImageFormat::PNG)
		{
			for (int i = 0; i < ColorBuffer.Num(); ++i)
			{
				ColorBuffer[i].A = 255;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("IMG WAS CAPTURED"));

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(im_format);

		TArray<uint8> result_img_array;
		TArray<uint8> raw_img_array;

		for (int i = 0; i < ColorBuffer.Num(); ++i)
		{
			raw_img_array.Add(ColorBuffer[i].R);
		}

		if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&ColorBuffer[0],
			img_array_size * sizeof(FColor), img_width, img_height, ERGBFormat::BGRA, 8))
		{
			result_img_array = ImageWrapper->GetCompressed();

			if (bNeedSaved)
			{
				bool bSubFolderMaked = true;

				if (!FPaths::DirectoryExists(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)))
				{
					bSubFolderMaked = IFileManager::Get().MakeDirectory(*(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)));
				}

				if (bSubFolderMaked)
				{
					FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder, FileName + img_extension);
					FFileHelper::SaveArrayToFile(result_img_array, *(PathToFile));

					UE_LOG(LogTemp, Log, TEXT("WE SAVE Image as FILE... %s"), *PathToFile);

				}
			}

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				RawOut.ExecuteIfBound(FImageStruct(raw_img_array, img_width, img_height, E_PR_ImageFormat::RAW_RED));
			});

		}

		ColorBuffer.Empty();
	}
	UE_LOG(LogTemp, Log, TEXT("CAPTURED FUNC WAS CALLED"));
}

bool ASimplePixelsReader::UpdateBuffer(FRenderTarget* RenderResource)
{
	ReadPixelsLib::ThreadSafe_ReadPixels(RenderResource, ColorBuffer);
	ReadPixelsFence.BeginFence();
	bReadPixelsStarted = true;
	ReadPixelsFence.Wait();
	if(bReadPixelsStarted && ReadPixelsFence.IsFenceComplete())
		return true;
	return false;
}
