#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemType.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "StudentPerceptorDanielAdamov.generated.h"

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
	
protected:
	UPROPERTY(EditAnywhere, Category = "Perceptor")
	float RefreshInterval{ 0.25f };
	
	// How long a zombie survivor no longer sees stays in "memory"
	UPROPERTY(EditAnywhere, Category = "Perceptor")
	float ThreatMemoryDuration{ 5.0f };
	
	// Distance at which survivor consider house checked
	UPROPERTY(EditAnywhere, Category = "Perceptor")
	float HouseVisitedRange{ 350.0f };
	
private:
	TArray<FPerceivedActor> Zombies;
	TArray<FPerceivedItem> Items;
	TArray<FPerceivedHouse> Houses;
	
	FTimerHandle RefreshTimer;
	
	void HandleSight(AActor* Actor, const FAIStimulus& Stimulus);
	void HandleDamage(AActor* Actor, const FAIStimulus& Stimulus);
	
	void RefreshWorldMemory(); // Using thr Refresh Timer, updates the arrays
	void WriteBlackboard();
	
	// Templated function so we can use it for a Zombie, Item and House
	// Checks if an actor of a type already exists in the TArray
	// TRecord is of type FPerceivedActor or derived
	template<typename TRecord>
	TRecord* FindRecord(TArray<TRecord>& Container, const AActor* Key) const;
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
