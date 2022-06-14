#pragma once

#include "CoreMinimal.h"
#include "SynthVisualizer/MusicResponder/MusicResponder.h"
#include "SynthSky.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class SYNTHVISUALIZER_API ASynthSky : public AMusicResponder
{
	GENERATED_BODY()

public:
	ASynthSky();

	// Music Responder
	virtual void InitializeMusicResponder() override;

	// Actor
	virtual void Tick(float DeltaTime) override;

private:

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Synth Sky")
	FTrackResponse BrightnessResponse;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Synth Sky")
	FName SkyBrightnessParam = FName("TrackSignal");

	UPROPERTY(EditDefaultsOnly, Category = "Synth Sky")
	UStaticMeshComponent* skyMesh;
	UPROPERTY(VisibleInstanceOnly, Category = "Synth Sky")
	TSubclassOf<UMaterialInstanceDynamic> skyMaterial;

private:
	UMaterialInstanceDynamic* dynamicMaterial;
	float initialBrightness;
};