#include "CongestionControl.hpp"

CongestionControl::CongestionControl()
    : state_(State::SLOW_START), cwnd_(1), ssthresh_(64), dupAcks_(0) {}

void CongestionControl::onPacketAcked(bool isDuplicateAck,int numberOfLossPacket) {
    if (isDuplicateAck) {
        dupAcks_++;
        if (state_ == State::FAST_RECOVERY) {
            cwnd_++;
        } else if (dupAcks_ == 3) {
            enterFastRecovery();
        }
    } else {
        dupAcks_ = 0;
        if (state_ == State::FAST_RECOVERY) {
            if(numberOfLossPacket <= 0){
                exitFastRecovery();
            }
            else{
                cwnd_++;
            }
        }

        if (state_ == State::SLOW_START) {
            cwnd_ *= 2;
            if (cwnd_ >= ssthresh_) {
                enterCongestionAvoidance();
            }
        } else if (state_ == State::CONGESTION_AVOIDANCE) {
            cwnd_ += 1 ;
        }
    }
}

void CongestionControl::onTimeout() {
    ssthresh_ = cwnd_ / 2;
    cwnd_ = 1;
    enterSlowStart();
}

uint32_t CongestionControl::getCwnd() const {
    return cwnd_;
}


void CongestionControl::enterSlowStart() {
    state_ = State::SLOW_START;
    std::cout << "Entering Slow Start\n";
}

void CongestionControl::enterCongestionAvoidance() {
    state_ = State::CONGESTION_AVOIDANCE;
    std::cout << "Entering Congestion Avoidance\n";
}

void CongestionControl::enterFastRecovery() {
    ssthresh_ = cwnd_ / 2;
    cwnd_ = ssthresh_ + 3;
    state_ = State::FAST_RECOVERY;
    std::cout << "Entering Fast Recovery\n";
}

void CongestionControl::exitFastRecovery() {
    cwnd_ = ssthresh_ - 1;
    enterCongestionAvoidance();
}

State CongestionControl::getState() const {
    return state_;
}
