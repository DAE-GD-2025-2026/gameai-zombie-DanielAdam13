#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ScanForThreat_DanA.generated.h"

/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTTask_ScanForThreat_DanA : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ScanForThreat_DanA();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	// The key that procs this Task:
	UPROPERTY(EditAnywhere, Category = "Scan")
	FBlackboardKeySelector TookDamageKey;
	UPROPERTY(EditAnywhere, Category = "Scan")
	FBlackboardKeySelector ThreatActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Scan")
	float ScanTurnRateDeg{ 240.f };
	// Total degrees to rotate before giving up - if nothing found
	UPROPERTY(EditAnywhere, Category = "Scan")
	float ScanSweepDeg{ 400.f };
	
private:
	float SweptSoFar{ 0.f };
};
