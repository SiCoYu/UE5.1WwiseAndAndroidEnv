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


#include "WwiseCodeExampleActor.h"

#include "AkAudioEvent.h"
#include "AkComponent.h"
#include "AkSwitchValue.h"
#include "Wwise/API/WwiseSoundEngineAPI.h"

// Sets default values
AWwiseCodeExampleActor::AWwiseCodeExampleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//The Actor needs an AkComponent attached if we want sound to be emitted from it.
	AkAudioComponent = CreateDefaultSubobject<UAkComponent>(TEXT("CppCreatedAKComponent"));
	AkAudioComponent->MoveComponent(GetActorLocation() - AkAudioComponent->GetPosition(), GetActorRotation(), false);
	AkAudioComponent->SetupAttachment(GetDefaultAttachComponent());
}

// Called when the game starts or when spawned
void AWwiseCodeExampleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWwiseCodeExampleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWwiseCodeExampleActor::PostEventSoundEngine(UAkAudioEvent* Event)
{
	if (auto* SoundEngine = IWwiseSoundEngineAPI::Get())
	{
		if(Event)
		{
			//PostEvent doesn't load the event. The Event needs to be loaded beforehand.
			if (!Event->IsLoaded())
			{
				Event->LoadData();
			}
			SoundEngine->PostEvent(Event->GetShortID(), AkAudioComponent->GetAkGameObjectID());
		}
	}
}

void AWwiseCodeExampleActor::SetSwitchSoundEngine(UAkSwitchValue* SwitchValue)
{
	if (auto* SoundEngine = IWwiseSoundEngineAPI::Get())
	{
		if(SwitchValue)
		{
			SoundEngine->SetSwitch(SwitchValue->GetGroupID(), SwitchValue->GetWwiseShortID(), AkAudioComponent->GetAkGameObjectID());
		}
	}
}

void AWwiseCodeExampleActor::PostEventAkAudio(UAkAudioEvent* Event)
{
	if (auto* AkAudio = FAkAudioDevice::Get())
	{
		if (Event)
		{
			AkAudio->PostEventOnActor(Event->GetWwiseShortID(), this);
		}
	}
}

void AWwiseCodeExampleActor::SetSwitchAkAudio(UAkSwitchValue* SwitchValue)
{
	if (auto* AkAudio = FAkAudioDevice::Get())
	{
		if (SwitchValue)
		{
			AkAudio->SetSwitch(SwitchValue->GetGroupID(), SwitchValue->GetWwiseShortID(), this);
		}
	}
}
