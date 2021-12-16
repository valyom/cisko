#include <iostream>
#include <utility>
#include "BBQ_StateMachine.hpp" 
#include <algorithm> 
#include <string>   

#include <stdio.h>
#include <string.h>//bzero
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
 
void error(const char *msg)
{
    perror(msg);
    exit(1);
}



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

//class providing separate state processing
BBQ_Serve::BBQ_Serve( ) : newsockfd_(-1) 
    , isEnough_(false)
{
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
        case StateMachine::Idle     : commonInStateCallback (stateIdleEnterAction.c_str());    break;
        case StateMachine::Checking : commonInStateCallback (stateCheckingEnterAction.c_str()); break;
        case StateMachine::Chicken  : commonInStateCallback (stateChickenEnterAction.c_str()); break;
        case StateMachine::Beef     : commonInStateCallback (stateBeefEnterAction.c_str());    break;
        case StateMachine::Mammoth  : commonInStateCallback (stateMammothEnterAction.c_str()); break;
        case StateMachine::Served   : commonInStateCallback (stateServedenterAction.c_str());  break;
        case StateMachine::Closed   : commonInStateCallback (stateClosedEnterAction.c_str());  break;
        default : {
            commonInStateCallback (stateWrongTransitionAction.c_str()); 
        }   
    }
}

void BBQ_Serve::onExitState(const StateMachine::State state) {
    if (Served == state) {
        std::string answer ;
        std::cout << "Enough ? [N/y] : ";
        std::cin >> answer;
        isEnough_ = answer == "y" || answer == "Y";
    }
}

void BBQ_Serve::commonInStateCallback(const char * message) {
    
    if (newsockfd_ != -1) {
        send(newsockfd_, message, strlen(message), 0); 
    }
    else
        std::cout << message << std::endl;
}

bool BBQ_Serve::clientRequest(const char * userInput) {

    if (nullptr != userInput && '\0' != *userInput ) {
        std::string request = userInput;
        transform(request.begin(), request.end(), request.begin(), ::toupper);
        const bool isKnown = requestToevent.end() != requestToevent.find(request) ;
        const StateMachine::Action action = isKnown ? requestToevent.at(request) : WrongInput;
        processSMEvent(action);
    }

    return true;
}


void BBQ_Serve::runServer(int portNum){
    int sockfd, portno=portNum;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
 
     
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  
        error("ERROR opening socket");
       
    // clear address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));

    
    serv_addr.sin_family = AF_INET;
 
    serv_addr.sin_addr.s_addr = INADDR_ANY;  

    //  converted into network byte order
    serv_addr.sin_port = htons(portno);
 
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
 
    bool reconnect = true;
    while (reconnect) {
        reconnect = false;
        // Here, we set the maximum size for the backlog queue to 1.
        listen(sockfd, 1);

        clilen = sizeof(cli_addr); 
        newsockfd_ = accept(sockfd,  (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd_ < 0) 
            error("ERROR on accept"); 

        std::cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr)<< " port " <<  ntohs(cli_addr.sin_port)  << std::endl; 
        std::cout << "server: wait a new client"   << std::endl;

        while (!isEnough_) {
            bzero(buffer,256); 
            n = read(newsockfd_,buffer,255); 
            if (n < 0)
                error("ERROR reading from socket");
            if (strstr(buffer, "Quit")) {
                 reconnect = true;
                 break;   
            }
            clientRequest(buffer); 
        }
        close(newsockfd_);
        newsockfd_ = -1;
    }
    std::cout << std::endl << "Exiting..." << std::endl; 

    close(sockfd);
    sockfd = 0; 
    newsockfd_ = -1;
}

int main (int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    BBQ_Serve bbq;
    bbq.runServer(atoi(argv[1]));
      
    return 0; 
} 

 


