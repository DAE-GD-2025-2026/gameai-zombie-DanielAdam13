#include "StudentPerceptorDanielAdamov.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "PurgeZones/PurgeZone.h"
#include "Village/House/House.h"
#include "Zombies/BaseZombie.h"

// Names must match the BB keys in editor
namespace SurvivorBBKeys
{
	static const FName ThreatActor { TEXT("ThreatActor") };
	static const FName TargetItem { TEXT("TargetItem") };
	static const FName KnownHouseTarget { TEXT("KnownHouseTarget") };
	static const FName PurgeToAvoid { TEXT("PurgeToAvoid") };
	static const FName ThreatSpeed { TEXT("ThreatSpeed") };
	static const FName NearCount { TEXT("NearCount") };
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
	
	// If actor is a purge zone
	if (Cast<APurgeZone>( Actor ))
	{
		FPerceivedActor* Record{ FindRecord( PurgeZones, Actor ) };
		// If no record of zone-> populate the Record and push to TArray
		if (!Record)
		{
			Record = &PurgeZones.AddDefaulted_GetRef();
			Record->Actor = Actor;
		}
		
		Record->bIsVisible = bSensed;
		Record->LastKnownLocation = bSensed ? Actor->GetActorLocation() : Stimulus.StimulusLocation;
		Record->LastSeenTime = TimeNow;
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
	
	// 1. Remove stagnant memory (zombies are forgotten AFTER A DURATION)
	Items.RemoveAll( [](const FPerceivedItem& R)
	{
		return !R.Actor.IsValid() || R.Actor->IsHidden();
	});
	Zombies.RemoveAll([&](const FPerceivedActor& R) // zombies are forgotten AFTER A DURATION
	{
		return !R.Actor.IsValid() || (!R.bIsVisible && (TimeNow - R.LastSeenTime) > ThreatMemoryDuration);
	});
	Houses.RemoveAll([](const FPerceivedHouse& R)
	{
		return !R.Actor.IsValid();
	});
	PurgeZones.RemoveAll([](const FPerceivedActor& R)
	{
		return !R.Actor.IsValid();
	});
	
	FVector MyLocation{};
	// 2. Update Houses visited flag
	if (const AActor* Owner = GetOwner())
	{
		MyLocation = Owner->GetActorLocation();
	}
	
	// Essential for House Investigate Task:
	for (FPerceivedHouse& H : Houses)
	{
		if (!H.bVisited && FVector::Dist2D( MyLocation, H.LastKnownLocation ) <= HouseVisitedRange)
		{
			H.bVisited = true;
		}
	}
	
	// for (FPerceivedActor& Z : Zombies)
	// {
	// 	if (Z.Actor.IsValid() && Z.bIsVisible)
	// 	{
	// 		Z.MaxObservedSpeed = FMath::Max( Z.MaxObservedSpeed, Z.Actor->GetVelocity().Size2D() );
	// 	}
	// }
	
	// !!! Stuck bug when inside house visited range but MoveTo reporting success when out of house !!!
	UpdateStuckGuard( MyLocation );
	
	// 3. Write after refreshing
	WriteBlackboard();
}

AActor* UStudentPerceptorDanielAdamov::SelectThreat() const
{
	const FVector MyLocation{ GetOwner()->GetActorLocation() };
	const float TimeNow{ static_cast<float>(GetWorld()->GetTimeSeconds()) };
	
	// Calculate the NEAREST ZOMBIE which is currently visible and is still in memory
	AActor* BestZombie{ nullptr };
	float BestDistSqr{ TNumericLimits<float>::Max() };
	for (const FPerceivedActor& Z : Zombies)
	{
		if (!Z.Actor.IsValid())
			continue;
		// Skip if Zombie is not currently visible and exceeded memory duration
		if (!Z.bIsVisible && (TimeNow - Z.LastSeenTime) > ThreatMemoryDuration)
			continue;
		
		const float DistSqr{ static_cast<float>(FVector::DistSquared( MyLocation, Z.LastKnownLocation )) };
		if (DistSqr < BestDistSqr)
		{
			BestDistSqr = DistSqr;
			BestZombie = Z.Actor.Get();
		}
	}
	
	return BestZombie;
}

AActor* UStudentPerceptorDanielAdamov::SelectTargetItem() const
{
	const FVector MyLocation{ GetOwner()->GetActorLocation() };
	
	// Most interesting/complicated decision-making logic:
	// Best Item depends on situation
	AActor* BestItem{ nullptr };
	int32 BestPriority{ 0 }; // 0 means not worth
	float BestDistSqr{ TNumericLimits<float>::Max() }; 	// Priority comes first, Distance is secondary 
	
	for (const FPerceivedItem& PerItem : Items)
	{
		if (!PerItem.Actor.IsValid() || PerItem.Actor->IsHidden())
			continue;
		const ABaseItem* Item{ Cast<ABaseItem>( PerItem.Actor.Get() ) };
		if (!Item)
			continue;
		
		// Skip if not worth at all
		const int32 Priority{ GetItemPriority( PerItem.Type, Item->GetValue() ) };
		if (Priority <= 0)
			continue;
		
		// Priority comes first, Distance is secondary 
		const float DistSqr{ static_cast<float>(FVector::DistSquared( MyLocation, PerItem.LastKnownLocation )) };
		if (Priority > BestPriority || (Priority == BestPriority && DistSqr < BestDistSqr))
		{
			BestPriority = Priority;
			BestDistSqr = DistSqr;
			BestItem = PerItem.Actor.Get();
		}
	}
	
	return BestItem;
}

int32 UStudentPerceptorDanielAdamov::GetItemPriority(EItemType ItemType, int32 Value) const
{
	if (Value <= 0)
		return 0;
	
	const bool bHasWeapon{ HasUsableWeapon() };
	const float Health{ GetHealthPct() };
	const float Stamina{ GetStaminaPct() };

	// Weapon is the biggest priority if survivor doesn't have one.
	// Medkit is second-biggest priority if health is low
	// Food is third-biggest priority if stamina is low
	switch (ItemType)
	{
	case EItemType::Pistol:
		return bHasWeapon ? 30 : 101;
	case EItemType::Shotgun:
		return bHasWeapon ? 30 : 100;
	case EItemType::Medkit:
		return Health < 0.5f ? 90 : 30;
	case EItemType::Food:
		return Stamina < 0.5f ? 80 : 40;
	default:
		return 0; // Garbage or unknown
	}
}

bool UStudentPerceptorDanielAdamov::HasUsableWeapon() const noexcept
{
	// Returns true if Inventory has a Pistol or Shotgun with >0 ammo
	
	const UInventoryComponent* Inv{ GetOwner()->FindComponentByClass<UInventoryComponent>() };
	if (!Inv)
		return false;
	
	for (const ABaseItem* Item : Inv->GetInventory())
	{
		if (!Item)
			continue;
		
		const EItemType T{ Item-> GetItemType() };
		if ((T == EItemType::Pistol || T == EItemType::Shotgun) && Item->GetValue() > 0 )
			return true;
	}
	
	return false;
}

AActor* UStudentPerceptorDanielAdamov::SelectHouseTarget() const
{
	const FVector MyLocation{ GetOwner()->GetActorLocation() };
	
	// Choose the Nearest House which is not already visited
	AActor* BestHouse{ nullptr };
	float BestDistSqr{ TNumericLimits<float>::Max() };
	for (const FPerceivedHouse& H : Houses)
	{
		// Skip already visited houses
		if (!H.Actor.IsValid() || H.bVisited)
			continue;
		
		const float DistSqr{ static_cast<float>(FVector::DistSquared( MyLocation, H.LastKnownLocation )) };
		if (DistSqr < BestDistSqr)
		{
			BestDistSqr = DistSqr;
			BestHouse = H.Actor.Get();
		}
	}
	
	return BestHouse;
}

void UStudentPerceptorDanielAdamov::WriteBlackboard()
{
	// This is being called every frame and on every Refresh World Memory...
	// It is needed I think because targets are constantly updated...
	
	UBlackboardComponent* BB{ GetBlackboard() };
	if (!BB)
		return;
	
	AActor* Threat{ SelectThreat() };
	// Don't update if key Object has not changed - prevents constant switching
	WriteObjectIfChanged( BB, SurvivorBBKeys::ThreatActor, Threat );
	WriteObjectIfChanged( BB, SurvivorBBKeys::TargetItem, SelectTargetItem() );
	WriteObjectIfChanged( BB, SurvivorBBKeys::KnownHouseTarget, SelectHouseTarget() );
	
	// -----------------------------------
	// Set Threat Information variables - used for Combat/Flee tasks:
	// -----------------------------------
	float ThreatSpeed{ 0.f };
	if (Threat)
	{
		if (const UFloatingPawnMovement* Move = Threat->FindComponentByClass<UFloatingPawnMovement>())
		{
			ThreatSpeed = Move->GetMaxSpeed();
		}
	}
	BB->SetValueAsFloat( SurvivorBBKeys::ThreatSpeed, ThreatSpeed );
	
	int32 NearCount{ 0 };
	const FVector MyLocation{ GetOwner()->GetActorLocation() };
	for (const FPerceivedActor& Z : Zombies)
	{
		if (Z.Actor.IsValid() && FVector::Dist2D( MyLocation, Z.LastKnownLocation ) <= NearThreatRadius)
		{
			++NearCount;
		}
	}
	BB->SetValueAsInt( SurvivorBBKeys::NearCount, NearCount );
	
	// -----------------------------------
	// Set Purge Zone variables
	// -----------------------------------
	AActor* DangerPurge{ nullptr };
	float BestDangerDistSqr{ TNumericLimits<float>::Max() };
	for (const FPerceivedActor& P : PurgeZones)
	{
		if (!P.Actor.IsValid()) 
			continue;
		const APurgeZone* Purge{ Cast<APurgeZone>( P.Actor.Get() ) };
		if (!Purge) 
			continue;

		const float DistSqr{ static_cast<float>( FVector::DistSquared2D( MyLocation, P.LastKnownLocation ) ) };
		// Only a threat if we're inside the danger radius
		if (DistSqr <= PurgeSafetyRadius * PurgeSafetyRadius && DistSqr < BestDangerDistSqr)
		{
			BestDangerDistSqr = DistSqr;
			DangerPurge = P.Actor.Get();
		}
	}
	WriteObjectIfChanged( BB, SurvivorBBKeys::PurgeToAvoid, DangerPurge );
}

UBlackboardComponent* UStudentPerceptorDanielAdamov::GetBlackboard() const
{
	const APawn* Pawn{ Cast<APawn>( GetOwner() ) };
	AAIController* AI{ Pawn ? Cast<AAIController>( Pawn->GetController() ) : nullptr };
	
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

void UStudentPerceptorDanielAdamov::UpdateStuckGuard(const FVector& MyLoc)
{
	AActor* HouseTarget{ SelectHouseTarget() };
	
	// Only guard while a house is not null and NOT INVESTIGATING item
	const bool bPursuingHouse{ (HouseTarget != nullptr) && (SelectTargetItem() == nullptr) };
	if (!bPursuingHouse)
	{
		StuckTime = 0.f;
		LastRefreshLocation = MyLoc;
		return;
	}
	
	const float Moved{  static_cast<float>( FVector::Dist2D( MyLoc, LastRefreshLocation ) ) };
	LastRefreshLocation = MyLoc;
	
	StuckTime = (Moved < StuckMoveThreshold) ? StuckTime + RefreshInterval : 0.f;
	
	if (StuckTime >= StuckTimeout)
	{
		if (FPerceivedHouse* H = FindRecord( Houses, HouseTarget ))
		{
			H->bVisited = true;
		}
		StuckTime = 0.f;
	}
}

void UStudentPerceptorDanielAdamov::WriteObjectIfChanged(UBlackboardComponent* BB, const FName& Key, UObject* NewValue)
{
	if (BB->GetValueAsObject( Key ) != NewValue)
	{
		BB->SetValueAsObject( Key, NewValue );
	}
}
