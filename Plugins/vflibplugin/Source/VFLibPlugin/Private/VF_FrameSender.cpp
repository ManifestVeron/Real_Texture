// Fill out your copyright notice in the Description page of Project Settings.


#include "VF_FrameSender.h"

/*
*	Methods of Actor class for send Image Container
*/

// Sets default values
AVF_FrameSender::AVF_FrameSender()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AVF_FrameSender::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVF_FrameSender::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVF_FrameSender::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Stop_VF_Frame_Sender();
	Super::EndPlay(EndPlayReason);
}

void AVF_FrameSender::Start_VF_Frame_Sender
(
	FString ipAddress, 
	int32 port,
	int32 frameSize, 
	int32 sendDelay,
	FString ImgsSubfolder,
	bool bNeedSaveImgs,
	FString TelemetrySubfolder,
	bool bNeedSaveTelemetry
)
{
	TWeakObjectPtr<AVF_FrameSender> thisWeakObjPtr = TWeakObjectPtr<AVF_FrameSender>(this);
	frameSenderWorker = TUniquePtr<FVF_FrameSenderWorker>
		(
			new FVF_FrameSenderWorker
			(
				ipAddress, port, frameSize, sendDelay,
				ImgsSubfolder, bNeedSaveImgs, TelemetrySubfolder, bNeedSaveTelemetry,
				thisWeakObjPtr, TimeBetweenTicksForSender
			)
		);
	
	frameSenderWorker->Start();
}

void AVF_FrameSender::Stop_VF_Frame_Sender()
{
	if (frameSenderWorker)
	{
		UE_LOG(LogTemp, Warning, TEXT("VF Frame Sender Stopped"));
		frameSenderWorker->Stop();
	}
}

void AVF_FrameSender::SendImageContainer(FImageContainer img_container, FResizeImgParams resize_params)
{
	frameSenderWorker->SetImgResizeParams(resize_params);
	frameSenderWorker->AddToOutbox(img_container);
}

void AVF_FrameSender::PrintToConsole(FString Str, bool Error)
{
	if (Error)
	{
		auto messageLog = FMessageLog("VF Frame Sender");
		messageLog.Open(EMessageSeverity::Error, true);
		messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
	}
}

/*
*	Methods of Worker class for send Image Container
*/

FVF_FrameSenderWorker::FVF_FrameSenderWorker
(
	FString inIp,
	int32 inPort,
	int32 inframeSize, 
	int32 insendDelay, 
	FString inImgsSubfolder,
	bool inbNeedSaveImgs,
	FString inTelemetrySubfolder,
	bool inbNeedSaveTelemetry,
	TWeakObjectPtr<AVF_FrameSender> InOwner,
	float inTimeBetweenTicks
):
	ipAddress(inIp),
	port(inPort),
	frameSize(inframeSize),
	sendDelay(insendDelay),
	ImgsSubfolder(inImgsSubfolder),
	bNeedSaveImgs(inbNeedSaveImgs),
	TelemetrySubfolder(inTelemetrySubfolder),
	bNeedSaveTelemetry(inbNeedSaveTelemetry),
	TimeBetweenTicks(inTimeBetweenTicks),
	ThreadSpawnerActor(InOwner)
{
	ClearDirectory(ImgsSubfolder);
	
	ClearMetaFile(TEXT("GEO_Board_Location"), TelemetrySubfolder);
	ClearMetaFile(TEXT("Board_Rotation"), TelemetrySubfolder);
	ClearMetaFile(TEXT("Camera_Viewing_Angles"), TelemetrySubfolder);
}

FVF_FrameSenderWorker::~FVF_FrameSenderWorker()
{
	AsyncTask(ENamedThreads::GameThread, []()
	{
		AVF_FrameSender::PrintToConsole("VF Frame Sender thread was destroyed.", false);
	});

	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FVF_FrameSenderWorker::Start()
{
	check(!Thread && "Thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");
	
	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Thread isn't null. It's: %s"), *Thread->GetThreadName());
	}
	
	Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("VF Frame Sender thread %s"), *ipAddress), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Warning, TEXT("Created VF Frame Sender thread"));
}

void FVF_FrameSenderWorker::SetImgResizeParams(FResizeImgParams inParams)
{
	m_mutex.Lock();
	mResizeImgParams = inParams;
	m_mutex.Unlock();
}

void FVF_FrameSenderWorker::AddToOutbox(FImageContainer image_container)
{
	OutBox.Enqueue(image_container);
}

void FVF_FrameSenderWorker::SaveImage(FImageStruct image_struct, FString FileName)
{
	bool bSubFolderMaked = true;

	if (!FPaths::DirectoryExists(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), ImgsSubfolder)))
	{
		bSubFolderMaked = IFileManager::Get().MakeDirectory(*(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), ImgsSubfolder)));
	}

	if (bSubFolderMaked)
	{
		FString imFormat;
		if (image_struct.img_format == E_PR_ImageFormat::JPEG)
			imFormat = TEXT(".jpg");
		else if (image_struct.img_format == E_PR_ImageFormat::PNG)
			imFormat = TEXT(".png");
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Unsupported format for Image Saving"));
			return;
		}

		FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), ImgsSubfolder, FileName + imFormat);
		FFileHelper::SaveArrayToFile(image_struct.img_array, *(PathToFile));

		UE_LOG(LogTemp, Log, TEXT("Image SAVED as FILE... %s"), *PathToFile);

	}
}

void FVF_FrameSenderWorker::SaveImage(cv::Mat mat_image, E_PR_ImageFormat ImgFormat, FString FileName)
{
	bool bSubFolderMaked = true;

	if (!FPaths::DirectoryExists(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), ImgsSubfolder)))
	{
		bSubFolderMaked = IFileManager::Get().MakeDirectory(*(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), ImgsSubfolder)));
	}

	if (bSubFolderMaked)
	{
		FString imFormat;
		if (ImgFormat == E_PR_ImageFormat::JPEG)
			imFormat = TEXT(".jpg");
		else if (ImgFormat == E_PR_ImageFormat::PNG)
			imFormat = TEXT(".png");
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Unsupported format for Image Saving"));
			return;
		}


		FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), ImgsSubfolder, FileName + imFormat);
		
		wstring w_double_string(&PathToFile.GetCharArray()[0]);
		string sPathToFile(w_double_string.begin(), w_double_string.end());
		
		cv::imwrite(sPathToFile, mat_image);

		UE_LOG(LogTemp, Log, TEXT("Image SAVED as FILE... %s"), *PathToFile);

	}
}

void FVF_FrameSenderWorker::SaveMetaData
(
	FString FileName,
	FString SubFolder,
	int32 MetaIndex,
	FString MetaString
)
{
	bool bSubFolderMaked = true;

	if (!FPaths::DirectoryExists(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)))
	{
		bSubFolderMaked = IFileManager::Get().MakeDirectory(*(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)));
	}

	if (bSubFolderMaked)
	{
		FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder, FileName + ".txt");

		FFileHelper::SaveStringToFile
		(
			FString::Printf(TEXT("[%d] %s\n"), MetaIndex, *MetaString),
			*(PathToFile),
			FFileHelper::EEncodingOptions::AutoDetect,
			&IFileManager::Get(),
			FILEWRITE_Append
		);

		UE_LOG(LogTemp, Log, TEXT("METADATA SAVED IN FILE '%s'"), *PathToFile);
	}
}

void FVF_FrameSenderWorker::ClearMetaFile(FString FileName, FString SubFolder)
{
	FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder, FileName + ".txt");

	FFileHelper::SaveStringToFile
	(
		FString::Printf(TEXT("")),
		*(PathToFile),
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		FILEWRITE_None
	);
}

void FVF_FrameSenderWorker::ClearDirectory(FString SubFolder)
{
	FString PathToDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder);
	FPlatformFileManager::Get().Get().GetPlatformFile().DeleteDirectoryRecursively(*PathToDir);
	IFileManager::Get().MakeDirectory(*PathToDir);
}

bool FVF_FrameSenderWorker::Init()
{
	bRun = true;
	return true;
}

uint32 FVF_FrameSenderWorker::Run()
{
	std::wstring w_ipAddress(&ipAddress.GetCharArray()[0]);
	std::string s_ipAddress(w_ipAddress.begin(), w_ipAddress.end());
	
	if (InitSend(s_ipAddress.c_str(), port, 
		static_cast<size_t>(frameSize), static_cast<size_t>(sendDelay)) == 1)
	{
		return 1;
	}

	AsyncTask(ENamedThreads::GameThread, []()
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting VF Frame Sender thread."));
	});

	//int frameNumber = 0;

	while(bRun)
	{
		FDateTime timeBeginningOfTick = FDateTime::UtcNow();

		if(!OutBox.IsEmpty())
		{
			FImageContainer image_container;
			OutBox.Dequeue(image_container);

			if (!image_container.image_struct.img_array.Num())
				continue;

			TUAVData uav_data;
			uav_data.board 		= image_container.uav_data.boardNumber;
			uav_data.number		= image_container.idx;
			uav_data.latitude 	= image_container.uav_data.latitude.convert_to_double();
			uav_data.longitude	= image_container.uav_data.longitude.convert_to_double();
			uav_data.altitude 	= image_container.uav_data.altitude.convert_to_double();
			uav_data.yaw = uav_data.courseYaw = image_container.uav_data.yaw;
			uav_data.pitch = image_container.uav_data.pitch;
			uav_data.roll = image_container.uav_data.roll;

			TFrameDescription frame_description;
			TNavigation cam_navigation(image_container.idx);
			

			cam_navigation.SetGeoPosition
			(
				TGeoPosition
				(
					uav_data.latitude, 
					uav_data.longitude, 
					uav_data.altitude
				)
			);

			cam_navigation.SetOrientation
			(
				TOrientation
				(
					image_container.uav_camera_sight.roll, 
					image_container.uav_camera_sight.pitch, 
					image_container.uav_camera_sight.yaw
				)
			);

			frame_description.angleDimension.first = true;
			frame_description.AddNavigationItem(cam_navigation);

			FrameData proto_frame(frame_description, uav_data, PacketType::VIDEO);

			UE_LOG(LogTemp, Log, TEXT("[VF_Sender]: Data was passed to func SendData"));
			UE_LOG(LogTemp, Log, TEXT("[VF_Sender]: SC host: ip - %s port - %d"), *ipAddress, port);
			UE_LOG(LogTemp, Log, TEXT("[VF_Sender]: Frame number - %d"), image_container.idx);
			UE_LOG(LogTemp, Log, TEXT("[VF_Sender]: Board number - %d"), uav_data.board);
			UE_LOG(LogTemp, Log, TEXT("[VF_Sender]: uav lat %.8f | lon %.8f | alt %.8f"),
				uav_data.latitude, uav_data.longitude, uav_data.altitude);
			UE_LOG(LogTemp, Log, TEXT("[VF_Sender]: uav yaw %.8f | pitch %.8f | roll %.8f"),
				uav_data.yaw, uav_data.pitch, uav_data.roll);
			UE_LOG(LogTemp, Log, TEXT("[VF_Sender]: cam yaw %.8f | pitch %.8f | roll %.8f"),
				image_container.uav_camera_sight.yaw,
				image_container.uav_camera_sight.pitch,
				image_container.uav_camera_sight.roll);

			std::vector<uchar> convert_buffer;
			convert_buffer.assign(reinterpret_cast<uchar*>(image_container.image_struct.img_array.GetData()),
				reinterpret_cast<uchar*>(image_container.image_struct.img_array.GetData()) + image_container.image_struct.img_array.Num());

			std::vector<uchar> encoded_buffer = convert_buffer;

			if (mResizeImgParams.bNeedResize)
			{
				cv::Mat image_background = cv::imdecode(cv::Mat(convert_buffer), -1);

				cv::Mat resized_image;
				cv::resize(image_background, resized_image, 
					cv::Size(mResizeImgParams.NewWidth, mResizeImgParams.NewHeight),
					0.0, 0.0, cv::INTER_AREA);

				//std::vector<uchar> encoded_buffer(resized_image.total() * resized_image.elemSize()); // to resolve error with imencode deallocation!!!
				
				cv::String imFormat;
				std::vector<int> imParams;
				if (image_container.image_struct.img_format == E_PR_ImageFormat::JPEG) 
				{
					imFormat = cv::String(".jpg");
					imParams = { cv::IMWRITE_JPEG_QUALITY, (mResizeImgParams.imQuality == 0 ? 85 : mResizeImgParams.imQuality)};
				}
				else if (image_container.image_struct.img_format == E_PR_ImageFormat::PNG)
				{
					imFormat = cv::String(".png");
					int quality;
					// define png compress strategy 
					// imQuality from 0 to 100
					if (mResizeImgParams.imQuality == 0)
						quality = 8;
					if (mResizeImgParams.imQuality == 100)
						quality = 9;
					else if (mResizeImgParams.imQuality > 10 &&
						mResizeImgParams.imQuality < 100)
						quality = static_cast<int>(mResizeImgParams.imQuality / 10);
					else
						quality = 0;

					imParams = { cv::IMWRITE_PNG_COMPRESSION, quality };
				}

				encoded_buffer.resize(image_background.total() * image_background.elemSize());
				cv::imencode(imFormat, resized_image, encoded_buffer, imParams);

				if (bNeedSaveImgs)
					SaveImage(resized_image, image_container.image_struct.img_format, 
						FString::Printf(TEXT("%d"), image_container.idx));
			}
			else 
			{
				if (bNeedSaveImgs)
					SaveImage(image_container.image_struct, FString::Printf(TEXT("%d"), image_container.idx));
			}

			SendData
			(
				encoded_buffer.data(),//image_container.image_struct.img_array.GetData(), 
				encoded_buffer.size(),//image_container.image_struct.img_array.Num(),
				proto_frame,
				image_container.idx
			);
				
			if (bNeedSaveTelemetry)
			{
				FString MetaString;

				// Telemetry about GEO Board Location
				MetaString = FString::Printf(TEXT("BOARD Location: Latitude = %.9f Longitude = %.9f Altitude = %.9f"),
					uav_data.latitude, uav_data.longitude, uav_data.altitude);
				SaveMetaData(TEXT("GEO_Board_Location"), TelemetrySubfolder, image_container.idx, MetaString);

				// Telemetry about Board Rotation
				MetaString = FString::Printf(TEXT("BOARD Rotation: Yaw = %.8f Pitch = %.8f Roll = %.8f"),
					uav_data.yaw, uav_data.pitch, uav_data.roll);
				SaveMetaData(TEXT("Board_Rotation"), TelemetrySubfolder, image_container.idx, MetaString);

				// Telemetry about Cam Sighting
				MetaString = FString::Printf(TEXT("CAM Rotation: Yaw = %.8f Pitch = %.8f Roll = %.8f"
					"\n\t FOV Capture Cam = %.4f FOV Main_Cam = %.4f"),
					image_container.uav_camera_sight.yaw,
					image_container.uav_camera_sight.pitch,
					image_container.uav_camera_sight.roll,
					image_container.uav_camera_sight.CaptureCamFov,
					image_container.uav_camera_sight.MainCamFov);
				SaveMetaData(TEXT("Camera_Viewing_Angles"), TelemetrySubfolder, image_container.idx, MetaString);

			}
			
		}
		/* In order to sleep, we will account for how much this tick took due to sending and receiving */
		FDateTime timeEndOfTick = FDateTime::UtcNow();
		FTimespan tickDuration = timeEndOfTick - timeBeginningOfTick;
		float secondsThisTickTook = tickDuration.GetTotalSeconds();
		float timeToSleep = TimeBetweenTicks - secondsThisTickTook;
		
		if (timeToSleep > 0.f)
		{
			FPlatformProcess::Sleep(timeToSleep);
		}
		
	}

	ConnectionShutdown();

	return 0;
}

void FVF_FrameSenderWorker::Stop()
{
	bRun = false;
}

void FVF_FrameSenderWorker::Exit()
{

}

void FVF_FrameSenderWorker::ConnectionShutdown()
{
	CloseSend();
}
