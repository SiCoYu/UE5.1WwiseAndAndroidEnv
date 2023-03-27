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


#include "AkTestCapture.h"

#include "Kismet/GameplayStatics.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Modules/ModuleManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#include "Wwise/API/WwiseMonitorAPI.h"

TArray<TSharedPtr<FJsonValue>> UAkTestCapture::WwiseLog;
TArray<TSharedPtr<FJsonValue>> UAkTestCapture::ResultsArray = TArray<TSharedPtr<FJsonValue>>();
FString UAkTestCapture::Name;
bool UAkTestCapture::AllTestsPassed;

void UAkTestCapture::StartTestCapture(FString TestName)
{
	WwiseLog = TArray<TSharedPtr<FJsonValue>>();
	Name = TestName;
	AllTestsPassed = true;

	//start capturing error log
	if (auto* Monitor = IWwiseMonitorAPI::Get())
	{
		Monitor->SetLocalOutput(AK::Monitor::ErrorLevel_All, StoreWwiseLog);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Starting Capture"));
}

void UAkTestCapture::StoreWwiseLog(AK::Monitor::ErrorCode in_errorCode, const AkOSChar* in_pszError, AK::Monitor::ErrorLevel in_errorLevel, AkPlayingID in_playingID, AkGameObjectID in_gameObjID)
{
	TSharedPtr<FJsonValueString> JSonString = MakeShareable(new FJsonValueString((in_errorLevel == AK::Monitor::ErrorLevel_Error ? TEXT("Error: ") : TEXT("")) + (FString)in_pszError));
	WwiseLog.Add(JSonString);
}

void UAkTestCapture::ValidateAndStopTestCapture()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Stoping Capture"));

	AkRtpcValue RTPCValue = 50;
	AK::SoundEngine::Query::RTPCValue_type RTPCType = AK::SoundEngine::Query::RTPCValue_Global;
	FAkAudioDevice::Get()->GetRTPCValue(TEXT("TestCurrentVolume"), AK_INVALID_GAME_OBJECT, 0, RTPCValue, RTPCType);

	//stop capturing error logs
	if (auto* Monitor = IWwiseMonitorAPI::Get())
	{
		Monitor->SetLocalOutput(AK::Monitor::ErrorLevel_All, 0);
	}

	TSharedPtr<FJsonObject> TestResult = MakeShareable<FJsonObject>(new FJsonObject);
	TSharedPtr<FJsonObject> TestResultWrapper = MakeShareable<FJsonObject>(new FJsonObject);

	FString command = FCommandLine::Get();
	if (command.Contains("-nosound"))
		TestResult->SetBoolField("isPassed", RTPCValue == 50 && RTPCType == AK::SoundEngine::Query::RTPCValue_Global);
	else
		TestResult->SetBoolField("isPassed", RTPCValue > -96.0f && RTPCValue != 50 && RTPCType == AK::SoundEngine::Query::RTPCValue_Global);

	TestResult->SetNumberField("volume", RTPCValue);
	TestResult->SetArrayField("wwiseEngineLog", WwiseLog);

	if (!TestResult->GetBoolField("isPassed"))
		AllTestsPassed = false;

	TestResultWrapper->SetObjectField(Name, TestResult);
	TSharedRef< FJsonValueObject > Result = MakeShareable(new FJsonValueObject(TestResultWrapper));

	ResultsArray.Add(Result);
}

FString UAkTestCapture::GetPlatformPort()
{
	FString platform = UGameplayStatics::GetPlatformName();
	// Fstring does not support switch comparisson
	if (platform == TEXT("Windows"))
		return "54321";
	if (platform == TEXT("PS4"))
		return "54322";
	if (platform == TEXT("XboxOne"))
		return "54323";
	if (platform == TEXT("Switch"))
		return "54324";
	if (platform == TEXT("Mac"))
		return "54325";
	if (platform == TEXT("Android"))
		return "54326";
	if (platform == TEXT("iOS"))
		return "54327";
	if (platform == TEXT("XboxOneGDK"))
		return "54328";
	if (platform == TEXT("PS5"))
		return "54329";
	if (platform == TEXT("XSX"))
		return "54330";
	if (platform == TEXT("WinGDK"))
		return "54331";
	return "";
}

void UAkTestCapture::SendTestsResultsAndQuit()
{
	FString Platform = UGameplayStatics::GetPlatformName();

	FString ResultServerIP = "";
	if (!FParse::Value(FCommandLine::Get(), TEXT("ResultServerIP"), ResultServerIP))
	{
		UE_LOG(LogTemp, Warning, TEXT("WwiseDemoGame: No -ResultServerIP argument given. Using default value 127.0.0.1"));
		ResultServerIP = TEXT("127.0.0.1");
	}

	FString Port = "";
	if (!FParse::Value(FCommandLine::Get(), TEXT("Port"), Port))
	{
		UE_LOG(LogTemp, Warning, TEXT("WwiseDemoGame: No -Port argument given. Using the default value for the platform."));
		Port = GetPlatformPort();
	}
	FString BuildConfig = "";

#if UE_BUILD_SHIPPING
	BuildConfig = Platform + " " + TEXT("Shipping");
#elif UE_BUILD_DEVELOPMENT
	BuildConfig = Platform + " " + TEXT("Development");
#else
	BuildConfig = Platform + " " + TEXT("Unknown config");
#endif

	FString ExtraParameters = "";
	if (FParse::Value(FCommandLine::Get(), TEXT("game"), ExtraParameters))
	{
		BuildConfig += " -game";
	}
	if (FParse::Value(FCommandLine::Get(), TEXT("nosound"), ExtraParameters))
	{
		BuildConfig += " -nosound";
	}

	TSharedPtr<FJsonObject> compiledResults = MakeShareable<FJsonObject>(new FJsonObject);
	compiledResults->SetBoolField("isPassed", AllTestsPassed);
	compiledResults->SetArrayField("results", ResultsArray);
	compiledResults->SetStringField("buildConfig", BuildConfig);

	FString outputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&outputString);
	FJsonSerializer::Serialize(compiledResults.ToSharedRef(), JsonWriter);

	FIPv4Address IpAddress;
	FIPv4Address::Parse(ResultServerIP, IpAddress);

	FString resultAddress = FString::Printf(TEXT("%s:%s"), *ResultServerIP, *Port);
	UE_LOG(LogTemp, Warning, TEXT("WwiseDemoGame: Attempting to connect to %s over tcp"), *resultAddress);

	FSocket* Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("results"), false);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(IpAddress.Value);
	addr->SetPort(FCString::Atoi(*Port));

	if (Socket->Connect(*addr))
	{
		UE_LOG(LogTemp, Warning, TEXT("WwiseDemoGame: Connected. Sending results: %s"), *outputString);
		FTCHARToUTF8 OutputStringUtf8(*outputString);
		int32 DestLen = OutputStringUtf8.Length();
		int32 BytesSent = 0;

		if (!Socket->Send((unsigned char*)(TCHAR_TO_UTF8(*outputString)), DestLen, BytesSent))
			UE_LOG(LogTemp, Warning, TEXT("WwiseDemoGame: Failed to send results"));
		FGenericPlatformMisc::RequestExit(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WwiseDemoGame: Failed to connect."));
	}

}
