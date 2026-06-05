#include "BTService_UpdateSelfStats_DanA.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/HealthComponent.h"
#include "Common/InventoryComponent.h"
#include "Common/StaminaComponent.h"

UBTService_UpdateSelfStats_DanA::UBTService_UpdateSelfStats_DanA()
{
	NodeName = TEXT("Update Self Stats");
	Interval = 0.2f;
	RandomDeviation = 0.05f;
	HealthPctKey.AddFloatFilter( 
		this, GET_MEMBER_NAME_CHECKED( UBTService_UpdateSelfStats_DanA, HealthPctKey ) );
	StaminaPctKey.AddFloatFilter( 
		this, GET_MEMBER_NAME_CHECKED( UBTService_UpdateSelfStats_DanA, StaminaPctKey ) );
	HasFreeSlotKey.AddBoolFilter( 
		this, GET_MEMBER_NAME_CHECKED( UBTService_UpdateSelfStats_DanA, HasFreeSlotKey ) );
	HasFoodKey.AddBoolFilter( 
		this, GET_MEMBER_NAME_CHECKED( UBTService_UpdateSelfStats_DanA, HasFoodKey ) );
	HasMedkitKey.AddBoolFilter( 
		this, GET_MEMBER_NAME_CHECKED( UBTService_UpdateSelfStats_DanA, HasMedkitKey ) );
}

void UBTService_UpdateSelfStats_DanA::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode( OwnerComp, NodeMemory, DeltaSeconds );
	
	// Task requires a valid AIController and Pawn as well as a valid BB
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	if (!Pawn || !BB)
		return;
	
	// Health Comp from GameAI_Zombie
	if (const UHealthComponent* Health = Pawn->FindComponentByClass<UHealthComponent>())
	{
		const float MaxHP{ static_cast<float>(FMath::Max( 1, Health->GetMaxHealth() )) };
		BB->SetValueAsFloat( HealthPctKey.SelectedKeyName, Health->GetHealth() / MaxHP );
	}
	
	// Stamina Comp from GameAI_Zombie
	if (const UStaminaComponent* Stamina = Pawn->FindComponentByClass<UStaminaComponent>())
	{
		const float MaxSt{ FMath::Max( 1, Stamina->GetMaxStamina() ) };
		BB->SetValueAsFloat( StaminaPctKey.SelectedKeyName, Stamina->GetCurrentStamina() / MaxSt );
	}
	
	// Inventory booleans checks - Has Free Slot / Has Food / Has Medkit
	if (const UInventoryComponent* Inv = Pawn->FindComponentByClass<UInventoryComponent>())
	{
		bool bHasFree{ false };
		bool bHasFood{ false };
		bool bHasMedkit{ false };
		for (const ABaseItem* Item : Inv->GetInventory())
		{
			if (!Item)
			{
				bHasFree = true; 
				continue;
			}
			if (Item->GetValue() <= 0) // Used resources don't count
				continue;
			if (Item->GetItemType() == EItemType::Food)
				bHasFood = true;
			if (Item->GetItemType() == EItemType::Medkit)
				bHasMedkit = true;
		}
		BB->SetValueAsBool(HasFreeSlotKey.SelectedKeyName, bHasFree);
		BB->SetValueAsBool(HasFoodKey.SelectedKeyName, bHasFood);
		BB->SetValueAsBool(HasMedkitKey.SelectedKeyName, bHasMedkit);
	}
}
