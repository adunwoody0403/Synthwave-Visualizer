#include "SpectrumBar.h"
#include "SynthVisualizer/MusicController/MusicController.h"
#include "Engine/World.h"

ASpectrumBar::ASpectrumBar()
{

}

void ASpectrumBar::Tick(float DeltaTime)
{
	if (bars.Num() <= 0) return;

	for (int i = 0; i < bars.Num(); i++)
	{
		FSpectrumBarData data = bars[i];
		AActor* bar = data.bar;
		float frequencyIndex = data.frequencyIndex;
		float frequency = musicController->EvaluateNormalizedSpectrum(frequencyIndex, TrackToRespondTo);
		bar->SetActorScale3D(FVector(1.0f, 1.0f, (frequency / Divisor)));
	}
}

void ASpectrumBar::InitializeMusicResponder()
{
	SpawnBars();
}

void ASpectrumBar::SpawnBars()
{
	UE_LOG(LogTemp, Log, TEXT("Spectrum Bar (%s): Spawning %d bars..."), *GetName(), NumberOfBars);
	for (int i = 0; i < NumberOfBars; i++)
	{
		UWorld* world = GetWorld();
		FVector spawnLocation = GetActorLocation() + (FVector::ForwardVector * BarDistance * i);
		FRotator spawnRotation = GetActorRotation();
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* barActor = world->SpawnActor <AActor>(BarTemplate, spawnLocation, spawnRotation, params);
		//barActor = world->SpawnActor<AActor>(BarTemplate->StaticClass(), , FTransform())
		//AActor* barActor = NewObject<AActor>(this, BarTemplate);// FString::Printf("Bar %d", i));
		FSpectrumBarData barData = FSpectrumBarData(barActor, i / NumberOfBars);
		bars.Add(barData);
	}
}