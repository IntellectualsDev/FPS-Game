//
// Created by Joseph on 4/28/2024.
//

#ifndef FPS_GAME_REMOTEPLAYER_H
#define FPS_GAME_REMOTEPLAYER_H

#include <vector>
#import "raylib.h"
#import "raymath.h"
#import "Bullet.h"
struct State{
    Vector3 temp_position;
    Vector3 temp_facing;
    int temp_uid; //TODO change

};
class RemotePlayer {
public:
    RemotePlayer(Vector3 position, Vector3 facing, int uid, Vector3 hitbox){
        this->position = position;
        this->facing = facing;
        this->uid = uid;
        this->hitbox = hitbox;
        alive = true;
        boundingBox.min = (Vector3){position.x - hitbox.x/2,
                                    position.y - hitbox.y/2-1.0f,
                                    position.z - hitbox.z/2};
    }

    void addState(State temp);
    State pullState();
private:
    vector<State> stateBuffer;
    Vector3 position{};
    Vector3 facing{};
    int uid;
    bool alive;
    float rtt;
    Vector3 hitbox;
    BoundingBox boundingBox;
    vector<Bullet> entities{};
};


#endif //FPS_GAME_REMOTEPLAYER_H