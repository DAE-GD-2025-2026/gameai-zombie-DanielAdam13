#pragma once

#include <functional>

namespace GameAI::FSM
{
	class State_DanA;
	
	class Transition_DanA
	{
	public:
		Transition_DanA(State_DanA* InFrom, State_DanA* InTo, std::function<bool()> InCondition);
		
		State_DanA* GetFrom() const noexcept;
		State_DanA* GetTo() const noexcept;
		bool Evaluate() const noexcept;
		
	private:
		State_DanA* m_From{ nullptr };
		State_DanA* m_To{ nullptr };
		std::function<bool()> m_Condition;
	
	};
}

