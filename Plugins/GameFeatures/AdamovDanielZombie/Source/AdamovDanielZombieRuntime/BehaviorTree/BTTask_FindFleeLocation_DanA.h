#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindFleeLocation_DanA.generated.h"

/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTTask_FindFleeLocation_DanA : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindFleeLocation_DanA();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Flee")
	float FleeRadius{ 1800.f };
	
	// Samples to choose the best Candidate for Flee Location
	UPROPERTY(EditAnywhere, Category = "Flee")
	int32 CandidateCount{ 10 };
	
	UPROPERTY(EditAnywhere, Category = "Flee")
	FBlackboardKeySelector ThreatActorKey;
	UPROPERTY(EditAnywhere, Category = "Flee")
	FBlackboardKeySelector FleeTargetKey;
	
};
