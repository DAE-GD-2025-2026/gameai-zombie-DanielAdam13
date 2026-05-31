#include "FindWanderLocationDanielAdamov.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UFindWanderLocationDanielAdamov::UFindWanderLocationDanielAdamov()
{
	NodeName = TEXT( "Find Wander Location" );
	
	TargetLocationKey.AddVectorFilter( 
		this, GET_MEMBER_NAME_CHECKED( UFindWanderLocationDanielAdamov, TargetLocationKey ));
}

EBTNodeResult::Type UFindWanderLocationDanielAdamov::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires a valid AIController and Pawn as well as a valid Nav System
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	if (!Pawn)
		return EBTNodeResult::Failed;
	UNavigationSystemV1* NavSys{ FNavigationSystem::GetCurrent<UNavigationSystemV1>( Pawn->GetWorld() ) };
	if (!NavSys)
		return EBTNodeResult::Failed;
	
	// --- Choose and set the BB key as the wander location ---
	FNavLocation Result;
	if (NavSys->GetRandomReachablePointInRadius( Pawn->GetActorLocation(), WanderRadius, Result ))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector( 
			TargetLocationKey.SelectedKeyName, Result.Location );
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
