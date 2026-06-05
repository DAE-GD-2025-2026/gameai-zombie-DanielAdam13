#include "FSM_DanA.h"

#include "Transition_DanA.h"

using namespace GameAI::FSM;

void FSM_DanA::SetBlackboard(UBlackboardComponent* InBlackboard)
{
	Blackboard = InBlackboard;
}

void FSM_DanA::SetController(AAIController* InController)
{
	Controller = InController;
}

State_DanA* FSM_DanA::AddState(std::unique_ptr<State_DanA>&& NewState)
{
	if (!NewState) 
		return nullptr;
	
	NewState->SetBlackboard( Blackboard );
	NewState->SetController( Controller );
	
	State_DanA* RawState{ NewState.get() };
	StatesContainer.push_back( std::move(NewState) );
	return RawState;
}

void FSM_DanA::AddTransition(State_DanA* From, State_DanA* To, std::function<bool()> Condition)
{
	if (!From || !To || !Condition)
		return;
	
	TransitionsContainer[From].emplace_back( From, To, Condition );
}

void FSM_DanA::Start()
{
	if (bRunning || StatesContainer.empty())
		return;
	
	bRunning = true;
	
	// Start first state
	CurrentState = StatesContainer.front().get();
	if (CurrentState)
		CurrentState->OnEnter();
}

void FSM_DanA::Stop()
{
	if (!bRunning)
		return;
	
	// Exit current state as soon as possible
	if (CurrentState)
		CurrentState->OnExit();
	
	CurrentState = nullptr;
	bRunning = false;
}

void FSM_DanA::Tick(float DeltaTime)
{
	if (!bRunning || !CurrentState)
		return;
	
	CurrentState->OnUpdate( DeltaTime );
	
	// Evaluate ongoing transitions:
	auto It{TransitionsContainer.find( CurrentState )};
	if (It != TransitionsContainer.end())
	{
		// First one to be evaluated true
		for (const Transition_DanA& t : It->second)
		{
			if (t.Evaluate())
			{
				ChangeState( t.GetTo() ); // One change per tick
				break;
			}
		}
	}
}

bool FSM_DanA::IsRunning() const noexcept
{
	return bRunning;
}

void FSM_DanA::ChangeState(State_DanA* NewState)
{
	if (CurrentState == NewState)
		return;
	
	// 1.
	if (CurrentState) 
		CurrentState->OnExit();
	// 2.
	CurrentState = NewState;
	// 3.
	CurrentState->OnEnter();
}
