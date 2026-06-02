#include "BTTask_FindFleeLocation_DanA.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindFleeLocation_DanA::UBTTask_FindFleeLocation_DanA()
{
	NodeName = TEXT( "Find Flee Location" );
	ThreatActorKey.AddObjectFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_FindFleeLocation_DanA, ThreatActorKey), AActor::StaticClass());
	FleeTargetKey.AddVectorFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_FindFleeLocation_DanA, FleeTargetKey));
}

EBTNodeResult::Type UBTTask_FindFleeLocation_DanA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Task execution requires valid AIController, Pawn and BB
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	if (!Pawn || !BB)
		return EBTNodeResult::Failed;

	const UNavigationSystemV1* NavSys{ FNavigationSystem::GetCurrent<UNavigationSystemV1>( Pawn->GetWorld() ) };
	if (!NavSys)
		return EBTNodeResult::Failed;
	
	const FVector Origin{ Pawn->GetActorLocation() };
	const AActor* ThreatActor{ Cast<AActor>( BB->GetValueAsObject( ThreatActorKey.SelectedKeyName ) ) };
	const FVector ThreatLocation{ ThreatActor ? 
		ThreatActor->GetActorLocation() : Origin - Pawn->GetActorForwardVector() * 100.f };
	
	FVector AwayDirection{ Origin - ThreatLocation };
	AwayDirection.Z = 0.f;
	if (!AwayDirection.Normalize(  ))
	{
		AwayDirection = Pawn->GetActorForwardVector();
	}
	
	FNavLocation Best;
	float BestScore{ -FLT_MAX };
	bool bFound{ false };
	
	// Choose the best Candidate for Flee Location out of [CandidateCount] samples
	for (int32 i{}; i < CandidateCount; ++i)
	{
		FNavLocation Candidate;
		if (!NavSys->GetRandomReachablePointInRadius( Origin, FleeRadius, Candidate ))
			continue;
		
		FVector ToCandidate{ Candidate.Location - Origin };
		ToCandidate.Z = 0.f;
		const float Length{ static_cast<float>(ToCandidate.Size()) };
		if (Length < 0.0001f)
			continue;
		
		// How "away" is the candidate
		const float AwayScore{ static_cast<float>( 
			(FVector::DotProduct( ToCandidate / Length, AwayDirection ) + 1.f) * 0.5f ) };
		// How far is the candidate
		const float DistScore{ static_cast<float>(
			FVector::Dist2D( Candidate.Location, ThreatLocation ) / (FleeRadius * 2.f)) };
		
		const float FinalScore{ AwayScore * 0.5f + DistScore * 0.5f };
		if (FinalScore > BestScore)
		{
			BestScore = FinalScore;
			Best = Candidate;
			bFound = true;
		}
	}
	
	if (!bFound)
		return EBTNodeResult::Failed;
	
	BB->SetValueAsVector( FleeTargetKey.SelectedKeyName, Best.Location );
	return EBTNodeResult::Succeeded;
}
