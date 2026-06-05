#include "BTTask_ScanForThreat_DanA.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ScanForThreat_DanA::UBTTask_ScanForThreat_DanA()
{
	NodeName = TEXT( "Scan For Threat" );
	bNotifyTick = true;
	TookDamageKey.AddBoolFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_ScanForThreat_DanA, TookDamageKey));
	ThreatActorKey.AddObjectFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_ScanForThreat_DanA, ThreatActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_ScanForThreat_DanA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires valid controller and pawn
	AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	if (!Pawn) 
		return EBTNodeResult::Failed;
	
	SweptSoFar = 0.f; // clean reset
	Controller->StopMovement(); // !!
	return EBTNodeResult::InProgress;
}

void UBTTask_ScanForThreat_DanA::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Task requires valid controller and pawn as well as BB
	AAIController* Controller{ OwnerComp.GetAIOwner() };
	APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	if (!Pawn || !BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed); 
		return;
	}
	
	// 1. FOUND THE THREAT -> Succeed task, reset took damage bool
	if (BB->GetValueAsObject(ThreatActorKey.SelectedKeyName))
	{
		BB->SetValueAsBool(TookDamageKey.SelectedKeyName, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	// 2. Else -> rotate
	const float Step{ ScanTurnRateDeg * DeltaSeconds };
	FRotator Rot{ Pawn->GetActorRotation() };
	Rot.Yaw += Step;
	Pawn->SetActorRotation( Rot );
	
	// 3. If max sweep reached and not found Threat -> Succeed task, reset took damage bool
	SweptSoFar += Step;
	if (SweptSoFar >= ScanSweepDeg)
	{
		BB->SetValueAsBool( TookDamageKey.SelectedKeyName, false );
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
