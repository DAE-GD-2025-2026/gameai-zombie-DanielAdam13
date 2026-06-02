#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetRunning_DanA.generated.h"

/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTTask_SetRunning_DanA : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SetRunning_DanA();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bRun{ true };
};
