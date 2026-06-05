#pragma once

class AAIController;
class UBlackboardComponent;

namespace GameAI::FSM
{
	// Base State class
	class State_DanA
	{
	public:
		virtual ~State_DanA() = default;
		virtual void OnEnter() {};
		virtual void OnUpdate(float DeltaTime) {};
		virtual void OnExit() {};
		
		void SetBlackboard(UBlackboardComponent* InBlackboard) { Blackboard = InBlackboard; }
		void SetController(AAIController* InController) { Controller = InController; }
		
	protected:
		UBlackboardComponent* GetBlackboard() const { return Blackboard; }
		AAIController* GetController() const { return Controller; }
		
	private:
		UBlackboardComponent* Blackboard{ nullptr};
		AAIController* Controller{nullptr};
		
	};
}

