#include "BTTask_Combat_DanA.h"

#include "AIController.h"
#include "AdamovDanielZombieRuntime/FSM/CombatStates.h"
#include "AdamovDanielZombieRuntime/Steering/Steering.h"
#include "../FSM/Transition.h"
#include "../FSM/State.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Combat_DanA::UBTTask_Combat_DanA()
{
	NodeName = TEXT("Combat");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	ThreatActorKey.AddObjectFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_Combat_DanA, ThreatActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Combat_DanA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires vali AiController, Pawn and BB
	AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	if (!Pawn || !BB)
		return EBTNodeResult::Failed;
	
	// Linear Velocity / Rotation is done manually via the Steering Behaviors in Tick
	Controller->StopMovement();
	
	// Start State machine
	if (!FSMInstance)
	{
		BuildFSM( *Controller, *BB );
	}
	if (FSMInstance)
	{
		FSMInstance->Start();
	}
	
	// HAS TO BE IN PROGRESS SO Tick goes through its own logic
	// Otherwise, task ends here
	return EBTNodeResult::InProgress;
}

void UBTTask_Combat_DanA::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Task requires a valid BB and Threat Actor
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	
	// If no threat actor -> it means survivor is safe and thus SUCCEEDED
	const AActor* ThreatActor{ BB ? 
		Cast<AActor>( BB->GetValueAsObject( ThreatActorKey.SelectedKeyName ) ) : nullptr };
	if (!ThreatActor)
	{
		FinishLatentTask( OwnerComp, EBTNodeResult::Succeeded );
		return;
	}

	// Update State Machine - updates current state and checks for transitions internally
	if (FSMInstance)
		FSMInstance->Tick( DeltaSeconds );
}

void UBTTask_Combat_DanA::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	if (FSMInstance && FSMInstance->IsRunning())
		FSMInstance->Stop();
}

EBTNodeResult::Type UBTTask_Combat_DanA::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Healing - using medkit can Abort the Combat task
	if (FSMInstance)
		FSMInstance->Stop();
	if (AAIController* C = OwnerComp.GetAIOwner())
		C->StopMovement();
	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
		BB->SetValueAsBool(TEXT("bCombatStuck"), false);
	
	return EBTNodeResult::Aborted;
}

void UBTTask_Combat_DanA::BuildFSM(AAIController& Controller, UBlackboardComponent& Blackboard)
{
	using namespace GameAI::FSM;
	FSMInstance = MakeUnique<FSM>();
	FSMInstance->SetController( &Controller );
	FSMInstance->SetBlackboard( &Blackboard );
	
	// Just populate the FSM needed data with the ones from the Task
	FCombatContext Ctx;
	Ctx.ThreatKeyName = ThreatActorKey.SelectedKeyName;
	Ctx.TurnRateDeg = TurnRateDeg;
	Ctx.MinDistance = MinDistance;
	Ctx.SafeDistance = SafeDistance;
	Ctx.FireCooldown = FireCooldown;
	Ctx.AlignToleranceDeg = AlightToleranceDeg;
	
	State* Engage{ FSMInstance->AddState( std::make_unique<FEngageState>( Ctx ) ) };
	State* Reposition{ FSMInstance->AddState( std::make_unique<FRepositionState>( Ctx ) ) };
	
	// Transitions
	AAIController* C{ &Controller };
	UBlackboardComponent* BB{ &Blackboard };
	const FName Key{ Ctx.ThreatKeyName };
	auto DistanceToThreat{ [C, BB, Key]() -> float
	{
		const APawn* P{ C ? C->GetPawn() : nullptr };
		const AActor* Threat{ BB ? Cast<AActor>(BB->GetValueAsObject(Key)) : nullptr };
		return (P && Threat) ? 
			FVector::Dist2D( P->GetActorLocation(), Threat->GetActorLocation() ) : TNumericLimits<float>::Max();
	}};
	
	// Transitions live update with distance from Survivor Pawn to Threat
	const float MinDist{ Ctx.MinDistance };
	const float SafeDist{ Ctx.SafeDistance };
	FSMInstance->AddTransition( Engage, Reposition, 
		[DistanceToThreat, MinDist](){ return DistanceToThreat() < MinDist; } );
	FSMInstance->AddTransition( Reposition, Engage, 
		[DistanceToThreat, SafeDist](){ return DistanceToThreat() >= SafeDist; } );
}
