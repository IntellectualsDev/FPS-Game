//
// Created by josep on 2/14/2024.
//

#ifndef GAME1_BULLET_H
#define GAME1_BULLET_H
namespace raylib{
#include <raylib.h>
}

#include <iostream>

using namespace std;
//TODO implement TTL on bullets and implent textures and animations on collision
class Bullet {
public:
    Bullet(Vector3 temp_position, Vector3 temp_velocity, Vector3 temp_hitbox,bool temp){
        position = temp_position;
        velocity = temp_velocity;
        hitbox = temp_hitbox;
        alive = temp;
//        setBulletModel();
    }

    void UpdatePosition(float x, float y,float z);
    Vector3 getPosition();
    Vector3 getVelocity();
    Vector3 getHitbox();
    void kill();
    bool getAlive();
    BoundingBox getBulletBox();
    void setBulletBox(BoundingBox temp);
    void setBulletModel();
    Model getBulletModel();

private:
    Model bulletModel;
    bool alive;
    BoundingBox bulletBox{};
    Vector3 position{};
    Vector3 velocity{};

    Vector3 hitbox{};
};


#endif //GAME1_BULLET_H
