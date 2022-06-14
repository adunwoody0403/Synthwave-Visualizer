#pragma once

#include "CoreMinimal.h"
#include "SynthVisualizer/MusicResponder/MusicResponder.h"
#include "SynthSun.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class SYNTHVISUALIZER_API ASynthSun : public AMusicResponder
{
	GENERATED_BODY()

public:
	ASynthSun();

	// Music Responder
	virtual void InitializeMusicResponder() override;
	
	// Actor
	virtual void Tick(float DeltaTime) override;

	// Synth Sun
	UFUNCTION(BlueprintCallable, Category = "Synth Sun")
	void SetBandPanningEnabled(bool panningEnabled);

private:

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Synth Sun", meta = (ClampMin = "0.1", ClampMax = "1000"))
	float maxScale = 0.5f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Synth Sun")
	FTrackResponse ScaleResponse;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Synth Sun")
	FTrackResponse BrightnessResponse;

	UPROPERTY(EditDefaultsOnly, Category = "Synth Sun")
	UStaticMeshComponent* sunMesh;
	UPROPERTY(EditAnywhere, Category = "Synth Sun")
	TSubclassOf<UMaterialInstanceDynamic> terrainMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* dynamicMaterial;
	UPROPERTY()
	float initialScale; 
};