#include "BTService_AssessThreat_DanA.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"

UBTService_AssessThreat_DanA::UBTService_AssessThreat_DanA()
{
	NodeName = TEXT("Assess Threat");
	Interval = 0.15f;
	RandomDeviation = 0.05f;
	
	ThreatActorKey.AddObjectFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTService_AssessThreat_DanA, ThreatActorKey), AActor::StaticClass());
	ThreatSpeedKey.AddFloatFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTService_AssessThreat_DanA, ThreatSpeedKey));
	ThreatCountKey.AddIntFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTService_AssessThreat_DanA, ThreatCountKey));
	HealthPctKey.AddFloatFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTService_AssessThreat_DanA, HealthPctKey));
	ShouldFleeKey.AddBoolFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTService_AssessThreat_DanA, ShouldFleeKey));
}

void UBTService_AssessThreat_DanA::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Task requires valid AIController as well as Pawn and BB
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	if (!Pawn || !BB) 
		return;
	
	// If no Threat -> early out AND set Should Flee to FALSE
	if (!BB->GetValueAsObject( ThreatActorKey.SelectedKeyName ))
	{
		BB->SetValueAsBool( ShouldFleeKey.SelectedKeyName, false );
		return;
	}
	
	bool bFlee{ false };
	const float ThreatSpeed{ BB->GetValueAsFloat( ThreatSpeedKey.SelectedKeyName ) };
	
	// Flee Conditions:
	// 1. Has no usable weapon/empty
	if (!HasUsableWeapon( *Pawn ))
	{
		bFlee = true;
	}
	// 2. Health is too low
	else if (BB->GetValueAsFloat(HealthPctKey.SelectedKeyName) < CriticalHealthPct)
	{
		bFlee = true;
	}
	// 3. Too many zombies nearby
	else if (BB->GetValueAsInt(ThreatCountKey.SelectedKeyName) >= SwarmThreatCount)
	{
		bFlee = true;
	}
	// 4. Heavy zombie nearby, not worth fighting
	else if (ThreatSpeed > 0.f && ThreatSpeed < HeavySpeedThreshold)
	{
		bFlee = true;
	}
	
	// !! Else we fight - enter Combat task !!
	
	BB->SetValueAsBool( ShouldFleeKey.SelectedKeyName, bFlee );
}

bool UBTService_AssessThreat_DanA::HasUsableWeapon(const APawn& Pawn) noexcept
{
	// Returns true if Inventory has a Pistol or Shotgun with >0 ammo
	
	const UInventoryComponent* Inv{ Pawn.FindComponentByClass<UInventoryComponent>() };
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
