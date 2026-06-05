#pragma once
#include "CoreMinimal.h"

namespace Steering
{
	struct FSteeringState
	{
		FVector2D Position{ FVector2D::ZeroVector };
		FVector2D Velocity{ FVector2D::ZeroVector };
		FVector2D Forward { 1.f, 0.f };
		float MaxSpeed{ 400.f };
	};
	
	// Result from steering - has both linear velocity and angle turn
	struct FSteeringOutput
	{
		FVector2D LinearVelocity{ FVector2D::ZeroVector };
		float AngularDeg{ 0.f };
		bool bValid{ true };
	};
	
	class ISteeringBehavior
	{
	public:
		ISteeringBehavior() = default;
		virtual ~ISteeringBehavior() = default;
		virtual FSteeringOutput Calculate(float DeltaT, const FSteeringState& Agent) = 0;

		void SetTargetPosition(const FVector2D& Pos) { Target.Position = Pos; }
		void SetTargetState(const FSteeringState& InState) { Target = InState; }

	protected:
		FSteeringState Target{};
	};
	
	class Seek final : public ISteeringBehavior
	{
	public:
		virtual FSteeringOutput Calculate(float DeltaT, const FSteeringState& Agent) override;
	};
	
	class Flee final : public ISteeringBehavior
	{
	public:
		virtual FSteeringOutput Calculate(float DeltaT, const FSteeringState& Agent) override;
	};
	
	class Face final : public ISteeringBehavior
	{
	public:
		virtual FSteeringOutput Calculate(float DeltaT, const FSteeringState& Agent) override;
	};
	
	class Pursuit final : public ISteeringBehavior
	{
	public:
		virtual FSteeringOutput Calculate(float DeltaT, const FSteeringState& Agent) override;
		
	private:
		float PredictionTime{ 0.3f };
	};
	
	class Evade final : public ISteeringBehavior
	{
	public:
		virtual FSteeringOutput Calculate(float DeltaT, const FSteeringState& Agent) override;
		float GetEvadeRadius() const noexcept { return EvadeRadius; }
		
	private:
		float PredictionTime{ 0.3f };
		float EvadeRadius{ 600.f };
	};
	
	
	FSteeringState MakeState(const APawn& Pawn);
	void Apply(APawn& Pawn, const FSteeringOutput& Out, float DeltaTime, float TurnRateDeg = 360.f);
}
