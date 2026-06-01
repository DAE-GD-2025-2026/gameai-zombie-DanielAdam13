#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Items/ItemType.h"
#include "BTTask_UseItemOfType_DanA.generated.h"

enum class EItemType : uint8;
/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTTask_UseItemOfType_DanA : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_UseItemOfType_DanA();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category="Item")
	EItemType ItemType{ EItemType::Medkit };
	
};
