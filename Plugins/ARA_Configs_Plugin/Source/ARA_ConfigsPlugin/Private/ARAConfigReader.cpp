// Fill out your copyright notice in the Description page of Project Settings.


#include "ARAConfigReader.h"

#include "Engine.h"

void UARAConfigReader::LoadARA_UDPConfig(int& recv_port, FString& sender_ip, int& sender_port)
{
	TMap<FString, FString> params;

	FString config_path = FPaths::Combine(*FPaths::ProjectConfigDir(), TEXT("UDP_params.cfg"));

	if (!UConfigReader::SimpleConfigLoader(params, config_path))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, (TEXT("%s"), *config_path));
		return;
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, (TEXT("%s"), *config_path));

	recv_port = 9891;
	FString frecv_port;

	if (!params.Contains("recv_port"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("recv_port value not found in config file. Default value have been set"));
	}
	else 
	{
		frecv_port = params["recv_port"];

		if (frecv_port.IsNumeric())
		{
			recv_port = FCString::Atoi(*frecv_port);
		}
	}

	sender_ip = TEXT("127.0.0.1");
	sender_port = 9791;
	FString fsender_port;

	if (!params.Contains("sender"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("sender parameter not found in config file. Default value have been set"));
	}
	else
	{
		params["sender"].Split(":", &sender_ip, &fsender_port);

		if (fsender_port.IsNumeric())
		{
			sender_port = FCString::Atoi(*fsender_port);
		}
	}
}

void UARAConfigReader::LoadCommonARAParams
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
)
{
	TMap<FString, FParametersList> params;

	FString config_path = FPaths::Combine(*FPaths::ProjectConfigDir(), TEXT("params.cfg"));

	if (!UConfigReader::ConfigLoader(params, config_path))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, (TEXT("%s"), *config_path));

		return;
	}
		
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, (TEXT("%s"), *config_path));

	FParametersList parameters_list;

	// Math Model Connection Parsing
	FString ip = TEXT("127.0.0.1");
	int32 port = 8541;

	if (!params.Contains("MATH_MODEL"))
	{	
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("MATH_MODEL section not found in config file. Default values have been set"));
			
		math_model_connection_params = FARAConnectionParams(ip, port);
	}
	else
	{
		parameters_list = params["MATH_MODEL"];
		check_connection_params(parameters_list, ip, port);
		math_model_connection_params = FARAConnectionParams(ip, port);
	}
	
	// Ready Signal (Math Model) connection parsing
	ip = TEXT("127.0.0.1");
	port = 30001;

	if (!params.Contains("READY_SIGNAL"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("READY_SIGNAL section not found in config file. Default values have been set"));

		ready_signal_connection_params = FARAConnectionParams(ip, port);
	}
	else
	{
		parameters_list = params["READY_SIGNAL"];
		check_connection_params(parameters_list, ip, port);
		ready_signal_connection_params = FARAConnectionParams(ip, port);
	}

	// Target Server connection parsing
	ip = TEXT("127.0.0.1");
	port = 8548;

	if (!params.Contains("ENV_SIM"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("ENV_SIM section not found in config file. Default values have been set"));
			
		env_serv_connection_params = FARAConnectionParams(ip, port);
	}
	else 
	{
		parameters_list = params["ENV_SIM"];
		check_connection_params(parameters_list, ip, port);
		env_serv_connection_params = FARAConnectionParams(ip, port);
	}

	// IPoint Peeper connection parsing
	ip = TEXT("230.0.0.1");
	port = 12346;

	if (!params.Contains("IPOINT_PEEPER"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("IPOINT_PEEPER section not found in config file. Default values have been set"));

		ipoint_peeper_connection_params = FARAConnectionParams(ip, port);
	}
	else
	{
		parameters_list = params["IPOINT_PEEPER"];
		check_connection_params(parameters_list, ip, port);
		ipoint_peeper_connection_params = FARAConnectionParams(ip, port);
	}

	// VS Server (to Special Calculator) connection parsing
	ip = TEXT("127.0.0.1");
	port = 1111;

	if (!params.Contains("VS_SERV"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("VS_SERV section not found in config file. Default values have been set"));
			
		vf_fs_connection_params = FARA_VFSenderParams(ip, port, 32000, 2);
	}
	else 
	{
		parameters_list = params["VS_SERV"];
		vf_fs_connection_params = check_vf_sender_params(parameters_list);
	}

	// Image Capturing params parsing
	bool bNeedSave = false;
	FString ImgFormat = TEXT("jpg");
	int32 ImgQuality = 0;

	if (!params.Contains("IMAGE_CAPTURING"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("IMAGE_CAPTURING section not found in config file. Default values have been set"));
			
		img_capture_params = FARAImageCaptureParams(bNeedSave, ImgFormat, ImgQuality);
	}
	else 
	{
		parameters_list = params["IMAGE_CAPTURING"];
		img_capture_params = check_img_capturing_params(parameters_list);
	}

	// RT Resize Params parsing
	if (!params.Contains("RT_RESIZE_PARAMS"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("RT_RESIZE_PARAMS section not found in config file. Default values have been set"));

		rt_resize_params = FARAResizeParams();
	}
	else
	{
		parameters_list = params["RT_RESIZE_PARAMS"];
		rt_resize_params = check_resize_params(parameters_list);
	}

	// Post Capture Resize Params
	if (!params.Contains("POST_CAPTURE_RESIZE_PARAMS"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("POST_CAPTURE_RESIZE_PARAMS section not found in config file. Default values have been set"));

		post_capture_resize_params = FARAResizeParams();
	}
	else
	{
		parameters_list = params["POST_CAPTURE_RESIZE_PARAMS"];
		post_capture_resize_params = check_resize_params(parameters_list);
	}

	// Scene Rendering params Parsing
	if (!params.Contains("SCENE_RENDERING"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("SCENE_RENDERING section not found in config file. Default values have been set"));

		scene_rendering_params = FARASceneRenderingParams();
	}
	else
	{
		parameters_list = params["SCENE_RENDERING"];
		scene_rendering_params = check_scene_rendering_params(parameters_list);
	}

	// Interactive Markup params Parsing
	if (!params.Contains("INTERACTIVE_MARKUP"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("INTERACTIVE_MARKUP section not found in config file. Default values have been set"));

		interactive_mrkp_params = FARAInteractiveMarkupParams();
	}
	else 
	{
		parameters_list = params["INTERACTIVE_MARKUP"];
		interactive_mrkp_params = check_interactive_mrkp_params(parameters_list);
	}

	// Dop Params Parsing
	if (!params.Contains("DOP_PARAMS"))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("DOP_PARAMS section not found in config file. Default values have been set"));

		dop_params = FARADopParams();
	}
	else
	{
		parameters_list = params["DOP_PARAMS"];
		dop_params = check_dop_params(parameters_list);
	}
	
}

void UARAConfigReader::check_connection_params(FParametersList params, FString& res_ip, int32& res_port)
{
	for (auto& parameter : params.parameters)
	{
		if (parameter.Name == TEXT("IP"))
		{
			res_ip = parameter.Value;
		}
		else if (parameter.Name == TEXT("PORT") && parameter.Value.IsNumeric())
		{
			res_port = FCString::Atoi(*parameter.Value);
		}
	}
}

FARA_VFSenderParams UARAConfigReader::check_vf_sender_params(FParametersList params)
{
	FString res_ip = "127.0.0.1";
	int32 res_port = 1111;
	int32 res_frameSize = 32000;
	int32 res_sendDelay = 0;

	for (auto& parameter : params.parameters)
	{
		if (parameter.Name == TEXT("IP"))
		{
			res_ip = parameter.Value;
		}
		else if (parameter.Name == TEXT("PORT") && parameter.Value.IsNumeric())
		{
			res_port = FCString::Atoi(*parameter.Value);
		}
		else if (parameter.Name == TEXT("FRAME_SIZE") && parameter.Value.IsNumeric())
		{
			res_frameSize = FCString::Atoi(*parameter.Value);
		}
		else if (parameter.Name == TEXT("SEND_DELAY") && parameter.Value.IsNumeric())
		{
			res_sendDelay = FCString::Atoi(*parameter.Value);
		}
	}

	return FARA_VFSenderParams(res_ip, res_port, res_frameSize, res_sendDelay);
}

FARAImageCaptureParams UARAConfigReader::check_img_capturing_params
(
	FParametersList params
)
{
	bool res_NeedSave = false;
	FString res_ImgFormat = TEXT("jpg");
	int32 res_ImgQuality = 0;
	float res_CustomDurationCapture = 0.0f;
	bool res_ActivateCustomDuration = false;

	for (auto& parameter : params.parameters)
	{
		if (parameter.Name == TEXT("IM_NeedSave"))
		{
			parameter.Value = parameter.Value.ToLower();
			if (parameter.Value == TEXT("yes"))
			{
				res_NeedSave = true;
			};
		}
		else if (parameter.Name == TEXT("IM_Format"))
		{
			res_ImgFormat = parameter.Value.ToLower();
		}
		else if (parameter.Name == TEXT("IM_Quality"))
		{
			wstring w_ImgQuality_string(&parameter.Value.GetCharArray()[0]);
			string s_ImgQuality_string(w_ImgQuality_string.begin(), w_ImgQuality_string.end());
			res_ImgQuality = stoi(s_ImgQuality_string);
		}
		else if (parameter.Name == TEXT("IM_CustomDurationCapture") && parameter.Value.IsNumeric())
		{
			res_CustomDurationCapture = FCString::Atof(*parameter.Value);
		}
		else if (parameter.Name == TEXT("IM_ActivateCustomDuration"))
		{
			parameter.Value = parameter.Value.ToLower();
			if (parameter.Value == TEXT("yes"))
			{
				res_ActivateCustomDuration = true;
			};
		}
	}

	return FARAImageCaptureParams(res_NeedSave, res_ImgFormat, res_ImgQuality,
		res_CustomDurationCapture, res_ActivateCustomDuration);
}

FARASceneRenderingParams UARAConfigReader::check_scene_rendering_params(FParametersList params)
{
	FARASceneRenderingParams scene_rendering_params;

	for (auto& parameter : params.parameters)
	{
		if (parameter.Name == TEXT("GrassType") && parameter.Value.IsNumeric())			
		{
			scene_rendering_params.GrassType= FCString::Atoi(*parameter.Value);
		}

		if (parameter.Name == TEXT("ForestType") && parameter.Value.IsNumeric())
		{
			scene_rendering_params.ForestType = FCString::Atoi(*parameter.Value);
		}

		if (parameter.Name == TEXT("StonesType") && parameter.Value.IsNumeric())
		{
			scene_rendering_params.StonesType = FCString::Atoi(*parameter.Value);
		}
	}
	
	return scene_rendering_params;
}

FARAResizeParams UARAConfigReader::check_resize_params(FParametersList params)
{
	int32 rt_width = 0;
	int32 rt_height = 0;
	bool bActivate = false;

	for (auto& parameter : params.parameters)
	{
		if (parameter.Name == TEXT("WIDTH") && parameter.Value.IsNumeric())
		{
			rt_width = FCString::Atoi(*parameter.Value);
		}

		if (parameter.Name == TEXT("HEIGHT") && parameter.Value.IsNumeric())
		{
			rt_height = FCString::Atoi(*parameter.Value);
		}

		if (parameter.Name == TEXT("ACTIVATE"))
		{
			parameter.Value = parameter.Value.ToLower();
			if (parameter.Value == TEXT("yes"))
			{
				bActivate = true;
			};
		}
	}

	return FARAResizeParams(rt_width, rt_height, bActivate);
}

FARAInteractiveMarkupParams UARAConfigReader::check_interactive_mrkp_params(FParametersList params)
{
	bool bNeedSave = false;
	FString MrkpFormat = TEXT("json");

	for (auto& parameter : params.parameters)
	{
		if (parameter.Name == TEXT("MRKP_NeedSave"))
		{
			parameter.Value = parameter.Value.ToLower();
			if (parameter.Value == TEXT("yes"))
			{
				bNeedSave = true;
			};
		}
		
		if (parameter.Name == TEXT("MRKP_Format"))
		{
			MrkpFormat = parameter.Value.ToLower();
		}		
	}

	return FARAInteractiveMarkupParams(bNeedSave, MrkpFormat);
}

FARADopParams UARAConfigReader::check_dop_params(FParametersList params)
{
	FARADopParams dop_params;

	for (auto& parameter : params.parameters)
	{
		if (parameter.Name == TEXT("BOARD_NUMBER"))			
		{
			dop_params.BoardNumber = parameter.Value;
		}

		if (parameter.Name == TEXT("USE_IPOINT_PEEPER"))
		{
			parameter.Value = parameter.Value.ToLower();
			if (parameter.Value == TEXT("yes"))
			{
				dop_params.bUseIPointPeeper = true;
			}
		}
	}
	
	return dop_params;
}
