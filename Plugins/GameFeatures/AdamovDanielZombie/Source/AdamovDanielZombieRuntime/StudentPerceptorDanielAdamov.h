#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemType.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "StudentPerceptorDanielAdamov.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnInventoryChanged );

enum class EItemType : uint8;

// ----------- Remembered Targets Structs -----------
// For zombie actors
struct FPerceivedActor
{
	TWeakObjectPtr<AActor> Actor;
	FVector LastKnownLocation{ FVector::ZeroVector };
	float LastSeenTime{ 0.f };
	bool bIsVisible{ false };
};
// For items
struct FPerceivedItem : FPerceivedActor
{
	EItemType Type{ EItemType::Garbage };
};
// For houses
struct FPerceivedHouse : FPerceivedActor
{
	bool bVisited{ false };
};
// ------------------------------------------------

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ADAMOVDANIELZOMBIERUNTIME_API UStudentPerceptorDanielAdamov : public UActorComponent
{
	GENERATED_BODY()

public:
	UStudentPerceptorDanielAdamov();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Just clears the refresh timer

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	// Fired from the Grab Task when item grabbed
	UPROPERTY(BlueprintAssignable, Category = "Perceptor")
	FOnInventoryChanged OnInventoryChanged;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Perceptor")
	float RefreshInterval{ 0.5f };
	
	// How long a zombie survivor no longer sees stays in "memory"
	UPROPERTY(EditAnywhere, Category = "Perceptor")
	float ThreatMemoryDuration{ 5.f };
	
	// Distance at which survivor consider house checked
	UPROPERTY(EditAnywhere, Category = "Perceptor")
	float HouseVisitedRange{ 350.f };
	
private:
	TArray<FPerceivedActor> Zombies;
	TArray<FPerceivedItem> Items;
	TArray<FPerceivedHouse> Houses;
	
	FTimerHandle RefreshTimer;
	
	// Called in OnPerceptionUpdate
	void HandleSight(AActor* Actor, const FAIStimulus& Stimulus);
	void HandleDamage(AActor* Actor, const FAIStimulus& Stimulus);
	
	void RefreshWorldMemory(); // Clears stagnant memory and updates house perception
	void WriteBlackboard() const;
	
	// ----- Hardest Logic of the class -----
	AActor* SelectThreat() const; // Called in WriteBlackboard()
	AActor* SelectTargetItem() const; // Called in WriteBlackboard()
	int32 GetItemPriority(EItemType ItemType, int32 Value) const; // Helper for Item
	bool HasUsableWeapon() const noexcept; // Helper for Item
	AActor* SelectHouseTarget() const; // Called in WriteBlackboard()
	
	// Templated function so we can use it for a Zombie, Item and House
	// Checks if an actor of a type already exists in the TArray
	// TRecord is of type FPerceivedActor or derived
	template<typename TRecord>
	TRecord* FindRecord(TArray<TRecord>& Container, const AActor* Key) const;
	
	// BB getters:
	UBlackboardComponent* GetBlackboard() const;
	float GetHealthPct() const noexcept; // Used for Item Priority
	float GetStaminaPct() const noexcept;  // Used for Item Priority
	
	// Stuck guard issue with infinite MoveTo: nullptr loop in the InvestigateHouse Sequence
	UPROPERTY(EditAnywhere, Category = "Perceptor") 
	float StuckTimeout{ 3.f };
	UPROPERTY(EditAnywhere, Category = "Perceptor") 
	float StuckMoveThreshold{ 2.f };
	FVector LastRefreshLocation{ FVector::ZeroVector };
	float StuckTime{ 0.f };
	
	void UpdateStuckGuard(const FVector& MyLoc);
};

template <typename TRecord>
TRecord* UStudentPerceptorDanielAdamov::FindRecord(TArray<TRecord>& Container, const AActor* Key) const
{
	// TRecord is of FPerceivedActor or derived structs type
	for (TRecord& R : Container)
	{
		// FPerceivedActor.Actor.Get()
		if (R.Actor.Get() == Key)
		{
			return &R;
		}
	}
	return nullptr;
}
