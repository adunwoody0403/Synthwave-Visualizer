// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicResponder.generated.h"

class AMusicController;

USTRUCT(BlueprintType)
struct FTrackResponse
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Music Response")
	FName trackName;
	UPROPERTY(EditAnywhere, Category = "Music Response", meta = (ClampMin="0", ClampMax="1"))
	float frequencyTune;

	FTrackResponse()
	{
		trackName = FName("None");
		frequencyTune = 0.0f;
	}
};

UCLASS(BlueprintType, Blueprintable)
class SYNTHVISUALIZER_API AMusicResponder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicResponder();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Music Responder Interface
	UFUNCTION()
	virtual void InitializeMusicResponder() {};
	UFUNCTION()
	virtual void OnTrackStart() {};
	UFUNCTION()
	virtual void OnTrackEnd() {};

public:

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Music Responder")
	AMusicController* musicController;
};
