#include "BTTask_FindDiscoveryLocation_DanA.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindDiscoveryLocation_DanA::UBTTask_FindDiscoveryLocation_DanA()
{
	NodeName = TEXT( "Find Discovery Location" );
	TargetLocationKey.AddVectorFilter( 
		this, GET_MEMBER_NAME_CHECKED( UBTTask_FindDiscoveryLocation_DanA, TargetLocationKey ));
}

EBTNodeResult::Type UBTTask_FindDiscoveryLocation_DanA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task requires a valid AIController and Pawn as well as a valid Nav System
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	if (!Pawn)
		return EBTNodeResult::Failed;
	UNavigationSystemV1* NavSys{ FNavigationSystem::GetCurrent<UNavigationSystemV1>( Pawn->GetWorld() ) };
	if (!NavSys)
		return EBTNodeResult::Failed;
	
	const FVector Origin{ Pawn->GetActorLocation() };
	
	FVector Heading{ Pawn->GetVelocity() };
	Heading.Z = 0.f;
	if (!Heading.Normalize())
	{
		Heading = Pawn->GetActorForwardVector();
	}
	
	FNavLocation BestLocation;
	float BestScore{ -FLT_MAX };
	bool bFound{ false };
	
	for (int32 i{}; i < CandidateCount; ++i)
	{
		FNavLocation Candidate;
		
		// Skip if not reachable
		if (!NavSys->GetRandomReachablePointInRadius( Origin, ExploreRadius, Candidate ))
		{
			continue;
		}
		
		// Evaluate if this candidate location is better than the one already stored
		FVector ToCandidate{ Candidate.Location - Origin };
		ToCandidate.Z = 0.f;
		const float Distance{ static_cast<float>(ToCandidate.Size()) };
		if (Distance < 0.0001f)
			continue;
		
		const FVector Dir{ ToCandidate / Distance };
		const float DistanceScore{ Distance / ExploreRadius };
		const float HeadingScore{ static_cast<float>((FVector::DotProduct( Dir, Heading ) + 1.f) * 0.5f) };
		
		const float Score{ FMath::Lerp(DistanceScore, HeadingScore, HeadingBias) };
		if (Score > BestScore)
		{
			BestScore = Score;
			BestLocation = Candidate;
			bFound = true;
		}
	}
	
	if (!bFound)
		return EBTNodeResult::Failed;
	
	OwnerComp.GetBlackboardComponent()->SetValueAsVector( TargetLocationKey.SelectedKeyName, BestLocation );
	return EBTNodeResult::Succeeded;
}
