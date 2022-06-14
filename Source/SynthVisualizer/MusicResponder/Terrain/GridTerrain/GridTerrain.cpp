#include "GridTerrain.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "SynthVisualizer/MusicController/MusicController.h"
#include "Kismet/KismetMathLibrary.h"

AGridTerrain::AGridTerrain()
{
	terrainMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Terrain Mesh"));
	if (terrainMesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("(%s): Failed to initialize. Missing terrain mesh."), *GetName());
		SetActorTickEnabled(false);
	}
}

void AGridTerrain::InitializeMusicResponder()
{
	dynamicMaterial = terrainMesh->CreateDynamicMaterialInstance(0, terrainMesh->GetMaterial(0));

	FMaterialParameterInfo paramInfo = FMaterialParameterInfo("EmissiveBrightness");
	dynamicMaterial->GetScalarParameterValue(paramInfo, initialBrightness);

	if (EnableTerrainPanning)
	{
		if (Camera == nullptr || !Camera->IsValidLowLevel())
		{
			EnableTerrainPanning = false;
		}
	}
}

void AGridTerrain::OnTrackStart()
{
	if (!EnableTerrainPanning) return;
	
	UE_LOG(LogTemp, Log, TEXT("Grid Terrain (%s): OnTrackPlay. Setting intial position."), *GetName());
	FVector startPos = FMath::Lerp(InitialPositionRelativeToCamera, FinalPositionRelativeToCamera, musicController->GetCurrentSongPercent());
	SetActorLocation(UKismetMathLibrary::TransformLocation(Camera->GetTransform(), startPos));
}

void AGridTerrain::Tick(float DeltaTime)
{
	DoTerrainPanningLogic(DeltaTime);
	UpdateTerrainMaterial();	
}

void AGridTerrain::CaptureInitialPanPosition()
{
	if (Camera == nullptr || !Camera->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("(%s): Couldn't capture initial pan position. Missing camera reference."), *GetName());
		return;
	}

	InitialPositionRelativeToCamera = UKismetMathLibrary::InverseTransformLocation(Camera->GetTransform(), GetActorLocation());
}

void AGridTerrain::CaptureFinalPanPosition()
{
	if (Camera == nullptr || !Camera->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("(%s): Couldn't capture initial pan position. Missing camera reference."), *GetName());
		return;
	}

	FinalPositionRelativeToCamera = UKismetMathLibrary::InverseTransformLocation(Camera->GetTransform(), GetActorLocation());
}


void AGridTerrain::DoTerrainPanningLogic(float DeltaTime)
{
	if (!EnableTerrainPanning) return;

	float currentSongPercent = musicController->GetCurrentSongPercent();
	FVector targetPositionLS = FMath::Lerp(InitialPositionRelativeToCamera, FinalPositionRelativeToCamera, currentSongPercent);
	FVector targetPositionWS = UKismetMathLibrary::TransformLocation(Camera->GetTransform(), targetPositionLS);
	FVector smoothPosition = FMath::Lerp(GetActorLocation(), targetPositionWS, PanSmoothLerp);
	SetActorLocation(smoothPosition);
}


void AGridTerrain::UpdateTerrainMaterial()
{
	float bassSignal = musicController->EvaluateNormalizedSpectrum(BassResponse.frequencyTune, BassResponse.trackName);
	float verseSignal = musicController->EvaluateNormalizedSpectrum(LeadVerseResponse.frequencyTune, LeadVerseResponse.trackName);
	float chorusSignal = musicController->EvaluateNormalizedSpectrum(LeadChorusResponse.frequencyTune, LeadChorusResponse.trackName);
	float outroSignal = musicController->EvaluateNormalizedSpectrum(OutroResponse.frequencyTune, OutroResponse.trackName);
	dynamicMaterial->SetScalarParameterValue("BassSignal", bassSignal);
	dynamicMaterial->SetScalarParameterValue("VerseSignal", verseSignal);
	dynamicMaterial->SetScalarParameterValue("ChorusSignal", chorusSignal);
	dynamicMaterial->SetScalarParameterValue("OutroSignal", outroSignal);
}