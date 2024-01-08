// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncCaptureLibrary.h"


void UAsyncCaptureLibrary::AsyncCaptureImageDelegated
(
	FImageStructDelegate Out,
    USceneCaptureComponent2D* CameraCapture,
	float Time,
	E_PR_ImageFormat imFormat,
	int imQuality
)
{
    FRenderTarget* RenderTarget = CameraCapture->TextureTarget->GameThread_GetRenderTargetResource();
	
	const int32 img_width = RenderTarget->GetSizeXY().X;
	const int32 img_height = RenderTarget->GetSizeXY().Y;
	const int32 img_array_size = img_width * img_height;

	EImageFormat im_format = EImageFormat::PNG;
	FString img_extension = TEXT(".png");

	if (imFormat == E_PR_ImageFormat::JPEG)
	{
		im_format = EImageFormat::JPEG;
		img_extension = TEXT(".jpg");
	}
	else if (imFormat == E_PR_ImageFormat::RAW_RED)
	{
		UE_LOG(LogTemp, Log, TEXT("This function does not supported raw format!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [=]()
	{
		TArray<FColor> RawPixels;

		bool bCaptured = false;

		bCaptured = ReadPixelsLib::ThreadSafe_ReadPixels(RenderTarget, RawPixels, Time);

		TArray<uint8> CapturedImage;

		if (!bCaptured || (RawPixels.Num() != img_array_size))
		{
			UE_LOG(LogTemp, Log, TEXT("!!BAD CAPTURE OF IMAGE"));
			return;
		}
		
		UE_LOG(LogTemp, Log, TEXT("IMG WAS CAPTURED"));

		if (imFormat == E_PR_ImageFormat::PNG)
		{
			for (int i = 0; i < RawPixels.Num(); ++i)
			{
				RawPixels[i].A = 255;
			}
		}

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(im_format);

		if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&RawPixels[0],
			RawPixels.Num() * sizeof(FColor), img_width, img_height, ERGBFormat::BGRA, 8))
		{
			CapturedImage = ImageWrapper->GetCompressed(imQuality);
		}

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			Out.ExecuteIfBound(FImageStruct(CapturedImage, img_width, img_height, imFormat));
		});
		
	});
}

void UAsyncCaptureLibrary::AsyncCaptureImageRawDelegated
(
	FImageStructDelegate Out,
	USceneCaptureComponent2D* CameraCapture,
	float Time
)
{
	FRenderTarget* RenderTarget = CameraCapture->TextureTarget->GameThread_GetRenderTargetResource();

	const int32 img_width = RenderTarget->GetSizeXY().X;
	const int32 img_height = RenderTarget->GetSizeXY().Y;
	const int32 img_array_size = img_width * img_height;

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [=]()
	{
		TArray<FColor> RawPixels;

		bool bCaptured = false;

		bCaptured = ReadPixelsLib::ThreadSafe_ReadPixels(RenderTarget, RawPixels, Time);

		TArray<uint8> CapturedImage;

		if (!bCaptured || (RawPixels.Num() != img_array_size))
		{
			UE_LOG(LogTemp, Log, TEXT("!!BAD CAPTURE OF IMAGE"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("IMG WAS CAPTURED"));

		for (int i = 0; i < CapturedImage.Num(); ++i)
		{
			CapturedImage.Add(RawPixels[i].R);
		}

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			Out.ExecuteIfBound(FImageStruct(
				CapturedImage, 
				img_width, 
				img_height, 
				E_PR_ImageFormat::RAW_RED));
		});
		
	});
}


void UAsyncCaptureLibrary::AsyncCaptureImageWithSaveDelegated
(
	FImageStructDelegate Out,
	USceneCaptureComponent2D* CameraCapture,
	float Time,
	FString FileName,
	FString SubFolder,
	E_PR_ImageFormat imFormat,
	int imQuality,
	bool bNeedSaved
)
{
	FRenderTarget* RenderTarget = CameraCapture->TextureTarget->GameThread_GetRenderTargetResource();

	const int32 img_width = RenderTarget->GetSizeXY().X;
	const int32 img_height = RenderTarget->GetSizeXY().Y;
	const int32 img_array_size = img_width * img_height;

	EImageFormat im_format = EImageFormat::PNG;
	FString img_extension = TEXT(".png");

	if (imFormat == E_PR_ImageFormat::JPEG)
	{
		im_format = EImageFormat::JPEG;
		img_extension = TEXT(".jpg");
	}
	else if (imFormat == E_PR_ImageFormat::RAW_RED)
	{
		UE_LOG(LogTemp, Log, TEXT("This function does not supported raw format!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [=]()
	{
		TArray<FColor> RawPixels;

		bool bCaptured = false;

		bCaptured = ReadPixelsLib::ThreadSafe_ReadPixels(RenderTarget, RawPixels, Time);

		if (!bCaptured || (RawPixels.Num() != img_array_size))
		{
			UE_LOG(LogTemp, Log, TEXT("!!BAD CAPTURE OF IMAGE"));
			return;
		}
		
		UE_LOG(LogTemp, Log, TEXT("IMG WAS CAPTURED"));

		if (imFormat == E_PR_ImageFormat::PNG)
		{
			for (int i = 0; i < RawPixels.Num(); ++i)
			{
				RawPixels[i].A = 255;
			}
		}

		TArray<uint8> CapturedImage;

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(im_format);

		if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&RawPixels[0],
			RawPixels.Num() * sizeof(FColor), img_width, img_height, ERGBFormat::BGRA, 8))
		{
			CapturedImage = ImageWrapper->GetCompressed(imQuality);
		}
		

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
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
					FFileHelper::SaveArrayToFile(CapturedImage, *(PathToFile));

					UE_LOG(LogTemp, Log, TEXT("WE SAVE Image as FILE... %s"), *PathToFile);

				}
			}

			Out.ExecuteIfBound(FImageStruct(CapturedImage, img_width, img_height, imFormat));
		});
	});
}


bool UAsyncCaptureLibrary::AsyncCaptureImageWithSaveReternedByFence
(
	FImageStruct& img_data,
	USceneCaptureComponent2D* CameraCapture,
	FString FileName,
	FString SubFolder,
	E_PR_ImageFormat imFormat,
	int imQuality,
	bool bNeedSaved
)
{
	FRenderTarget* RenderTarget = CameraCapture->TextureTarget->GameThread_GetRenderTargetResource();

	const int32 img_width = RenderTarget->GetSizeXY().X;
	const int32 img_height = RenderTarget->GetSizeXY().Y;
	const int32 img_array_size = img_width * img_height;

	EImageFormat im_format = EImageFormat::PNG;
	FString img_extension = TEXT(".png");

	if (imFormat == E_PR_ImageFormat::JPEG)
	{
		im_format = EImageFormat::JPEG;
		img_extension = TEXT(".jpg");
	}
	else if (imFormat == E_PR_ImageFormat::RAW_RED)
	{
		UE_LOG(LogTemp, Log, TEXT("This function does not supported raw format!"));
		return false;
	}

	TArray<FColor> ColorBuffer;
	TArray<uint8> CapturedImage;

	FRenderCommandFence ReadPixelsFence;
	do {
		ReadPixelsLib::ThreadSafe_ReadPixels(RenderTarget, ColorBuffer);
		ReadPixelsFence.BeginFence();
		ReadPixelsFence.Wait();
	} while (!ReadPixelsFence.IsFenceComplete() && ColorBuffer.Num() != img_array_size);

	if (!ColorBuffer.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("!!BAD CAPTURE OF IMAGE with name '%s.%s'!!"), *FileName, *img_extension);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("IMG '%s.%s' WAS CAPTURED"), *FileName, *img_extension);
	UE_LOG(LogTemp, Log, TEXT("Color Buffer size - %d. Resolution %d x %d"), 
		ColorBuffer.Num(), img_width, img_height);

	if (imFormat == E_PR_ImageFormat::PNG)
	{
		for (int i = 0; i < ColorBuffer.Num(); ++i)
		{
			ColorBuffer[i].A = 255;
		}
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(im_format);

	if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&ColorBuffer[0],
		ColorBuffer.Num() * sizeof(FColor), img_width, img_height, ERGBFormat::BGRA, 8))
	{
		CapturedImage = ImageWrapper->GetCompressed(imQuality);
	}

	AsyncTask(ENamedThreads::AnyThread, [=]()
	{

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
				FFileHelper::SaveArrayToFile(CapturedImage, *(PathToFile));

				UE_LOG(LogTemp, Log, TEXT("WE SAVE Image as FILE... %s"), *PathToFile);

			}
		}
	});
	
	img_data.img_array = CapturedImage;
	img_data.width = img_width;
	img_data.height = img_height;
	img_data.img_format = imFormat;

	return true;
}

bool UAsyncCaptureLibrary::AsyncCaptureImageRawReternedByFence
(
	FImageStruct& img_data,
	USceneCaptureComponent2D* CameraCapture
)
{
	FRenderTarget* RenderTarget = CameraCapture->TextureTarget->GameThread_GetRenderTargetResource();

	const int32 img_width = RenderTarget->GetSizeXY().X;
	const int32 img_height = RenderTarget->GetSizeXY().Y;
	const int32 img_array_size = img_width * img_height;

	TArray<FColor> ColorBuffer;
	TArray<uint8> CapturedImage;

	FRenderCommandFence ReadPixelsFence;
	do {
		ReadPixelsLib::ThreadSafe_ReadPixels(RenderTarget, ColorBuffer);
		ReadPixelsFence.BeginFence();
		ReadPixelsFence.Wait();
	} while (!ReadPixelsFence.IsFenceComplete() && ColorBuffer.Num() != img_array_size);

	if (!ColorBuffer.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("!!BAD CAPTURE OF IMAGE with name !!"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("IMG  WAS CAPTURED"));
	UE_LOG(LogTemp, Log, TEXT("Color Buffer size - %d. Resolution %d x %d"),
		ColorBuffer.Num(), img_width, img_height);

	for (int i = 0; i < ColorBuffer.Num(); ++i)
	{
		CapturedImage.Add(ColorBuffer[i].R);
	}

	img_data.img_array = CapturedImage;
	img_data.width = img_width;
	img_data.height = img_height;
	img_data.img_format = E_PR_ImageFormat::RAW_RED;

	return true;
}

bool UAsyncCaptureLibrary::AsyncCaptureImageWithSaveReterned
(
	FImageStruct& img_data,
	USceneCaptureComponent2D* CameraCapture,
	float Time,
	FString FileName,
	FString SubFolder,
	E_PR_ImageFormat imFormat,
	int imQuality,
	bool bNeedSaved
)
{
	FRenderTarget* RenderTarget = CameraCapture->TextureTarget->GameThread_GetRenderTargetResource();

	if (RenderTarget == nullptr)
		return false;

	const int32 img_width = RenderTarget->GetSizeXY().X;
	const int32 img_height = RenderTarget->GetSizeXY().Y;
	const int32 img_array_size = img_width * img_height;

	EImageFormat im_format = EImageFormat::PNG;
	FString img_extension = TEXT(".png");

	if (imFormat == E_PR_ImageFormat::JPEG)
	{
		im_format = EImageFormat::JPEG;
		img_extension = TEXT(".jpg");
	}
	else if (imFormat == E_PR_ImageFormat::RAW_RED)
	{
		UE_LOG(LogTemp, Log, TEXT("This function does not supported raw format!"));
		return false;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [=, &img_data]()
	{
		TArray<FColor> RawPixels;

		bool bCaptured = false;

		bCaptured = ReadPixelsLib::ThreadSafe_ReadPixels(RenderTarget, RawPixels, Time);

		if (!bCaptured || (RawPixels.Num() != img_array_size))
		{
			UE_LOG(LogTemp, Log, TEXT("!!BAD CAPTURE OF IMAGE with name '%s.%s'!!"), *FileName, *img_extension);
			UE_LOG(LogTemp, Log, TEXT("Color Buffer size - %d. Resolution %d x %d"),
				RawPixels.Num(), img_width, img_height);
			return false;
		}

		UE_LOG(LogTemp, Log, TEXT("IMG '%s.%s' WAS CAPTURED"), *FileName, *img_extension);
		UE_LOG(LogTemp, Log, TEXT("Color Buffer size - %d. Resolution %d x %d"),
			RawPixels.Num(), img_width, img_height);

		if (imFormat == E_PR_ImageFormat::PNG)
		{
			for (int i = 0; i < RawPixels.Num(); ++i)
			{
				RawPixels[i].A = 255;
			}
		}

		TArray<uint8> CapturedImage;

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(im_format);

		if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&RawPixels[0],
			RawPixels.Num() * sizeof(FColor), img_width, img_height, ERGBFormat::BGRA, 8))
		{
			CapturedImage = ImageWrapper->GetCompressed();
		}

		AsyncTask(ENamedThreads::GameThread, [=, &img_data]()
		{
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
					FFileHelper::SaveArrayToFile(CapturedImage, *(PathToFile));

					UE_LOG(LogTemp, Log, TEXT("WE SAVE Image as FILE... %s"), *PathToFile);

				}
			}

			img_data.img_array = CapturedImage;
			img_data.width = img_width;
			img_data.height = img_height;
			img_data.img_format = imFormat;

		});
		return true;
	});
	return true;
}

