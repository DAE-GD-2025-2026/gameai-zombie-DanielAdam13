#include "BTTask_DropEmptyWeapons_DanA.h"

#include "AIController.h"
#include "AdamovDanielZombieRuntime/StudentPerceptorDanielAdamov.h"
#include "Common/InventoryComponent.h"

UBTTask_DropEmptyWeapons_DanA::UBTTask_DropEmptyWeapons_DanA()
{
	NodeName = TEXT( "Drop Empty Weapons" );
}

EBTNodeResult::Type UBTTask_DropEmptyWeapons_DanA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires vali AiController and Pawn
	AAIController* Controller{ OwnerComp.GetAIOwner() };
	APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	if (!Pawn)
		return EBTNodeResult::Failed;
	
	UInventoryComponent* Inventory{ Pawn->FindComponentByClass<UInventoryComponent>() };
	if (!Inventory)
		return EBTNodeResult::Failed;
	
	// Pretty self-explanatory - Remove item from inventory if type is weapon and value is <= 0
	bool bRemovedAny{ false };
	const auto& Items{ Inventory->GetInventory() };
	for (int32 Slot{}; Slot < Items.Num(); ++Slot)
	{
		const ABaseItem* Item{ Items[Slot] };
		if (!Item)
			continue;
		
		const EItemType T{ Item->GetItemType() };
		if ((T == EItemType::Pistol || T == EItemType::Shotgun) && Item->GetValue() <= 0)
		{
			if (Inventory->RemoveItem( Slot ))
				bRemovedAny = true;
		}
	}
	
	// BROADCAST OnInventoryChanged (only if it actually did)
	// Used for the UI update
	if (bRemovedAny)
	{
		if (const UStudentPerceptorDanielAdamov* P = Pawn->FindComponentByClass<UStudentPerceptorDanielAdamov>())
		{
			P->OnInventoryChanged.Broadcast();
		}
	}
	
	return bRemovedAny ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
