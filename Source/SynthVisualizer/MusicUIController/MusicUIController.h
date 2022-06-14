#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicUIController.generated.h"

class UUserWidget;

UCLASS(BlueprintType)
class SYNTHVISUALIZER_API AMusicUIController : public AActor
{
	GENERATED_BODY()

public:
	AMusicUIController();

	// Actor
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;

protected:
	virtual void BeginPlay() override;

private:
	void InitializeUI();


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music UI Controller")
	TSubclassOf<UUserWidget> mainMusicUIWidget;

private:
};