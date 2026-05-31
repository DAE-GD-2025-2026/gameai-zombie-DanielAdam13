#include "BTTask_UseItemOfType.h"

#include "AIController.h"
#include "Common/InventoryComponent.h"

UBTTask_UseItemOfType::UBTTask_UseItemOfType()
{
	NodeName = TEXT( "Use Item Of Type" );
}

EBTNodeResult::Type UBTTask_UseItemOfType::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires a valid AIController and Pawn
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	if (!Pawn)
		return EBTNodeResult::Failed;
	
	// Task requires a valid Inventory component on Pawn
	UInventoryComponent* Inventory{ Pawn->FindComponentByClass<UInventoryComponent>() };
	if (!Inventory)
		return EBTNodeResult::Failed;
	
	// Loop through all item slots and USE ITEM if the slot contains an item of the slot and has at least 1 count
	const TArray<ABaseItem*>& Items{ Inventory->GetInventory() };
	for (int32 Slot{}; Slot < Items.Num(); ++Slot)
	{
		const ABaseItem* Item{ Items[Slot] };
		if (Item && Item->GetItemType() == ItemType && Item->GetValue() >= 1)
		{
			return Inventory->UseItem( Slot ) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
		}
	}
	
	return EBTNodeResult::Failed;
}
