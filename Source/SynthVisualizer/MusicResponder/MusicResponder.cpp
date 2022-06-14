// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicResponder.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "SynthVisualizer/MusicController/MusicController.h"

// Sets default values
AMusicResponder::AMusicResponder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMusicResponder::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> musicControllers;
	UGameplayStatics::GetAllActorsOfClass(this, AMusicController::StaticClass(), musicControllers);

	if (musicControllers.Num() < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Music Responder (%s): Failed to initializer. Couldn't find music controller."), *GetName());
		SetActorTickEnabled(false);
	}

	musicController = Cast<AMusicController>(musicControllers[0]);
	if (musicController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Music Responder (%s): Failed to initializer. Failed to cast music controller?"), *GetName());
		SetActorTickEnabled(false);
	}

	musicController->OnTrackStart.AddDynamic(this, &AMusicResponder::OnTrackStart);
	musicController->OnTrackEnd.AddDynamic(this, &AMusicResponder::OnTrackEnd);

	InitializeMusicResponder();
	UE_LOG(LogTemp, Log, TEXT("Music Responder (%s): Initialized."), *GetName());
}

