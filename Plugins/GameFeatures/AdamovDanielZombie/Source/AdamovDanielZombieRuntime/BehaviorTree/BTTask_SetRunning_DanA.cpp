#include "BTTask_SetRunning_DanA.h"

#include "AIController.h"
#include "Survivor/SurvivorPawn.h"

UBTTask_SetRunning_DanA::UBTTask_SetRunning_DanA()
{
	NodeName = TEXT("Set Running");
}

EBTNodeResult::Type UBTTask_SetRunning_DanA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires valid Controller and Pawn Survivor
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	ASurvivorPawn* Survivor{ Controller ? Cast<ASurvivorPawn>(Controller->GetPawn()) : nullptr };
	if (!Survivor) 
		return EBTNodeResult::Failed;

	const UStaminaComponent* StaminaComp{ Survivor->FindComponentByClass<UStaminaComponent>() };
	if (bRun && (StaminaComp && StaminaComp->GetCurrentStamina() > 0.f))
		Survivor->StartRunning();
	else
		Survivor->StopRunning();
	
	return EBTNodeResult::Succeeded;
}
