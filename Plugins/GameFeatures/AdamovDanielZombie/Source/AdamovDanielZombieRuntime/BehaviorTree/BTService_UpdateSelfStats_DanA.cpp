#include "BTService_UpdateSelfStats_DanA.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"

UBTService_UpdateSelfStats_DanA::UBTService_UpdateSelfStats_DanA()
{
	NodeName = TEXT("Update Self Stats");
	Interval = 0.2f;
	RandomDeviation = 0.05f;
	HealthPctKey.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( UBTService_UpdateSelfStats_DanA, HealthPctKey ) );
	StaminaPctKey.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( UBTService_UpdateSelfStats_DanA, StaminaPctKey ) );
}

void UBTService_UpdateSelfStats_DanA::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode( OwnerComp, NodeMemory, DeltaSeconds );
	
	// Task requires a valid AIController and Pawn as well as a valid BB
	const AAIController* Controller{ OwnerComp.GetAIOwner() };
	const APawn* Pawn{ Controller ? Controller->GetPawn() : nullptr };
	UBlackboardComponent* BB{ OwnerComp.GetBlackboardComponent() };
	if (!Pawn || !BB)
		return;
	
	// Health Comp from GameAI_Zombie
	if (const UHealthComponent* Health = Pawn->FindComponentByClass<UHealthComponent>())
	{
		const float MaxHP{ static_cast<float>(FMath::Max( 1, Health->GetMaxHealth() )) };
		BB->SetValueAsFloat( HealthPctKey.SelectedKeyName, Health->GetHealth() / MaxHP );
	}
	
	// Stamina Comp from GameAI_Zombie
	if (const UStaminaComponent* Health = Pawn->FindComponentByClass<UStaminaComponent>())
	{
		const float MaxSt{ static_cast<float>(FMath::Max( 1, Health->GetMaxStamina() )) };
		BB->SetValueAsFloat( HealthPctKey.SelectedKeyName, Health->GetCurrentStamina() / MaxSt );
	}
}
