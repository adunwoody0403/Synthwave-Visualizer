#include "SynthSun.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "SynthVisualizer/MusicController/MusicController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ASynthSun::ASynthSun()
{
	sunMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Sun Mesh"));
	if (sunMesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("(%s): Failed to initialize. Missing terrain mesh."), *GetName());
		SetActorTickEnabled(false);
	}
}

void ASynthSun::SetBandPanningEnabled(bool panningEnabled)
{
	if (dynamicMaterial == nullptr || !dynamicMaterial->IsValidLowLevel()) return;

	dynamicMaterial->SetScalarParameterValue(FName("PanFactor"), panningEnabled);
	dynamicMaterial->SetScalarParameterValue(FName("PanTimeOffset"), UGameplayStatics::GetTimeSeconds(GetWorld()));
}


void ASynthSun::InitializeMusicResponder()
{
	dynamicMaterial = sunMesh->CreateDynamicMaterialInstance(0, sunMesh->GetMaterial(0));
	initialScale = GetActorScale().X;
}

void ASynthSun::Tick(float DeltaTime)
{
	float scaleSignal = musicController->EvaluateNormalizedSpectrum(ScaleResponse.frequencyTune, ScaleResponse.trackName);
	float scale = FMath::Lerp(initialScale, initialScale * maxScale, scaleSignal);
	SetActorScale3D(FVector(scale, scale, scale));

	float brightnessSignal = musicController->EvaluateNormalizedSpectrum(BrightnessResponse.frequencyTune, BrightnessResponse.trackName);
	dynamicMaterial->SetScalarParameterValue("BrightnessSignal", brightnessSignal);
}