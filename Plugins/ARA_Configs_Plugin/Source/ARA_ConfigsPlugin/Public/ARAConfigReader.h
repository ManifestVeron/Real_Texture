// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "ConfigReader.h"
#include "ARA_ConfigParams.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARAConfigReader.generated.h"

using namespace std;

/**
 * 
 */
UCLASS()
class ARA_CONFIGSPLUGIN_API UARAConfigReader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	*	For config to RMD_VKA stand
	*/
	UFUNCTION(BluePrintCallable, Category = "ARA Configs | RMD_VKA Parameters")
		static void LoadARA_UDPConfig(int& recv_port, FString& sender_ip, int& sender_port);

	/*
	*	For config to Dynamic stand
	*/
	UFUNCTION(BluePrintCallable, Category = "Parameters Loaders")
		static void LoadCommonARAParams
		(
			FARAConnectionParams& math_model_connection_params,
			FARAConnectionParams& ready_signal_connection_params, 
			FARAConnectionParams& env_serv_connection_params,
			FARAConnectionParams& ipoint_peeper_connection_params,
			FARA_VFSenderParams& vf_fs_connection_params,
			FARAImageCaptureParams& img_capture_params,
			FARAResizeParams& rt_resize_params,
			FARAResizeParams& post_capture_resize_params,
			FARASceneRenderingParams& scene_rendering_params,
			FARAInteractiveMarkupParams& interactive_mrkp_params,
			FARADopParams& dop_params
		);

private:
	static void check_connection_params(FParametersList params, FString& res_ip, int32& res_port);
	static FARA_VFSenderParams check_vf_sender_params(FParametersList params);
	static FARAImageCaptureParams check_img_capturing_params(FParametersList params);
	static FARASceneRenderingParams check_scene_rendering_params(FParametersList params);
	static FARAResizeParams check_resize_params(FParametersList params);
	static FARAInteractiveMarkupParams check_interactive_mrkp_params(FParametersList params);
	static FARADopParams check_dop_params(FParametersList params);
};
