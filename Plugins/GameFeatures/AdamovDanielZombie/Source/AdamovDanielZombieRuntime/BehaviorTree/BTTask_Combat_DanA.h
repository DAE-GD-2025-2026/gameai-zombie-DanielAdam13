#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "../FSM/FSM_DanA.h"
#include "BTTask_Combat_DanA.generated.h"

/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTTask_Combat_DanA : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Combat_DanA();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Combat")
	FBlackboardKeySelector ThreatActorKey;

	// Used to populate the FSM needed data
	UPROPERTY(EditAnywhere, Category = "Combat")
	float TurnRateDeg{ 540.f };
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MinDistance{ 250.f };
	UPROPERTY(EditAnywhere, Category = "Combat")
	float SafeDistance{ 450.f };
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireCooldown{ 0.5f };
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AlightToleranceDeg{ 12.f };
	
	void BuildFSM(AAIController& Controller, UBlackboardComponent& Blackboard);
	TUniquePtr<GameAI::FSM::FSM_DanA> FSMInstance;
	
};
