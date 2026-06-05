#pragma once
#include "CoreMinimal.h"
#include "State_DanA.h"

struct FCombatContext
{
	FName ThreatKeyName{ TEXT( "ThreatActor" ) };
	float TurnRateDeg{ 120.f };
	float MinDistance{ 250.f };
	float SafeDistance{ 450.f };
	float FireCooldown{ 0.5f };
	float AlignToleranceDeg{ 12.f };
	float GiveUpTime{ 2.f }; // seconds of no progress before deciding to go from Combat -> Flee
};

class FCombatState : public GameAI::FSM::State_DanA
{
public:
	explicit FCombatState(const FCombatContext& InContext);
	virtual ~FCombatState() override = default; 
	
protected:
	APawn* GetPawn() const noexcept;
	AActor* GetThreatActor() const noexcept;
	FCombatContext Context;
};

class FEngageState final : public FCombatState
{
public:
	explicit FEngageState(const FCombatContext& InContext);
	virtual ~FEngageState() override = default;
	
	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	
private:
	float FireTimer{ 0.f };
};

class FRepositionState final : public FCombatState
{
public:
	explicit FRepositionState(const FCombatContext& InContext);
	virtual ~FRepositionState() override = default;
	
	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	
private:
	float StuckTime{ 0.f };
};

// Free Helper - Fire the first weapon that still has ammo(Value).
// Returns true if shot has fired.
bool TryFireWeapon(const APawn& Pawn);
