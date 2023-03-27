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


#include "WwiseCodeExample.h"
#include "AkAudioEvent.h"
#include "AkComponent.h"
#include "Wwise/API/WwiseSoundEngineAPI.h"


void UWwiseCodeExample::PostEventSoundEngine(UAkAudioEvent* Event, AActor* GameObject)
{
	if (auto* SoundEngine = IWwiseSoundEngineAPI::Get())
	{
		if(Event && GameObject)
		{
			TArray<UAkComponent*> Array;
			GameObject->GetComponents<UAkComponent>(Array);
			//The Actor doesn't have an AkComponent attached. This component is needed if we want sound to be emitted from this actor.
			if (Array.Num() == 0)
			{
				//PostEvent doesn't load the event. The Event needs to be loaded beforehand.
				if (!Event->IsLoaded())
				{
					Event->LoadData();
				}

				auto* AkAudioComponent = NewObject<UAkComponent>(GameObject);
				AkAudioComponent->SetupAttachment(GameObject->GetDefaultAttachComponent());
				Array.Add(AkAudioComponent);
				AkAudioComponent->RegisterComponent();
			}

			SoundEngine->PostEvent(Event->GetWwiseShortID(), Array[0]->GetAkGameObjectID());
		}
	}
}

void UWwiseCodeExample::PostEventAkAudio(UAkAudioEvent* Event, AActor* GameObject)
{
	if(auto* AkAudio = FAkAudioDevice::Get())
	{
		if (Event && GameObject)
		{
			Event->PostOnActor(GameObject, {}, 0, true);
		}
	}

}