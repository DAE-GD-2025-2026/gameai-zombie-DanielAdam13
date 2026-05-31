#include "StudentPerceptorDanielAdamov.h"

#include "Items/BaseItem.h"
#include "Village/House/House.h"
#include "Zombies/BaseZombie.h"

// Names must match the BB keys in editor
namespace SurvivorBBKeys
{
	static const FName ThreatActor { TEXT("ThreatActor") };
	static const FName TargetItem { TEXT("TargetItem") };
	static const FName KnownHouseTarget { TEXT("KnownHouseTarget") };
}

UStudentPerceptorDanielAdamov::UStudentPerceptorDanielAdamov()
{
	PrimaryComponentTick.bCanEverTick = false; // Timer driven, no per-frame logic
}

void UStudentPerceptorDanielAdamov::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptorDanielAdamov::OnPerceptionUpdated);
	}
	
	// Start by refreshing memory
	GetWorld()->GetTimerManager().SetTimer(
		RefreshTimer, this, &UStudentPerceptorDanielAdamov::RefreshWorldMemory, RefreshInterval, true);
}

void UStudentPerceptorDanielAdamov::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshTimer);
	}
	Super::EndPlay( EndPlayReason );
}

void UStudentPerceptorDanielAdamov::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
		return;
	
	// Damage 
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		HandleDamage( Actor, Stimulus );
		
		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Red, 
	FString::Printf(TEXT("Hit Something!")));
	}
	else // Sight
	{
		HandleSight( Actor, Stimulus );
		
		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, 
	FString::Printf(TEXT("Saw Something!")));
	}
	
	// REACT to new sightings now at this frame
	WriteBlackboard();
}

void UStudentPerceptorDanielAdamov::HandleSight(AActor* Actor, const FAIStimulus& Stimulus)
{
	const bool bSensed{ Stimulus.WasSuccessfullySensed() };
	const float TimeNow{ static_cast<float>(GetWorld()->GetTimeSeconds()) };
	
	// If actor is zombie -> save info
	if (Cast<ABaseZombie>( Actor ))
	{
		FPerceivedActor* Record{ FindRecord( Zombies, Actor ) };
		// If no record of zombie-> populate the Record and push to TArray
		if (!Record)
		{
			Record = &Zombies.AddDefaulted_GetRef();
			Record->Actor = Actor;
		}
		
		Record->bIsVisible = bSensed;
		Record->LastKnownLocation = bSensed ? Actor->GetActorLocation() : Stimulus.StimulusLocation;
		Record->LastSeenTime = TimeNow;
		
		return;
	}
	
	// If actor is an Item
	if (ABaseItem* Item = Cast<ABaseItem>(Actor))
	{
		// Don't remember Garbage !!!
		if ( Item->GetItemType() == EItemType::Garbage)
			return;
		
		FPerceivedItem* Record{ FindRecord( Items, Actor ) };
		// If no record of item-> populate the Record and push to TArray
		if (!Record)
		{
			Record = &Items.AddDefaulted_GetRef();
			Record->Actor = Actor;
		}
		
		Record->Type = Item->GetItemType(); // -- Unique for Item --
		Record->bIsVisible = bSensed;
		Record->LastKnownLocation = Item->GetActorLocation();
		Record->LastSeenTime = TimeNow;
		
		return;
	}

	// If actor is a House
	if (Cast<AHouse>(Actor))
	{
		FPerceivedHouse* Record{ FindRecord(Houses, Actor) };
		// If no record of House-> populate the Record and push to TArray
		if (!Record)
		{
			Record = &Houses.AddDefaulted_GetRef();
			Record->Actor = Actor;
		}
		
		Record->bIsVisible = bSensed;
		Record->LastKnownLocation = Actor->GetActorLocation();
		Record->LastSeenTime = TimeNow;
		
		return;
	}
}

void UStudentPerceptorDanielAdamov::HandleDamage(AActor* Actor, const FAIStimulus& Stimulus)
{
}

void UStudentPerceptorDanielAdamov::RefreshWorldMemory()
{
}

void UStudentPerceptorDanielAdamov::WriteBlackboard()
{
}
