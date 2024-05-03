//
// Created by Joseph on 4/28/2024.
//

#include "RemotePlayer.h"

State RemotePlayer::pullState() {
    //TODO: pull correct interpolated state

    return (stateBuffer[0]);
}

void RemotePlayer::addState(State temp) {
    stateBuffer.push_back(temp);
}