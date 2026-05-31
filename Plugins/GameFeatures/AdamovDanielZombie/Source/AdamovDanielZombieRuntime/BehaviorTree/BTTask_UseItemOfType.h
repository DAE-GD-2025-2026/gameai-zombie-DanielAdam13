#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Items/ItemType.h"
#include "BTTask_UseItemOfType.generated.h"

enum class EItemType : uint8;
/**
 * 
 */
UCLASS()
class ADAMOVDANIELZOMBIERUNTIME_API UBTTask_UseItemOfType : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_UseItemOfType();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category="Item")
	EItemType ItemType{ EItemType::Medkit };
	
};
