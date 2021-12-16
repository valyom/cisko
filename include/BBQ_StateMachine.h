#pragma once


#include <map>

// 
//  State machine transitions table
//     curr state    event           new state
    // Idle          NewClient   => Checking;
    // Checking      PushChicken => Chicken;
    // Chicken       Reject      => Beef;
    // Beef          Reject      => Mammoth;
    // Mammoth       Reject      => Idle;
    // Chicken       Accept      => Served;
    // Beef          Accept      => Served;
    // Mammoth       Accept      => Served;
    // Served        PushIdle    => Idle;
    // Idle          Close       => Closed;
    // Served        Close       => Closed;

class StateMachine {
public: 
    enum State { Idle, 
                 Checking,  // in Idle state a new client ask for food and BQQ shop sould check for available meal and propose  
                 Chicken,   // BBQ shop propose a chicken to the client
                 Beef,      // BBQ shop propose a beef to the client
                 Mammoth,   // BBQ shop propose a mammoth to the client
                 Served,    // Client is acceptted one of the proposed meals
                 Closed,    // Program ends (shop is closed)
                 NumStates };


    enum Action { NewClient,   // client sayd "I AM HUNGRY, GIVE ME BBQ"
                  PushChicken, // internal event to transit to state Chicken
                  Reject,      // client sayd  "NO THANKS"
                  Accept,      // client sayd  "I TAKE THAT!!!"  
                  PushIdle,    // internal event to move to state Idle 
                  Close,       // internal event to move to state Close
                  WrongInput,  // internal event to handle unexpected inputs
                  NumEvents };

    StateMachine ();
protected:
    virtual void onEnterState (const State state) {}; // allows the children of the StateMachine class to take soem actions on enter a state
    virtual void onExitState (const State state) {};   // allows the children of the StateMachine class to take soem actions on exit a state

    struct StateTransition {
        StateMachine::State state_;
        StateMachine::Action action_;
        StateTransition(StateMachine::State state, StateMachine::Action action):state_(state), action_(action){}

        friend bool operator<(const StateTransition& sta1, const StateTransition& sta2) {
            return (sta1.state_ < sta2.state_)  || (sta1.state_ == sta2.state_ && sta1.action_ < sta2.action_);
        }
    };

    typedef std::map<StateMachine::StateTransition, StateMachine::State> StateDiagram;
    std::map <StateMachine::Action, std::string>  clientRequests;

    void internalRecursiveTransitions();
public:
   void processSMEvent (Action event);
   State getCurrentState();
 protected:    
    State getInitialState();

    StateDiagram stateDiag_;
    State currentState_; 
};
