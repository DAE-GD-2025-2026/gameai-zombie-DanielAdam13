#pragma once
#include "CoreMinimal.h"
#include "State.h"

struct FCombatContext
{
	FName ThreatKeyName{ TEXT( "ThreatActor" ) };
	float TurnRateDeg{ 120.f };
	float MinDistance{ 250.f };
	float SafeDistance{ 450.f };
	float FireCooldown{ 0.5f };
	float AlignToleranceDeg{ 12.f };
};

class FCombatState : public GameAI::FSM::State
{
public:
	virtual ~FCombatState() override = default; 
	
	explicit FCombatState(const FCombatContext& InContext);
	
protected:
	APawn* GetPawn() const noexcept;
	AActor* GetThreatActor() const noexcept;
	FCombatContext Context;
};

class FEngageState final : public FCombatState
{
public:
	virtual ~FEngageState() override = default;
	
	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	
private:
	float FireTimer{ 0.f };
	
};

class FRepositionState final : public FCombatState
{
public:
	virtual ~FRepositionState() override = default;
	
	virtual void OnUpdate(float DeltaTime) override;
};

// Free Helper - Fire the first weapon that still has ammo(Value).
// Returns true if shot has fired.
bool TryFireWeapon(const APawn& Pawn);
