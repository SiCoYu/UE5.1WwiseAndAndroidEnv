/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/


#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Dom/JsonObject.h"
#include "Sockets.h"

#include "AkAudioDevice.h"
#include "AkWaapiClient.h"

#include "AkTestCapture.generated.h"


/**
 *
 */
UCLASS()
class WWISEDEMOGAME_API UAkTestCapture : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "AutomatedTesting")
		static void StartTestCapture(FString TestName);

	UFUNCTION(BlueprintCallable, Category = "AutomatedTesting")
		static void ValidateAndStopTestCapture();

	UFUNCTION(BlueprintCallable, Category = "AutomatedTesting")
		static void SendTestsResultsAndQuit();

	static void StoreWwiseLog(AK::Monitor::ErrorCode in_errorCode, const AkOSChar* in_pszError, AK::Monitor::ErrorLevel in_errorLevel, AkPlayingID in_playingID, AkGameObjectID in_gameObjID);
	static FString GetPlatformPort();
	static TArray<TSharedPtr<FJsonValue>> WwiseLog;
	static TArray<TSharedPtr<FJsonValue>> ResultsArray;
	static FString Name;
	static bool AllTestsPassed;
};
