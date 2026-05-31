#include "StudentPerceptorDanielAdamov.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	// Only zombie can damage survivor
	if (!Cast<ABaseZombie>( Actor ))
		return;
	
	FPerceivedActor* Record{ FindRecord( Zombies, Actor ) };
	// If no record of zombie-> populate the Record and push to TArray
	if (!Record)
	{
		Record = &Zombies.AddDefaulted_GetRef();
		Record->Actor = Actor;
	}
	if (Stimulus.WasSuccessfullySensed())
	{
		Record->LastKnownLocation = Stimulus.StimulusLocation;
	}
	
	Record->LastSeenTime = GetWorld()->GetTimeSeconds();
}

void UStudentPerceptorDanielAdamov::RefreshWorldMemory()
{
	const float TimeNow{ static_cast<float>(GetWorld()->GetTimeSeconds()) };
	
	// 1. Remove stagnant memory
	Items.RemoveAll( [](const FPerceivedItem& R)
	{
		return !R.Actor.IsValid() || R.Actor->IsHidden();
	});
	Zombies.RemoveAll([&](const FPerceivedActor& R)
	{
		return !R.Actor.IsValid() || (!R.bIsVisible && (TimeNow - R.LastSeenTime) > ThreatMemoryDuration);
	});
	Houses.RemoveAll([](const FPerceivedHouse& R)
	{
		return !R.Actor.IsValid();
	});
	
	// 2. Update Houses visited flag
	if (const AActor* Owner = GetOwner())
	{
		const FVector MyLocation{ Owner->GetActorLocation() };
		for (FPerceivedHouse& H : Houses)
		{
			if (!H.bVisited && FVector::Dist( MyLocation, H.LastKnownLocation ) <= HouseVisitedRange)
			{
				H.bVisited = true;
			}
		}
	}
	
	// 3. Write after refreshing
	WriteBlackboard();
}

AActor* UStudentPerceptorDanielAdamov::SelectThreat() const
{
	
}

AActor* UStudentPerceptorDanielAdamov::SelectTargetItem() const
{
}

AActor* UStudentPerceptorDanielAdamov::SelectHouseTarget() const
{
}

void UStudentPerceptorDanielAdamov::WriteBlackboard()
{
	UBlackboardComponent* BB{ GetBlackboard() };
	if (!BB)
		return;
	
	//BB->SetValueAsObject( SurvivorBBKeys::ThreatActor )
}

UBlackboardComponent* UStudentPerceptorDanielAdamov::GetBlackboard() const
{
	const APawn* Pawn{ Cast<APawn>( GetOwner() ) };
	const AAIController* AI{ Pawn ? Cast<AAIController>( Pawn->GetController() ) : nullptr };
	
	return AI ? AI->GetBlackboardComponent() : nullptr;
}

float UStudentPerceptorDanielAdamov::GetHealthPct() const noexcept
{
	const UHealthComponent* H{ GetOwner()->FindComponentByClass<UHealthComponent>() };
	const float HealthPct{ H->GetHealth() / FMath::Max(1.f, static_cast<float>(H->GetMaxHealth())) };
	return H ? HealthPct : 1.f;
}

float UStudentPerceptorDanielAdamov::GetStaminaPct() const noexcept
{
	const UStaminaComponent* S{ GetOwner()->FindComponentByClass<UStaminaComponent>() };
	const float StaminaPct{ S->GetCurrentStamina() / FMath::Max(1.f, static_cast<float>(S->GetMaxStamina())) };
	return S ? StaminaPct : 1.f;
}
