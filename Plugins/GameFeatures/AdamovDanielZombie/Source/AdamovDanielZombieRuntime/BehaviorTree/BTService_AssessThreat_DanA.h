#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AssessThreat_DanA.generated.h"

/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTService_AssessThreat_DanA : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_AssessThreat_DanA();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	// Needed BB information
	UPROPERTY(EditAnywhere, Category = "Assess") 
	FBlackboardKeySelector ThreatActorKey;
	UPROPERTY(EditAnywhere, Category = "Assess") 
	FBlackboardKeySelector ThreatSpeedKey;
	UPROPERTY(EditAnywhere, Category = "Assess") 
	FBlackboardKeySelector ThreatCountKey;
	UPROPERTY(EditAnywhere, Category = "Assess") 
	FBlackboardKeySelector HealthPctKey;
	UPROPERTY(EditAnywhere, Category = "Assess") 
	FBlackboardKeySelector ShouldFleeKey;
	UPROPERTY(EditAnywhere, Category = "Assess") 
	FBlackboardKeySelector CombatStuckKey;
	
	UPROPERTY(EditAnywhere, Category = "Assess") 
	float CriticalHealthPct{ 0.30f };
	UPROPERTY(EditAnywhere, Category = "Assess") 
	int32 SwarmThreatCount{ 3 };
	UPROPERTY(EditAnywhere, Category = "Assess") 
	float HeavySpeedThreshold{ 300.f };
	
private:
	// Helper
	static bool HasUsableWeapon(const APawn& Pawn) noexcept;
};
