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
//TODO finish in game hud
//TODO in game settings menu
//TODO head body_hitbox

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
    Player(Vector3 temp_postion,string temp_character,float temp_fov){

        velocity = Vector3Zero();

        camera = {0};

        //TODO depending if player 1 or 2
        position = temp_postion;
        camera.position = position;

        camera.target = (Vector3){10.0f, 2.0f, 10.0f};
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        fov = temp_fov;
        camera.fovy = fov;                                // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE;
        alive = true;
        JumpTimer = 0.0f;


        sens = 0.391f;
        //TODO differentiate characters
        if(temp_character == "scout"){
            body_hitbox = (Vector3){1.0f,3.0f,1.0f};
            playerBox = (BoundingBox){(Vector3){position.x - body_hitbox.x / 2,
                                                position.y - body_hitbox.y-head_hitbox.y/2,
                                                position.z - body_hitbox.z / 2},
                                      (Vector3){position.x + body_hitbox.x / 2,
                                                position.y -head_hitbox.y/2,
                                                position.z + body_hitbox.z / 2}};
            head_hitbox = (Vector3){0.5f,0.5f,0.5f};
            headbox = (BoundingBox){(Vector3){position.x - head_hitbox.x / 2,
                                              position.y-head_hitbox.y/2,
                                              position.z - head_hitbox.z / 2},
                                    (Vector3){position.x + head_hitbox.x / 2,
                                              position.y + head_hitbox.y/2,
                                              position.z + head_hitbox.z / 2}};
        }else if(temp_character == "heavy"){
            body_hitbox = (Vector3){1.0f,2.0f,1.0f};
            playerBox = (BoundingBox){(Vector3){position.x - body_hitbox.x / 2,
                                                position.y - body_hitbox.y -head_hitbox.y/2,
                                                position.z - body_hitbox.z / 2},
                                      (Vector3){position.x + body_hitbox.x / 2,
                                                position.y -head_hitbox.y/2,
                                                position.z + body_hitbox.z / 2}};
            head_hitbox = (Vector3){0.5f,0.5f,0.5f};
            headbox = (BoundingBox){(Vector3){position.x - head_hitbox.x / 2,
                                                position.y-head_hitbox.y/2,
                                                position.z - head_hitbox.z / 2},
                                      (Vector3){position.x + head_hitbox.x / 2,
                                                position.y + head_hitbox.y/2,
                                                position.z + head_hitbox.z / 2}};
        }
//        outputBuffer = CircularBuffer<outputState>(); //size needs to be tick rate * transmission time
        //size needs to be max allowable rtt
    };
    bool CheckCollision(BoundingBox playerBB, BoundingBox wallBB, Vector3& separationVector);
    void updateEntities(float dt,vector<BoundingBox> &terrainList);
    Vector3 camera_direction(Camera *tcamera);
    void UpdatePlayer(bool w, bool a, bool s, bool d,Vector2 mouseDelta,bool shoot,bool space,float dt,Vector3 prevPosition,vector<BoundingBox> &terrainList,vector<BoundingBox> &topBoxVector,bool sprint,bool crouch,PacketBuffer& outputBuffer);
    Camera3D * getCamera();
    Vector3 getBodyHitBox();
    Vector3 getHeadHitBox();
    float getFOV();
    void setFOV(float temp);
    void setCameraMode(int temp);
    Vector3 getVelocity();
    vector<Bullet>* getEntities();
    Vector3 getPosition();
    void startJumpTimer(float dt);
    float getJumpTimer();
    BoundingBox getHeadBox();
    BoundingBox getPlayerBox();
    void setPosition(Vector3 temp);
    void setSens(float temp);
    float getSense();

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
    float velocityCrouchCounter = 0.0f;
    Vector3 crouchingOffset = {0.0f,-2.0f,0.0f};
    float fov;
    bool crouching = false;
    string character;
    Vector3 head_hitbox{};
    float sens;
    Vector3 separationVector{};
    Vector3 Gravity = {0.0f,-0.05f,0.0f};
    Vector3 Jump = {0.0f,1.0f,0.0f};
    float lateralSpeed = 0.05f;
    bool grounded = false;
    BoundingBox playerBox{};
    BoundingBox headbox{};
    float coolDown = 0;
    vector<Bullet> entities = {};
    Camera3D camera{};
    Vector3 position{};
    Vector3 velocity = Vector3Zero();
    Vector3 body_hitbox{};
    bool alive;
    float JumpTimer;
    float friction = 0.15f;
    BoundingBox sweptAABB;
};
