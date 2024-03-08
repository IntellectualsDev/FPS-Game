//
// Created by josep on 2/8/2024.
//

#ifndef GAME1_PLAYER_H
#define GAME1_PLAYER_H

#include <raylib.h>
#include <iostream>
#include "Bullet.h"
#include "vector"
#include <raylib.h>
#include <raymath.h>
#endif //GAME1_PLAYER_H
//TODO implement player collision and hitbox, potentially uI too or atleast hands and a gun
//TODO test implementation of maps and collisions with those
using namespace std;

class Player {

public:
    Player(Vector3 temp_postion, Vector3 temp_velocity, Vector3 temp_hitbox1){
        position = temp_postion;
        velocity = temp_velocity;
        hitbox = temp_hitbox1;
        camera = {0};
        camera.position = position;
        camera.target = (Vector3){10.0f, 2.0f, 10.0f};
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.fovy = 60.0f;                                // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE;
        alive = true;
        JumpTimer = 0.0f;
        playerBox = (BoundingBox){(Vector3){position.x - hitbox.x/2,
                                            position.y - hitbox.y/2,
                                            position.z - hitbox.z/2},
                                  (Vector3){position.x + hitbox.x/2,
                                            position.y + hitbox.y/2,
                                            position.z + hitbox.z/2}};
    };
    void updateEntities(float dt);
    Vector3 camera_direction(Camera *tcamera);
    void UpdatePlayer(bool w, bool a, bool s, bool d,Vector2 mouseDelta,bool shoot,bool space,float dt);
    Camera3D * getCamera();
    Vector3 getHitBox();
    void setCameraMode(int temp);
    vector<Bullet>* getEntities();
    Vector3 getPosition();
    void startJumpTimer(float dt);
    float getJumpTimer();
    BoundingBox getPlayerBox();
    void setPosition(Vector3 temp);
private:
    float Gravity = -1.0f;
    float Jump = 1.0f;
    bool grounded = true;
    BoundingBox playerBox;
    float coolDown = 0;
    vector<Bullet> entities = {};
    Camera3D camera{};
    Vector3 position{};
    Vector3 velocity{};
    Vector3 hitbox{};
    bool alive;
    int cameraMode = CAMERA_FIRST_PERSON;
    float JumpTimer;
};
