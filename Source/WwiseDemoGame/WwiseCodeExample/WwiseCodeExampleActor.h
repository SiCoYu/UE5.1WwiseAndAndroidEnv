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
#include "GameFramework/Actor.h"
#include "WwiseCodeExampleActor.generated.h"

class UAkComponent;
class UAkAudioEvent;
class UAkSwitchValue;

UCLASS()
class WWISEDEMOGAME_API AWwiseCodeExampleActor : public AActor
{
	GENERATED_BODY()
	UAkComponent* AkAudioComponent;
public:	
	// Sets default values for this actor's properties
	AWwiseCodeExampleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PostEventSoundEngine(UAkAudioEvent* Event);

	UFUNCTION(BlueprintCallable)
	void PostEventAkAudio(UAkAudioEvent* Event);

	UFUNCTION(BlueprintCallable)
	void SetSwitchSoundEngine(UAkSwitchValue* SwitchValue);

	UFUNCTION(BlueprintCallable)
	void SetSwitchAkAudio(UAkSwitchValue* SwitchValue);

};