#include <iostream>
#include <utility>
#include "BBQ_StateMachine.h" 
#include <algorithm> 
#include <string>   
 
StateMachine::StateMachine() {
//                                           curr state    event           new state
    stateDiag_[StateMachine::StateTransition(Idle,         NewClient)]   = Checking;
    stateDiag_[StateMachine::StateTransition(Checking,     PushChicken)] = Chicken;
    stateDiag_[StateMachine::StateTransition(Chicken,      Reject)]      = Beef;
    stateDiag_[StateMachine::StateTransition(Beef,         Reject)]      = Mammoth;
    stateDiag_[StateMachine::StateTransition(Mammoth,      Reject)]      = Idle;
    stateDiag_[StateMachine::StateTransition(Chicken,      Accept)]      = Served;
    stateDiag_[StateMachine::StateTransition(Beef,         Accept)]      = Served;
    stateDiag_[StateMachine::StateTransition(Mammoth,      Accept)]      = Served;
    stateDiag_[StateMachine::StateTransition(Served,       PushIdle)]    = Idle;
    stateDiag_[StateMachine::StateTransition(Idle,         Close)]       = Closed;
    stateDiag_[StateMachine::StateTransition(Served,       Close)]       = Closed;
 
 
    //  current state is initialized to Idle
    currentState_ = Idle;   
}

void StateMachine::internalRecursiveTransitions() {
    //  If current state is: Served change state to Idle
    if (Served == currentState_) {
        processSMEvent(PushIdle);
    }

    //  If current state is: NewClient change state to Chicken
    if (Checking == currentState_) {
        processSMEvent(PushChicken);
    }
}

void StateMachine::processSMEvent (Action event) {
    //  The state machine "state" is: currentState
    StateTransition stateAction(currentState_, event);

    if (stateDiag_.find(stateAction) == stateDiag_.end()) {
        //  Invalid state transition
        onEnterState(NumStates);
        return;
    } 

    //  Execute the action and update the state machine
    onExitState(currentState_);
    currentState_ = stateDiag_[stateAction];
    onEnterState(currentState_);

    internalRecursiveTransitions() ;
}

StateMachine::State StateMachine::getCurrentState() {
    return currentState_;
}

StateMachine::State StateMachine::getInitialState() {
    return Idle;
}