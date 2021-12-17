#include <iostream>
#include <utility>
#include "BBQ_StateMachine.h" 
#include <algorithm> 
#include <string>   
 
StateMachine::StateMachine() {
//                                           curr state    event           new state
    m_stateDiag[StateMachine::StateTransition(Idle,         NewClient)]   = Checking;
    m_stateDiag[StateMachine::StateTransition(Checking,     PushChicken)] = Chicken;
    m_stateDiag[StateMachine::StateTransition(Chicken,      Reject)]      = Beef;
    m_stateDiag[StateMachine::StateTransition(Beef,         Reject)]      = Mammoth;
    m_stateDiag[StateMachine::StateTransition(Mammoth,      Reject)]      = Idle;
    m_stateDiag[StateMachine::StateTransition(Chicken,      Accept)]      = Served;
    m_stateDiag[StateMachine::StateTransition(Beef,         Accept)]      = Served;
    m_stateDiag[StateMachine::StateTransition(Mammoth,      Accept)]      = Served;
    m_stateDiag[StateMachine::StateTransition(Served,       PushIdle)]    = Idle;
    m_stateDiag[StateMachine::StateTransition(Idle,         Close)]       = Closed;
    m_stateDiag[StateMachine::StateTransition(Served,       Close)]       = Closed;
 
 
    //  current state is initialized to Idle
    m_currentState = Idle;   
}

void StateMachine::internalRecursiveTransitions() {
    //  If current state is: Served change state to Idle
    if (Served == m_currentState) {
        processSMEvent(PushIdle);
    }

    //  If current state is: NewClient change state to Chicken
    if (Checking == m_currentState) {
        processSMEvent(PushChicken);
    }
}

void StateMachine::processSMEvent (Event event) {
    //  The state machine "state" is: currentState
    StateTransition stateAction(m_currentState, event);

    if (m_stateDiag.find(stateAction) == m_stateDiag.end()) {
        //  Invalid state transition
        onEnterState(NumStates);
        return;
    } 

    //  Execute the action and update the state machine
    onExitState(m_currentState);
    m_currentState = m_stateDiag[stateAction];
    onEnterState(m_currentState);

    internalRecursiveTransitions() ;
}

StateMachine::State StateMachine::getCurrentState() {
    return m_currentState;
}

StateMachine::State StateMachine::getInitialState() {
    return Idle;
}