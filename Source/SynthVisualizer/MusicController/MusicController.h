// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundWave.h"
#include "MusicController.generated.h"

class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMusicControllerEvent);

USTRUCT(BlueprintType)
struct FTrackData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Synth Visualization Track Properties")
	FName trackID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Synth Visualization Track Properties")
	USoundWave* track;
	UPROPERTY(EditAnywhere, Category = "Synth Visualization Track Properties")
	int32 spectrumResolution;
	UPROPERTY(EditAnywhere, Category = "Synth Visualization Track Properties")
	float spectrumTimeSlice;
	UPROPERTY(EditAnywhere, Category = "Synth Visualization Track Properties")
	float spectrumClamp;
	UPROPERTY(EditAnywhere, Category = "Synth Visualization Track Properties")
	float spectrumPowerFactor;
	UPROPERTY(EditAnywhere, Category = "Synth Visualization Track Properties Debug")
	FColor trackColour;

	UPROPERTY(VisibleInstanceOnly, Category = "Synth Visualization Track Properties")
	bool isArmed;
	UPROPERTY(VisibleInstanceOnly, Category = "Synth Visualization Track Properties")
	float minFrequency;
	UPROPERTY(VisibleInstanceOnly, Category = "Synth Visualization Track Properties")
	float maxFrequency;
	UPROPERTY(VisibleInstanceOnly, Category = "Synth Visualization Track Properties")
	TArray<float> spectrum;
	UPROPERTY(VisibleInstanceOnly, Category = "Synth Visualization Track Properties")
	USoundWave* trackInstance;

	FTrackData();
	bool operator== (FTrackData data)
	{
		return (this->trackID == data.trackID);
	}

	FORCEINLINE explicit operator bool() const
	{
		return isArmed;
	}

	void ArmTrack(AMusicController* musicController, USoundWave* masterTrack = nullptr);
	void UpdateSpectrum(AMusicController* musicController);
	float EvaluateRawFrequency(float frequencyNormalized);
	float EvaluateClampedFrequency(float frequencyNormalized);
	float EvaluateNormalizedFrequency(float frequencyNormalized);
};

USTRUCT(BlueprintType)
struct FSong
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FTrackData masterTrack;
	UPROPERTY()
	TArray<FTrackData> detailTracks;

	FSong()
	{
		masterTrack = FTrackData();
		detailTracks = TArray<FTrackData>();
	}
};

UCLASS(BlueprintType, Blueprintable)
class SYNTHVISUALIZER_API AMusicController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicController();

	// Music Controller Public API
	UPROPERTY(BlueprintAssignable)
	FMusicControllerEvent OnTrackStart;
	UPROPERTY(BlueprintAssignable)
	FMusicControllerEvent OnTrackEnd;
	UPROPERTY(BlueprintAssignable)
	FMusicControllerEvent OnTrackPaused;

	// Music Controller Blueprint
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	void ArmTrackTest(FSong trackData);
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	void PlayTrack(float startPercent = 0.0f, float fadeInDuration = 0.0f);
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	void PauseTrack();
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	void ResumeTrack();
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	void StopTrack();
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	void ResetTrack();
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	float EvaluateNormalizedSpectrum(float normalizedFrequency, FName trackID);
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	float GetCurrentSongTime();
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	float GetCurrentSongPercent();
	UFUNCTION(BlueprintCallable, Category = "Synth Visualization Music Controller")
	float GetCurrentSongDuration();
	UFUNCTION(Blueprintcallable, Category = "Synth Visualization Music Controller")
	FString GetCurrentTrackTimeText();
	UFUNCTION(BlueprintImplementableEvent)
	TArray<float> CalculateFrequencySpectrum(USoundWave* track, float startTime, float timeLength, int32 spectrumResolution); // Currently have to do this in BP >:( because can't access SoundVisualizations library from c++ >:(    >:(    >:( 

	// Actor
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;
	//void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void Initialize();
	void ArmTrack();
	void DisarmTrack();
	void UpdateTrackState(float DeltaTime);
	void UpdateFrequencySpectrums();

	UFUNCTION()
	void UpdatePlaybackPercent(const USoundWave* playingSoundWave, const float playbackPercent);
	UFUNCTION()
	void OnAudioFinished();

	// Utils
	USoundWave* GetTrack(FName trackID);
	FTrackData* GetTrackData(FName trackID);

	// Debugging
	void DoDebugLogic();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music Controller")
	UAudioComponent* AudioComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Controller")
	FTrackData MasterTrack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Controller")
	TArray<FTrackData> detailTracks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Controller")
	bool PlayOnStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Controller", meta = (EditCondition="PlayOnStart", ClampMin = "0", ClampMax = "1"))
	float SongStartPercent;

	UPROPERTY(EditAnywhere, Category = "Music Controller Debugging")
	bool enableDebugging = false;
	UPROPERTY(EditAnywhere, Category = "Music Controller Debugging")
	float debugLineSegmentDistance = 200.0f;
	UPROPERTY(EditAnywhere, Category = "Music Controller Debugging")
	float debugLineSeperationDistance = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Music Controller Debugging")
	float debugLineThickness = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Music Controller Debugging")
	float debugFrequencyHeightScale;
	UPROPERTY(EditAnywhere, Category = "Music Controller Debugging", meta = (ClampMin="0", ClampMax="1"))
	float debugSpectrumPosition;

private:
	bool isArmed;
	bool isPlayingTrack;
	float songPercent;
	float songTime;
	float songDuration;
	float initialAudioTime;
	float initialPlaybackPercent;
	TMap<FName, FTrackData*> trackMap;
};
