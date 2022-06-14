#include "SynthSky.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "SynthVisualizer/MusicController/MusicController.h"

ASynthSky::ASynthSky()
{
	skyMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Terrain Mesh"));
	if (skyMesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("(%s): Failed to initialize. Missing terrain mesh."), *GetName());
		SetActorTickEnabled(false);
	}
}

void ASynthSky::InitializeMusicResponder()
{
	dynamicMaterial = skyMesh->CreateDynamicMaterialInstance(0, skyMesh->GetMaterial(0));
}

void ASynthSky::Tick(float DeltaTime)
{
	float currentNormalizedFrequency = 0.0f;
	currentNormalizedFrequency = musicController->EvaluateNormalizedSpectrum(BrightnessResponse.frequencyTune, BrightnessResponse.trackName);
	dynamicMaterial->SetScalarParameterValue(SkyBrightnessParam, currentNormalizedFrequency);
}