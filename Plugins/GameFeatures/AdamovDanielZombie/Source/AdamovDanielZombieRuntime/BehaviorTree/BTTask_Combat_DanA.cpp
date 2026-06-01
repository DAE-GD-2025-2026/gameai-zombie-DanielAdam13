#include "BTTask_Combat_DanA.h"

#include "AIController.h"
#include "AdamovDanielZombieRuntime/Steering/Steering.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Combat_DanA::UBTTask_Combat_DanA()
{
	NodeName = TEXT("Combat");
	bNotifyTick = true;
	ThreatActorKey.AddObjectFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_Combat_DanA, ThreatActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Combat_DanA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires vali AiController and Pawn
	AAIController* Controller{ OwnerComp.GetAIOwner() };
	if (APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr }; !Pawn)
		return EBTNodeResult::Failed;
	
	// Linear Velocity / Rotation is done manually via the Steering Behaviors in Tick
	Controller->StopMovement();
	
	// HAS TO BE IN PROGRESS SO Tick goes through its own logic
	// Otherwise, task ends here
	return EBTNodeResult::InProgress;
}

void UBTTask_Combat_DanA::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Task requires a valid AIController and Pawn as well as a valid BB
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	if (!Pawn || !BB)
	{
		FinishLatentTask( OwnerComp, EBTNodeResult::Failed );
		return;
	}
	
	// If no threat actor -> it means survivor is safe and thus SUCCEEDED
	const AActor* ThreatActor{ Cast<AActor>( BB->GetValueAsObject( ThreatActorKey.SelectedKeyName ) ) };
	if (!ThreatActor)
	{
		FinishLatentTask( OwnerComp, EBTNodeResult::Succeeded );
		return;
	}

	const Steering::FSteeringState State{ Steering::MakeState( *Pawn ) };
	Steering::Face FaceBehavior;
	const FVector ThrLocation{ ThreatActor->GetActorLocation() };
	FaceBehavior.SetTargetPosition( FVector2D(ThrLocation.X, ThrLocation.Y) );
	
	Steering::Apply( *Pawn, FaceBehavior.Calculate( DeltaSeconds, State ), DeltaSeconds, TurnRateDeg );
}
