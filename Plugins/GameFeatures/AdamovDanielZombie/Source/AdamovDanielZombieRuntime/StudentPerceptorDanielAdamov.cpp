#include "StudentPerceptorDanielAdamov.h"


UStudentPerceptorDanielAdamov::UStudentPerceptorDanielAdamov()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptorDanielAdamov::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptorDanielAdamov::OnPerceptionUpdated);
	}
}

void UStudentPerceptorDanielAdamov::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, 
	FString::Printf(TEXT("Saw Something!")));
}
