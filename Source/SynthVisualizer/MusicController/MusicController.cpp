// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicController.h"
#include "Components/AudioComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "HAL/MemoryBase.h"

#include "DrawDebugHelpers.h"

FTrackData::FTrackData()
{
	trackID = FName("TrackID");
	track = nullptr;
	spectrumResolution = 64;
	spectrumTimeSlice = 0.1f;
	spectrumClamp = 60.0f;
	spectrumPowerFactor = 2.0f;
	isArmed = false;
	minFrequency = spectrumClamp;
	maxFrequency = -spectrumClamp;
	spectrum = TArray<float>();
	trackInstance = nullptr;
	trackColour = FColor::White;
	for (int i = 0; i < spectrumResolution; i++) spectrum.Add(-spectrumClamp);
}

void FTrackData::ArmTrack(AMusicController* musicController, USoundWave* masterTrack)
{
	isArmed = false;
	if (track == nullptr || !track->IsValidLowLevel()) return;

	trackInstance = track;
	spectrum.Empty();
	for (int i = 0; i < spectrumResolution; i++) spectrum.Add(-spectrumClamp);
	isArmed = true;

	if (masterTrack != nullptr)
	{
		if (trackInstance->GetDuration() != masterTrack->GetDuration())
		{
			UE_LOG(LogTemp, Warning, TEXT("(%s): Detail track (%s) duration (%f) does not match master track duration (%f)."), *(musicController->GetName()), *(trackID.ToString()), trackInstance->GetDuration(), masterTrack->GetDuration());
		}
	}
}

void FTrackData::UpdateSpectrum(AMusicController* musicController)
{
	if (!isArmed) return;

	spectrum = musicController->CalculateFrequencySpectrum(trackInstance, musicController->GetCurrentSongTime()/* + timeOffset*/, spectrumTimeSlice, spectrumResolution);

	for (int i = 0; i < /*spectrumResolution*/spectrum.Num(); i++)
	{
		float frequency = FMath::Clamp(spectrum[i], -spectrumClamp, spectrumClamp);
		if (frequency > maxFrequency)
		{
			maxFrequency = frequency;
		}

		if (frequency < minFrequency)
		{
			minFrequency = frequency;
		}
	}
}

float FTrackData::EvaluateRawFrequency(float frequencyNormalized)
{
	if (!isArmed) return -spectrumClamp;

	float freqNorm = FMath::Clamp(frequencyNormalized, 0.0f, 1.0f);
	float freq = freqNorm * (spectrumResolution - 1);
	int freqIndx = FMath::RoundToInt(freq);
	return spectrum[freqIndx];

	//float freq = FMath::Clamp<float>(frequencyNormalized, 0.0f, 1.0f);
	//int floorFreq = FMath::FloorToInt(freq);
	//int ceilFreq = FMath::CeilToInt(freq);

	//float frequencyValueA, frequencyValueB;
	//frequencyValueA = spectrum[floorFreq];
	//if (floorFreq == ceilFreq || ceilFreq >= 1)
	//{
	//	return frequencyValueA;
	//}
	//else
	//{
	//	frequencyValueB = spectrum[ceilFreq];
	//	float remainder = freq - (float)(floorFreq);
	//	return FMath::Lerp(frequencyValueA, frequencyValueB, remainder);
	//}
}

float FTrackData::EvaluateClampedFrequency(float frequencyRange)
{
	if (!isArmed) return 0.0f;

	return FMath::Clamp(EvaluateRawFrequency(frequencyRange), -spectrumClamp, spectrumClamp);
}

float FTrackData::EvaluateNormalizedFrequency(float frequencyRange)
{
	if (!isArmed) return 0.0f;

	//float clampedFrequency = EvaluateClampedFrequency(frequencyRange);
	//return (clampedFrequency - minFrequency) / (maxFrequency - minFrequency);
	float clampedFrequency = EvaluateClampedFrequency(frequencyRange);
	float normalizedFrequency = (clampedFrequency + spectrumClamp) / (2 * spectrumClamp);
	float powerFrequency = FMath::Pow(normalizedFrequency, spectrumPowerFactor);
	return powerFrequency;
}

// Sets default values
AMusicController::AMusicController()
{
	isArmed = false;
	isPlayingTrack = false;
	songPercent = 0.0f;
	songDuration = 0.0f;
	trackMap = TMap<FName, FTrackData*>();
	PrimaryActorTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("Audio Player"));
}

void AMusicController::ArmTrackTest(FSong trackData)
{

}

void AMusicController::PlayTrack(float startPercent, float fadeInDuration)
{
	if (!isArmed)
	{
		UE_LOG(LogTemp, Warning, TEXT("(%s): Trying to play a song but not armed!"), *GetName());
		return;
	}

	if (isPlayingTrack)
	{
		ResetTrack();
		return;
	}

	isPlayingTrack = true;
	songPercent = startPercent;
	songTime = songPercent * songDuration;
	initialAudioTime = UGameplayStatics::GetAudioTimeSeconds(GetWorld()) - songTime;
	initialPlaybackPercent = songPercent;

	AudioComponent->Sound = Cast<USoundBase>(MasterTrack.trackInstance);
	//AudioComponent->Play(songTime);
	AudioComponent->FadeIn(fadeInDuration, 1.0f, songTime, EAudioFaderCurve::Linear);
	OnTrackStart.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("(%s): Playing track..."), *GetName());
}

void AMusicController::PauseTrack()
{
	isPlayingTrack = false;
	AudioComponent->SetPaused(true);
	OnTrackPaused.Broadcast();
}

void AMusicController::ResumeTrack()
{
	if (!isArmed) return;

	isPlayingTrack = true;
	//AudioComponent->Play(songTime);
	AudioComponent->SetPaused(false);
}

void AMusicController::StopTrack()
{
	isPlayingTrack = false;
	songTime = 0.0f;
	songPercent = 0.0f;
	AudioComponent->Stop();
	OnTrackEnd.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("(%s): Track finished."), *GetName());
}

void AMusicController::ResetTrack()
{
	isPlayingTrack = false;
}

float AMusicController::EvaluateNormalizedSpectrum(float normalizedFrequency, FName trackID)
{
	if (!trackMap.Contains(trackID))
	{
		return 0.0f;
	}
	else
	{
		FTrackData* track = trackMap[trackID];
		return track->EvaluateNormalizedFrequency(normalizedFrequency);
	}
}

float AMusicController::GetCurrentSongTime()
{
	return songTime;
}

float AMusicController::GetCurrentSongPercent()
{
	return songPercent;
}

float AMusicController::GetCurrentSongDuration()
{
	return songDuration;
}

FString AMusicController::GetCurrentTrackTimeText()
{
	float processedTrackTime = songPercent * songDuration;
	int trackMinutes = FMath::FloorToInt(processedTrackTime / 60.0f);
	processedTrackTime = fmod(processedTrackTime, 60.0f);
	int trackSeconds = FMath::FloorToInt(processedTrackTime);
	processedTrackTime = fmod(processedTrackTime, 1.0f);
	int trackDeci = FMath::FloorToInt(processedTrackTime * 100.0f);
	return FString::Printf(TEXT("%02d : %02d : %02d"), trackMinutes, trackSeconds, trackDeci);
}

void AMusicController::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
	if (isArmed && PlayOnStart) PlayTrack(SongStartPercent);
}

void AMusicController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateTrackState(DeltaTime);
	if (enableDebugging) DoDebugLogic();
}


void AMusicController::BeginDestroy()
{
	if (IsRooted()) RemoveFromRoot();
	Super::BeginDestroy();
}

void AMusicController::Initialize()
{
	AudioComponent->OnAudioPlaybackPercent.AddDynamic(this, &AMusicController::UpdatePlaybackPercent);
	AudioComponent->OnAudioFinished.AddDynamic(this, &AMusicController::OnAudioFinished);
	ArmTrack();
}

void AMusicController::ArmTrack()
{
	if (isArmed) DisarmTrack();

	MasterTrack.ArmTrack(this);
	if (!MasterTrack.isArmed)
	{
		UE_LOG(LogTemp, Warning, TEXT("(%s): Failed to arm track. Couldn't arm master track."), *GetName());
		return;
	}

	songDuration = MasterTrack.trackInstance->GetDuration();
	trackMap.Add(TTuple<FName, FTrackData*>(MasterTrack.trackID, &MasterTrack));
	UE_LOG(LogTemp, Log, TEXT("(%s): Master track (%s) armed."), *GetName(), *(MasterTrack.trackID.ToString()));

	TArray<FTrackData> tracksToRemove = TArray<FTrackData>();
	for (int i = 0; i < detailTracks.Num(); i++)
	{
		if (trackMap.Contains(detailTracks[i].trackID))
		{
			UE_LOG(LogTemp, Warning, TEXT("(%s): Trying to arm track with duplicate track ID (%s)"), *GetName(), *(detailTracks[i].trackID.ToString()));
			tracksToRemove.Add(detailTracks[i]);
			continue;
		}

		detailTracks[i].ArmTrack(this, MasterTrack.trackInstance);
		if (!detailTracks[i].isArmed)
		{
			tracksToRemove.Add(detailTracks[i]);
			continue;
		}

		trackMap.Add(detailTracks[i].trackID, &detailTracks[i]);
		UE_LOG(LogTemp, Warning, TEXT("(%s): Armed detail track (%s)"), *GetName(), *(detailTracks[i].trackID.ToString()));
	}

	for (int i = 0; i < tracksToRemove.Num(); i++)
	{
		detailTracks.Remove(tracksToRemove[i]);
	}

	isArmed = true;
	UE_LOG(LogTemp, Log, TEXT("(%s): Track armed."), *GetName());
}

void AMusicController::DisarmTrack()
{
	if (!isArmed) return;
	trackMap.Empty();
	isArmed = false;
}

void AMusicController::UpdateTrackState(float DeltaTime)
{
	if (isPlayingTrack)
	{
		UpdateFrequencySpectrums();
	}
}

void AMusicController::UpdateFrequencySpectrums()
{
	if (MasterTrack.isArmed) MasterTrack.UpdateSpectrum(this);

	for (int i = 0; i < detailTracks.Num(); i++)
	{
		if (!detailTracks[i].isArmed) continue;
		detailTracks[i].UpdateSpectrum(this);
	}
}

void AMusicController::UpdatePlaybackPercent(const USoundWave* playingSoundWave, const float playbackPercent)
{
	songPercent = initialPlaybackPercent + playbackPercent;
	songTime = songPercent * songDuration;
}

void AMusicController::OnAudioFinished()
{
	StopTrack();
}

USoundWave* AMusicController::GetTrack(FName trackID)
{
	FTrackData* trackData = GetTrackData(trackID);
	if (trackData != nullptr)
	{
		return trackData->trackInstance;
	}
	else
	{
		return nullptr;
	}
}

FTrackData* AMusicController::GetTrackData(FName trackID)
{
	FTrackData** trackData = trackMap.Find(trackID);
	if (trackData == nullptr || *trackData == nullptr || !(*trackData)->isArmed)
	{
		return nullptr;
	}
	else
	{
		return *trackData;
	}
}

void AMusicController::DoDebugLogic()
{
	if (!isArmed || !isPlayingTrack) return;

	FVector startPos = GetActorLocation();
	for (int i = 1; i < MasterTrack.spectrumResolution; i++)
	{
		//float freq1 = FMath::Pow(MasterTrack.spectrum[i - 1], debugPowerFactor) * debugFrequencyHeightScale;
		//float freq2 = FMath::Pow(MasterTrack.spectrum[i], debugPowerFactor) * debugFrequencyHeightScale;
		float freq1Indx = (i - 1.0f) / (float)MasterTrack.spectrumResolution;
		float freq1 = MasterTrack.EvaluateNormalizedFrequency(freq1Indx) * debugFrequencyHeightScale;
		float freq2Indx = (float)i / (float)MasterTrack.spectrumResolution;
		float freq2 = MasterTrack.EvaluateNormalizedFrequency(freq2Indx) * debugFrequencyHeightScale;

		FVector pos1 = startPos + (FVector::ForwardVector * debugLineSegmentDistance * (i - 1) + (FVector::UpVector * freq1));
		FVector pos2 = startPos + (FVector::ForwardVector * debugLineSegmentDistance * (i)) + (FVector::UpVector * freq2);;
		DrawDebugLine(GetWorld(), pos1, pos2, MasterTrack.trackColour, false, 0.0f, 1.0f, debugLineThickness);

		if (FMath::Abs(freq1Indx - debugSpectrumPosition) <= (0.5f / MasterTrack.spectrumResolution))
		{
			DrawDebugLine(GetWorld(), pos1, startPos + (FVector::ForwardVector* debugLineSegmentDistance * (i-1)), MasterTrack.trackColour, false, 0.0f, 1.0f, debugLineThickness);
		}
	}

	for (int i = 0; i < detailTracks.Num(); i++)
	{
		if (!detailTracks[i].isArmed) continue;
		for (int j = 1; j < MasterTrack.spectrumResolution; j++)
		{
			//float freq1 = FMath::Pow(detailTracks[i].spectrum[j - 1], debugPowerFactor) * debugFrequencyHeightScale;
			//float freq2 = FMath::Pow(detailTracks[i].spectrum[j], debugPowerFactor) * debugFrequencyHeightScale;
			float freq1Indx = (j - 1.0f) / (float)detailTracks[i].spectrumResolution;
			float freq1 = detailTracks[i].EvaluateNormalizedFrequency(freq1Indx) * debugFrequencyHeightScale;
			float freq2Indx = (float)j / (float)detailTracks[i].spectrumResolution;
			float freq2 = detailTracks[i].EvaluateNormalizedFrequency(freq2Indx) * debugFrequencyHeightScale;

			FVector pos1 = startPos + (FVector::RightVector * debugLineSeperationDistance * (i + 1)) + (FVector::ForwardVector * debugLineSegmentDistance * (j - 1) + (FVector::UpVector * freq1));
			FVector pos2 = startPos + (FVector::RightVector * debugLineSeperationDistance * (i + 1)) + (FVector::ForwardVector * debugLineSegmentDistance * (j)) + (FVector::UpVector * freq2);;
			DrawDebugLine(GetWorld(), pos1, pos2, detailTracks[i].trackColour, false, 0.0f, 1.0f, debugLineThickness);

			if (FMath::Abs(freq1Indx - debugSpectrumPosition) <= (0.5f / detailTracks[i].spectrumResolution))
			{
				DrawDebugLine(GetWorld(), pos1, startPos + (FVector::RightVector * debugLineSeperationDistance * (i + 1)) + (FVector::ForwardVector* debugLineSegmentDistance * (j - 1)), detailTracks[i].trackColour, false, 0.0f, 1.0f, debugLineThickness);
			}
		}
	}

	DrawDebugLine(GetWorld(), startPos, startPos + (debugLineSegmentDistance * MasterTrack.spectrumResolution * FVector::ForwardVector), FColor::Black, false, 0.0f, 1.0f, debugLineThickness);
	DrawDebugLine(GetWorld(), startPos + (FVector::UpVector * debugFrequencyHeightScale), startPos + (debugLineSegmentDistance * MasterTrack.spectrumResolution * FVector::ForwardVector) + (FVector::UpVector * debugFrequencyHeightScale), FColor::Black, false, 0.0f, 1.0f, debugLineThickness);
	//DrawDebugLine(GetWorld(), startPos + (debugLineSegmentDistance * MasterTrack.spectrumResolution * debugSpectrumPosition * FVector::ForwardVector), startPos + (debugLineSegmentDistance * MasterTrack.spectrumResolution * debugSpectrumPosition * FVector::ForwardVector) + (FVector::UpVector * debugFrequencyHeightScale), FColor::Black, false, 0.0f, 1.0f, debugLineThickness);
}