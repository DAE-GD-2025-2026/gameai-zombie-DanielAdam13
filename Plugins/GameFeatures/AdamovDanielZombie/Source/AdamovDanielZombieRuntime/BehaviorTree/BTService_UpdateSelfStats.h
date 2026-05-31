#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateSelfStats.generated.h"

/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTService_UpdateSelfStats : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateSelfStats();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, Category="Stats")
	FBlackboardKeySelector HealthPctKey;
	UPROPERTY(EditAnywhere, Category="Stats")
	FBlackboardKeySelector StaminaPctKey;
	
	
};
