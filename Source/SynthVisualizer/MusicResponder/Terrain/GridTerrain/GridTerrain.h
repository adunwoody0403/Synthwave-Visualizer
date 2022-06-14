#pragma once

#include "CoreMinimal.h"
#include "SynthVisualizer/MusicResponder/MusicResponder.h"
#include "GridTerrain.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class SYNTHVISUALIZER_API AGridTerrain : public AMusicResponder
{
	GENERATED_BODY()

public:
	AGridTerrain();


	// Actor
	virtual void Tick(float DeltaTime) override;

	// Blueprint
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Grin Terrain Panning")
	void CaptureInitialPanPosition();
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Grin Terrain Panning")
	void CaptureFinalPanPosition();

protected:
	// Music Responder
	virtual void InitializeMusicResponder() override;
	virtual void OnTrackStart() override;

private:
	void DoTerrainPanningLogic(float DeltaTime);
	void UpdateTerrainMaterial();

protected:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain", meta = (ClampMin = "0.1", ClampMax = "1000"))
	float maxBrightness = 2.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain")
	FTrackResponse BassResponse;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain")
	FTrackResponse LeadVerseResponse;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain")
	FTrackResponse LeadChorusResponse;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain")
	FTrackResponse OutroResponse;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain Panning")
	bool EnableTerrainPanning;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain Panning", meta = (EditCondition = "EnableTerrainPanning", ClampMin = "0", ClampMax = "1"))
	float PanSmoothLerp = 0.01f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain Panning", meta = (EditCondition = "EnableTerrainPanning"))
	FVector InitialPositionRelativeToCamera;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Terrain Panning", meta = (EditCondition = "EnableTerrainPanning"))
	FVector FinalPositionRelativeToCamera;
	UPROPERTY(EditInstanceOnly, Category = "Grid Terrain Panning", meta = (EditCondition = "EnableTerrainPanning"))
	AActor* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Grid Terrain")
	UStaticMeshComponent* terrainMesh;
	UPROPERTY(VisibleInstanceOnly, Category = "Grid Terrain")
	TSubclassOf<UMaterialInstanceDynamic> terrainMaterial;

private:
	UMaterialInstanceDynamic* dynamicMaterial;
	float initialBrightness;
};