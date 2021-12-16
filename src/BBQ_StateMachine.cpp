#include <iostream>
#include <utility>
#include "BBQ_StateMachine.hpp"


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
        std::cout << "server: I bag your pardon." << std::endl;
        return;
    } 

    //  Execute the action and update the state machine
    onExiState(currentState_);
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

//class providing separate state processing
BBQ_Serve::BBQ_Serve()  {
    std::cout << "BBQ Opened" << std::endl; 
    requestToevent[REQ_HUNGRY] = NewClient;
    requestToevent[REQ_ACCEPT] = Accept;
    requestToevent[REQ_REECT]  = Reject;
}

BBQ_Serve::~BBQ_Serve(){
    processSMEvent(Close);
}

void BBQ_Serve::onEnterState(const StateMachine::State state) {
    //separate processing per state 
    switch (state) {
        case StateMachine::Idle     : commonInStateCallback (stateIdleEnterAction.c_str()); break;
        case StateMachine::Checking : commonInStateCallback (stateCheckingEnterAction.c_str()); break;
        case StateMachine::Chicken  : commonInStateCallback (stateChickenEnterAction.c_str()); break;
        case StateMachine::Beef     : commonInStateCallback (stateBeefEnterAction.c_str()); break;
        case StateMachine::Mammoth  : commonInStateCallback (stateMammothEnterAction.c_str()); break;
        case StateMachine::Served   : commonInStateCallback (stateServedenterAction.c_str()); break;
        case StateMachine::Closed   : 
        commonInStateCallback (stateClosedEnterAction.c_str());
         break;
        default : {
        }   
    }
}

void BBQ_Serve::commonInStateCallback(const char * message) {
    std::cout << message << std::endl;
}

bool BBQ_Serve::clientRequest(const char * userInput) {

    const std::string request = nullptr != userInput ? userInput : "";  
    const bool isKnown =  requestToevent.end() != requestToevent.find(request) ;
    const StateMachine::Action action = isKnown ?requestToevent .at(userInput) : WrongInput;

    std::cout << "client: " << userInput << std::endl;
    processSMEvent(action);
    return true;
}
  

 


