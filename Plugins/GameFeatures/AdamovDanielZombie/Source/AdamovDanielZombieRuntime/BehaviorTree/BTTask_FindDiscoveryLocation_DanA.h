#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindDiscoveryLocation_DanA.generated.h"

/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTTask_FindDiscoveryLocation_DanA : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindDiscoveryLocation_DanA();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Explore")
	float ExploreRadius{ 1500.f };

	// Sampled candidate points 
	UPROPERTY(EditAnywhere, Category = "Explore")
	int32 CandidateCount{ 8 };

	// Signifies how important the Current Direction is in the Candidate evaluation.
	// 1 would mean that a candidate in the current direction will always be preferred.
	UPROPERTY(EditAnywhere, Category = "Explore", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HeadingBias{ 0.5f };

	UPROPERTY(EditAnywhere, Category = "Explore")
	FBlackboardKeySelector TargetLocationKey;
	
};
