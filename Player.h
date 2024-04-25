//
// Created by josep on 2/8/2024.
//

#ifndef GAME1_PLAYER_H
#define GAME1_PLAYER_H

#include <raylib.h>
#include <raymath.h>

#include "PacketBuffer.h"
#include <iostream>
#include "Bullet.h"
#include "vector"


#endif
//GAME1_PLAYER_H
//TODO implement player collision and hitbox, potentially uI too or atleast hands and a gun
//TODO test implementation of maps and collisions with those
using namespace std;
struct FPSClientState {
    float dt{};
    Vector3 separationVector{};
    bool topCollision = false;
    float Gravity = -0.05;
    float Jump = 0.8f;
    bool grounded = false;
    bool space;
    BoundingBox playerBox{};
    Vector3 hitBox{};
    float coolDown = 0;
    vector<Bullet> entities = {};
    Camera3D camera = {0};
    Vector3 position = (Vector3){0,2,1};
    Vector3 velocity = (Vector3){0,0,0};
    Vector3 hitbox = (Vector3){1.0f,2.0f,1.0f};
    bool alive = true;
    int cameraMode = CAMERA_FIRST_PERSON;
};
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
//        outputBuffer = CircularBuffer<outputState>(); //size needs to be tick rate * transmission time
        //size needs to be max allowable rtt
    };
    bool CheckCollision(BoundingBox playerBB, BoundingBox wallBB, Vector3& separationVector);
    void updateEntities(float dt,vector<BoundingBox> &terrainList);
    Vector3 camera_direction(Camera *tcamera);
    void UpdatePlayer(bool w, bool a, bool s, bool d,Vector2 mouseDelta,bool shoot,bool space,float dt,Vector3 prevPosition,vector<BoundingBox> &terrainList,vector<BoundingBox> &topBoxVector,bool sprint,bool crouch,PacketBuffer& outputBuffer);
    Camera3D * getCamera();
    Vector3 getHitBox();
    void setCameraMode(int temp);
    void setGrounded(bool temp,float dt);
    bool getGrounded();
    Vector3 getVelocity();
    vector<Bullet>* getEntities();
    Vector3 getPosition();
    void startJumpTimer(float dt);
    float getJumpTimer();
    BoundingBox getPlayerBox();
    void setPosition(Vector3 temp);

//
//    CircularBuffer<outputState>* getOutputBuffer(){
//        return &outputBuffer;
//    }
//    CircularBuffer<inputState>* getInputBuffer(){
//        return &inputBuffer;
//    }
private:
//    CircularBuffer<outputState> outputBuffer;
//    CircularBuffer<inputState> inputBuffer;
    Vector3 separationVector{};
    bool topCollision{};
    Vector3 Gravity = {0.0f,-0.05f,0.0f};
    Vector3 Jump = {0.0f,1.0f,0.0f};
    float lateralSpeed = 0.05f;
    bool grounded = false;
    BoundingBox playerBox{};
    float coolDown = 0;
    vector<Bullet> entities = {};
    Camera3D camera{};
    Vector3 position{};
    Vector3 velocity = Vector3Zero();
    Vector3 hitbox{};
    bool alive;
    float JumpTimer;
    float friction = 0.15f;
    BoundingBox sweptAABB;
};
