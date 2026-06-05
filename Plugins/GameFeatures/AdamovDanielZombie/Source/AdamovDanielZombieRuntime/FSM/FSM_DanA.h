#pragma once
#include "State_DanA.h"
#include "Transition_DanA.h"

#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>

class AAIController;
class UBlackboardComponent;

namespace GameAI::FSM
{
	// Owns and manages States and Transitions.
	// Holds the current State.
	class FSM_DanA final
	{
	public:
		void SetBlackboard(UBlackboardComponent* InBlackboard);
		void SetController(AAIController* InController);
		
		// Transfer ownership
		State_DanA* AddState(std::unique_ptr<State_DanA>&& NewState);
		void AddTransition(State_DanA* From, State_DanA* To, std::function<bool()> Condition);
		
		void Start();
		void Stop();
		void Tick(float DeltaTime);
		
		bool IsRunning() const noexcept;
		
	private:
		std::vector<std::unique_ptr<State_DanA>> StatesContainer{};
		// Multiple Transitions PER state
		std::unordered_map<State_DanA*, std::vector<Transition_DanA>> TransitionsContainer{};
		
		State_DanA* CurrentState{ nullptr };
		UBlackboardComponent* Blackboard{ nullptr };
		AAIController* Controller{ nullptr };
		
		bool bRunning{ false };
		
		void ChangeState(State_DanA* NewState);
	};
}
