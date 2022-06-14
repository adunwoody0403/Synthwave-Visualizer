#pragma once

#include "CoreMinimal.h"
#include "SynthVisualizer/MusicResponder/MusicResponder.h"
#include "SpectrumBar.generated.h"

USTRUCT()
struct FSpectrumBarData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* bar;
	UPROPERTY()
	float frequencyIndex;

	FSpectrumBarData()
	{
		bar = nullptr;
		frequencyIndex = 0.0f;
	}

	FSpectrumBarData(AActor* bar, float frequencyIndex)
	{
		this->bar = bar;
		this->frequencyIndex = frequencyIndex;
	}
};

UCLASS(BlueprintType, Blueprintable)
class SYNTHVISUALIZER_API ASpectrumBar : public AMusicResponder
{
	GENERATED_BODY()

public:
	ASpectrumBar();

public:
	// Actor interface
	virtual void Tick(float DeltaTime) override;
	
	// Spectrum Bar Interface
	virtual void InitializeMusicResponder() override;

protected:


private:
	void SpawnBars();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectrum Bar")
	int32 NumberOfBars;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectrum Bar")
	TSubclassOf<AActor> BarTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectrum Bar")
	float BarDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectrum Bar")
	float Divisor = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectrum Bar")
	FName TrackToRespondTo;

private:

	TArray<FSpectrumBarData> bars;
};