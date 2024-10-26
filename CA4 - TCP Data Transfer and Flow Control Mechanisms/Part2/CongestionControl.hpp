#ifndef CONGESTIONCONTROL_HPP
#define CONGESTIONCONTROL_HPP

#include <cstdint>
#include <iostream>

enum class State {
        SLOW_START,
        CONGESTION_AVOIDANCE,
        FAST_RECOVERY
    };

class CongestionControl {
public:
    uint32_t ssthresh_; 

    CongestionControl();

    void onPacketAcked(bool isDuplicateAck,int numberOfLossPacket);
    void onTimeout();

    uint32_t getCwnd() const;
    State getState() const;

private:
    
    State state_;
    float cwnd_;       
    uint32_t dupAcks_;  

    void enterSlowStart();
    void enterCongestionAvoidance();
    void enterFastRecovery();
    void exitFastRecovery();
};

#endif
