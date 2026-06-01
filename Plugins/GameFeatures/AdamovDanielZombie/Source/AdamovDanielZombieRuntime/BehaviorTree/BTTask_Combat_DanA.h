#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
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
	
protected:
	UPROPERTY(EditAnywhere, Category = "Combat")
	FBlackboardKeySelector ThreatActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float TurnRateDeg{ 540.f };
	
};
