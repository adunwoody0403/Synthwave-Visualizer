#include "MusicUIController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AMusicUIController::AMusicUIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMusicUIController::BeginPlay()
{
	Super::BeginPlay();
	InitializeUI();
}

void AMusicUIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMusicUIController::BeginDestroy()
{
	Super::BeginDestroy();
}

void AMusicUIController::InitializeUI()
{
	//if (mainMusicUIWidget == nullptr || !mainMusicUIWidget->IsValidLowLevel())
	//{
	//	UE_LOG(LogTemp, Error, TEXT("(%s): Failed to initialize music ui. Invalid ui class."), *GetName());
	//	SetActorTickEnabled(false);
	//	return;
	//}
	//
	//UE_LOG(LogTemp, Log, TEXT("(%s): Initializing music UI..."), *GetName());
	//UUserWidget* widget = UUserWidget::CreateWidgetInstance(*UGameplayStatics::GetPlayerController(GetWorld(), 0), mainMusicUIWidget, FName("Music UI"));
	//widget->AddToViewport();
}