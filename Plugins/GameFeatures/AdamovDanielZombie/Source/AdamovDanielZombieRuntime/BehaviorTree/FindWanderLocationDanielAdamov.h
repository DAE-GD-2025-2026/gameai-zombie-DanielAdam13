#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FindWanderLocationDanielAdamov.generated.h"

/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UFindWanderLocationDanielAdamov : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UFindWanderLocationDanielAdamov();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Wander")
	float WanderRadius{ 1500.f };
	
	UPROPERTY(EditAnywhere, Category = "Wander")
	FBlackboardKeySelector TargetLocationKey;
	
	
private:
};
