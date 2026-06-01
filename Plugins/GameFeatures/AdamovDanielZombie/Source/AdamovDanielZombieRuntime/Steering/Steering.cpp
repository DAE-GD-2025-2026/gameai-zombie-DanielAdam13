#include "D:\GitHub\ZombieGame_GameAI\gameai-zombie-DanielAdam13\Intermediate\Build\Win64\x64\GameAI_ZombieEditor\Development\UnrealEd\SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.h"
#include "Steering.h"

#include "GameFramework/FloatingPawnMovement.h"

Steering::FSteeringOutput Steering::Seek::Calculate(float DeltaT, const FSteeringState& Agent)
{
	FSteeringOutput Out;
	Out.LinearVelocity = (Target.Position - Agent.Position).GetSafeNormal() * Agent.MaxSpeed;
	return Out;
}

Steering::FSteeringOutput Steering::Flee::Calculate(float DeltaT, const FSteeringState& Agent)
{
	FSteeringOutput Out;
	Out.LinearVelocity = (Agent.Position - Target.Position).GetSafeNormal() * Agent.MaxSpeed;
	return Out;
}

Steering::FSteeringOutput Steering::Face::Calculate(float DeltaT, const FSteeringState& Agent)
{
	FSteeringOutput Out;
	FVector2D ToTarget{ Target.Position - Agent.Position };
	if (ToTarget.IsNearlyZero())
		return Out;
	ToTarget.Normalize();
	
	const FVector2D Forward{ Agent.Forward.GetSafeNormal() };
	const float Dot{ FMath::Clamp( 
		static_cast<float>(FVector2D::DotProduct(Forward, ToTarget) ), -1.f, 1.f) };
	const float Angle{ FMath::Acos( Dot ) };
	const float Sign{ static_cast<float>(FMath::Sign( Forward.X * ToTarget.Y - Forward.Y * ToTarget.X )) };
	
	Out.AngularDeg = FMath::RadiansToDegrees( Angle * Sign );
	return Out;
}

Steering::FSteeringOutput Steering::Pursuit::Calculate(float DeltaT, const FSteeringState& Agent)
{
	const FVector2D Predicted{ Target.Position + Target.Velocity * PredictionTime };
	FSteeringOutput Out;
	Out.LinearVelocity = (Predicted - Agent.Position).GetSafeNormal() * Agent.MaxSpeed;
	return Out;
}

Steering::FSteeringOutput Steering::Evade::Calculate(float DeltaT, const FSteeringState& Agent)
{
	const FVector2D Predicted{ Target.Position + Target.Velocity * PredictionTime };
	FSteeringOutput Out;
	Out.LinearVelocity = (Predicted - Agent.Position).GetSafeNormal() * Agent.MaxSpeed;
	Out.bValid = FVector2D::DistSquared( Agent.Position, Predicted ) <= EvadeRadius * EvadeRadius;
	return Out;
}

Steering::FSteeringState Steering::MakeState(const APawn& Pawn)
{
	FSteeringState S;
	const FVector Loc{ Pawn.GetActorLocation() };
	const FVector Vel{ Pawn.GetVelocity() };
	const FVector Fwd{ Pawn.GetActorForwardVector() };
	S.Position = FVector2D(Loc.X, Loc.Y);
	S.Velocity = FVector2D(Vel.X, Vel.Y);
	S.Forward  = FVector2D(Fwd.X, Fwd.Y);
	
	if (const UFloatingPawnMovement* FloatingMove = Pawn.FindComponentByClass<UFloatingPawnMovement>())
	{
		S.MaxSpeed = FloatingMove->GetMaxSpeed();
	}
	return S;
}

void Steering::Apply(APawn& Pawn, const FSteeringOutput& Out, float DeltaTime, float TurnRateDeg)
{
	// --- Linear Velocity ---
	if (!Out.LinearVelocity.IsNearlyZero())
	{
		const FVector Dir(Out.LinearVelocity.X, Out.LinearVelocity.Y, 0.f);
		// const float Scale{ static_cast<float>(FMath::Clamp( 
		// 	Out.LinearVelocity.Size() / 
		// 	FMath::Max( 1.f, MakeState(Pawn).MaxSpeed ), 
		// 	0.f, 1.f )) };
		
		Pawn.AddMovementInput( Dir.GetSafeNormal(), 1.f );
	}
	
	// --- Angular ---
	if (!FMath::IsNearlyZero( Out.AngularDeg ))
	{
		const float Step{ static_cast<float>( FMath::Clamp( Out.AngularDeg, 
			-TurnRateDeg * DeltaTime, TurnRateDeg * DeltaTime ) )};
		FRotator Rotator{ Pawn.GetActorRotation() };
		Rotator.Yaw += Step;
		Pawn.SetActorRotation( Rotator );
	}
}
