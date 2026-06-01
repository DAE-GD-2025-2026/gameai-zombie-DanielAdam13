#include "BTTask_GrabItem_DanA.h"

#include "AIController.h"
#include "AdamovDanielZombieRuntime/StudentPerceptorDanielAdamov.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"

UBTTask_GrabItem_DanA::UBTTask_GrabItem_DanA()
{
	NodeName = TEXT( "Grab Item" );
	TargetItemKey.AddObjectFilter( 
		this, GET_MEMBER_NAME_CHECKED( UBTTask_GrabItem_DanA, TargetItemKey ), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_GrabItem_DanA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires a valid AIController and Pawn as well as a valid BB
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	if (!Pawn || !BB)
		return EBTNodeResult::Failed;
	
	ABaseItem* Item{ Cast<ABaseItem>( BB->GetValueAsObject( TargetItemKey.SelectedKeyName ) ) };
	if (!Item)
		return EBTNodeResult::Failed;
	
	UInventoryComponent* Inventory{ Pawn->FindComponentByClass<UInventoryComponent>() };
	if (!Inventory)
		return EBTNodeResult::Failed;
	
	// Check just in case, MoveTo would already check this but it's defensive:
	const float Dist{ static_cast<float>(FVector::Dist2D( Pawn->GetActorLocation(), Item->GetActorLocation() )) };
	if (Dist > Inventory->GetPickupRange())
		return EBTNodeResult::Failed;
	
	// First empty slot:
	const auto& Items{ Inventory->GetInventory() };
	int32 FreeSlotIdx{ INDEX_NONE };
	for (int32 i{}; i < Items.Num(); ++i)
	{
		if (Items[i] == nullptr)
		{
			FreeSlotIdx = i;
			break;
		}
	}
	// Inventory slots full -> just skip this Grab task
	if (FreeSlotIdx == INDEX_NONE)
	{
		return EBTNodeResult::Failed;
	}
	
	const bool bGrabbed{ Inventory->GrabItem( FreeSlotIdx, Item ) };
	if (bGrabbed)
	{
		BB->ClearValue( TargetItemKey.SelectedKeyName );
		
		if (UStudentPerceptorDanielAdamov* Perceptor = Pawn->FindComponentByClass<UStudentPerceptorDanielAdamov>())
		{
			Perceptor->OnInventoryChanged.Broadcast();
		}
	}
	return bGrabbed ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
