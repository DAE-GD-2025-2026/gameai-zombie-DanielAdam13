#include "Transition_DanA.h"

using namespace GameAI::FSM;

Transition_DanA::Transition_DanA(State_DanA* InFrom, State_DanA* InTo, std::function<bool()> InCondition)
	:m_From( InFrom ),
	m_To( InTo ),
	m_Condition(  std::move(InCondition) )
{
}

State_DanA* Transition_DanA::GetFrom() const noexcept
{
	return m_From;
}

State_DanA* Transition_DanA::GetTo() const noexcept
{
	return m_To;
}

bool Transition_DanA::Evaluate() const noexcept
{
	return m_Condition ? m_Condition() : false;
}
