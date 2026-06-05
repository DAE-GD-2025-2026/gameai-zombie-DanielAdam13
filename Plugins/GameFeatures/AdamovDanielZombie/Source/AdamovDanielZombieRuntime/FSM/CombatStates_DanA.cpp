#include "CombatStates_DanA.h"

#include "AIController.h"
#include "AdamovDanielZombieRuntime/Steering/Steering_DanA.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"

FCombatState::FCombatState(const FCombatContext& InContext)
	:State_DanA::State_DanA(),
	Context( InContext )
{
}

APawn* FCombatState::GetPawn() const noexcept
{
	AAIController* C{ GetController() };
	return C ? C->GetPawn() : nullptr;
}

AActor* FCombatState::GetThreatActor() const noexcept
{
	UBlackboardComponent* BB{ GetBlackboard() };
	return BB ? Cast<AActor>( BB->GetValueAsObject( Context.ThreatKeyName ) ) : nullptr;
}

FEngageState::FEngageState(const FCombatContext& InContext)
	:FCombatState( InContext )
{
}

void FEngageState::OnEnter()
{
	FireTimer = 0.f;
}

void FEngageState::OnUpdate(float DeltaTime)
{
	APawn* Pawn{ GetPawn() };
	const AActor* Threat{ GetThreatActor() };
	if (!Pawn || !Threat) 
		return;
	
	const FVector ThrLocation{ Threat->GetActorLocation() };
	
	// 1. Face the Threat by APPLYING the Face behavior
	const Steering::FSteeringState State{ Steering::MakeState( *Pawn ) };
	Steering::Face FaceBehavior;
	FaceBehavior.SetTargetPosition( FVector2D(ThrLocation.X, ThrLocation.Y) );
	const Steering::FSteeringOutput Out{ FaceBehavior.Calculate( DeltaTime, State ) };
	Steering::Apply( *Pawn, Out, DeltaTime, Context.TurnRateDeg );
	
	// 2. Fire weapon if aligned and < Cooldown
	FireTimer += DeltaTime;
	if (FireTimer >= Context.FireCooldown && FMath::Abs( Out.AngularDeg ) <= Context.AlignToleranceDeg)
	{
		if (TryFireWeapon( *Pawn ))
		{
			FireTimer = 0.f;
		}
	}
}

FRepositionState::FRepositionState(const FCombatContext& InContext)
	:FCombatState( InContext )
{
}

void FRepositionState::OnEnter()
{
	StuckTime = 0.f;
}

void FRepositionState::OnUpdate(float DeltaTime)
{
	APawn* Pawn{ GetPawn() };
	const AActor* Threat{ GetThreatActor() };
	if (!Pawn || !Threat) 
		return;
	
	// ---------------------------------------------------------
	// Stuck in Reposition Logic
	StuckTime += DeltaTime;
	
	// If stuck -> set stuck flag which will abort Combat in the BT
	if (StuckTime >= Context.GiveUpTime)
	{
		if (UBlackboardComponent* BB = GetBlackboard())
		{
			BB->SetValueAsBool(TEXT("bCombatStuck"), true);
		}
		return;
	}
	// --------------------------------------------------------
	
	const FVector ThrLocation{ Threat->GetActorLocation() };
	const FVector2D ThrLoc2D(ThrLocation.X, ThrLocation.Y);
	
	// Flee and Face the Threat by APPLYING the Face behavior
	const Steering::FSteeringState State{ Steering::MakeState( *Pawn ) };
	Steering::Flee FleeBehavior;
	Steering::Face FaceBehavior;
	FleeBehavior.SetTargetPosition( ThrLoc2D );
	FaceBehavior.SetTargetPosition( ThrLoc2D );
	
	// Linear velocity - Flee
	Steering::FSteeringOutput Out{ FleeBehavior.Calculate( DeltaTime, State ) };
	// Angular - Face
	Out.AngularDeg = FaceBehavior.Calculate( DeltaTime, State ).AngularDeg;
	Steering::Apply( *Pawn, Out, DeltaTime, Context.TurnRateDeg );
}

bool TryFireWeapon(const APawn& Pawn)
{
	UInventoryComponent* Inventory{ Pawn.FindComponentByClass<UInventoryComponent>() };
	if (!Inventory)
		return false;
	
	const auto& Items{ Inventory->GetInventory() };
	for (int32 Slot{}; Slot < Items.Num(); ++Slot)
	{
		const ABaseItem* Item{ Items[Slot] };
		if (!Item)
			continue;
		
		const EItemType T{ Item->GetItemType() };
		if ((T == EItemType::Pistol || T == EItemType::Shotgun) && Item->GetValue() > 0)
		{
			return Inventory->UseItem( Slot );
		}
	}
	return false;
}
