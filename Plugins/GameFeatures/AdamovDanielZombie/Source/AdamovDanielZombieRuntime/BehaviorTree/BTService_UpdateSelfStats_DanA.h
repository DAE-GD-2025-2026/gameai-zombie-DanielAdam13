#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateSelfStats_DanA.generated.h"

// One of the most important parts of the whole project:
// Checks things that need to always be updated - health, stamina, Inventory slot info
// + took damage key since Damage Stimuli does not work
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTService_UpdateSelfStats_DanA : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateSelfStats_DanA();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, Category="Stats")
	FBlackboardKeySelector HealthPctKey;
	UPROPERTY(EditAnywhere, Category="Stats")
	FBlackboardKeySelector StaminaPctKey;
	
	UPROPERTY(EditAnywhere, Category="Stats")
	FBlackboardKeySelector HasFreeSlotKey;
	UPROPERTY(EditAnywhere, Category="Stats")
	FBlackboardKeySelector HasFoodKey;
	UPROPERTY(EditAnywhere, Category="Stats")
	FBlackboardKeySelector HasMedkitKey;
	
	UPROPERTY(EditAnywhere, Category="Stats")
	FBlackboardKeySelector TookDamageKey;
	
private:
	float LastHealth{ -1.f };
	
};
