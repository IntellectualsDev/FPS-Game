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
#include <random>


#endif
//GAME1_PLAYER_H
//TODO finish in game hud
//TODO in game settings menu
//TODO head body_hitbox

using namespace std;
struct playerState{
    Vector3 velocity;
    Vector3 position;
    BoundingBox boundingBox;
};
struct gun{
    string name;
    string type;
    float velocity;
    float rate;
    int maxAmmo;
    int currAmmo;
    bool out;
    float reloadTime = 1.6f;
};
struct bulletHole{
    Vector3 position;
    float ttl = 16.0f;
};
struct target{
    Vector3 position;
    Vector3 head_hitbox = (Vector3){0.5f,0.5f,0.5f};
    BoundingBox headBox;
    Vector3 body_hitbox = (Vector3){1.0f,3.0f,1.0f};
    BoundingBox bodyBox;
    int hp = 200;

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
        currGun = 0;

        sens = 0.391f;
        //TODO differentiate characters
        if(temp_character == "scout"){
            body_hitbox = (Vector3){1.0f,3.0f,1.0f};
            playerBox = (BoundingBox){(Vector3){position.x - body_hitbox.x / 2.0f,
                                                position.y - body_hitbox.y-head_hitbox.y/2.0f,
                                                position.z - body_hitbox.z / 2.0f},
                                      (Vector3){position.x + body_hitbox.x / 2.0f,
                                                position.y -head_hitbox.y/2.0f,
                                                position.z + body_hitbox.z / 2.0f}};
            head_hitbox = (Vector3){0.5f,0.5f,0.5f};
            headbox = (BoundingBox){(Vector3){position.x - head_hitbox.x / 2.0f,
                                              position.y-head_hitbox.y/2.0f,
                                              position.z - head_hitbox.z / 2.0f},
                                    (Vector3){position.x + head_hitbox.x / 2.0f,
                                              position.y + head_hitbox.y/2.0f,
                                              position.z + head_hitbox.z / 2.0f}};
            maxHealth = 150;
            lateralSpeed = 0.05f;
            gun AR;
            AR.name = "Assult Rifle";
            AR.type = "primary";
            AR.velocity = 10.0f;
            AR.rate = 0.1f;
            AR.maxAmmo = 30;
            AR.currAmmo = AR.maxAmmo;
            AR.out = false;
            inventory[0] = AR;
            gun revolver;
            revolver.name = "Pistol";
            revolver.type = "secondary";
            revolver.velocity = 10.0f;
            revolver.rate = 0.3f;
            revolver.maxAmmo = 10;
            revolver.currAmmo = revolver.maxAmmo;
            revolver.out = false;
            inventory[1] = revolver;
            gun sword;
            sword.name = "Knife";
            sword.type = "melee";
            sword.velocity = 0.0f;
            sword.rate = 0.1f;
            sword.maxAmmo = -1;
            sword.currAmmo =-1;
            sword.out = false;
            inventory[2] = sword;
        }else if(temp_character == "heavy"){
            body_hitbox = (Vector3){1.0f,2.0f,1.0f};
            playerBox = (BoundingBox){(Vector3){position.x - body_hitbox.x / 2.0f,
                                                position.y - body_hitbox.y -head_hitbox.y/2.0f,
                                                position.z - body_hitbox.z / 2.0f},
                                      (Vector3){position.x + body_hitbox.x / 2.0f,
                                                position.y -head_hitbox.y/2.0f,
                                                position.z + body_hitbox.z / 2.0f}};
            head_hitbox = (Vector3){0.5f,0.5f,0.5f};
            headbox = (BoundingBox){(Vector3){position.x - head_hitbox.x / 2.0f,
                                                position.y-head_hitbox.y/2.0f,
                                                position.z - head_hitbox.z / 2.0f},
                                      (Vector3){position.x + head_hitbox.x / 2.0f,
                                                position.y + head_hitbox.y/2.0f,
                                                position.z + head_hitbox.z / 2.0f}};
            maxHealth = 250;

            lateralSpeed = 0.025f;
        }
        currentHealth = maxHealth;
        prevState.position = position;
        prevState.velocity = velocity;
        prevState.boundingBox = playerBox;
//        outputBuffer = CircularBuffer<outputState>(); //size needs to be tick rate * transmission time
        //size needs to be max allowable rtt
    };
    bool CheckCollision(BoundingBox playerBB, BoundingBox wallBB, Vector3& separationVector);
    void updateEntities(float dt,vector<BoundingBox> &terrainList);
    Vector3 camera_direction(Camera *tcamera);
    void UpdatePlayer(bool w, bool a, bool s, bool d,Vector2 mouseDelta,bool shoot,bool space,float dt,
                      BoundingBox prevBoundingBox,vector<BoundingBox> &terrainList,vector<BoundingBox> &topBoxVector,
                      bool sprint,bool crouch, int tick,float scroll,bool reload,bool shootHold);
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
    int getCurrentHealth();
    int getMaxHealth();
    array<gun,3> getInventory();
    int getCurrGun();
    void setInventory(int index, gun weapon);
    bool getReloading();
    vector<bulletHole> getBulletHoles();
    vector<target> getTargets();
    void updateTargets();
    void initTargets(int n);

//
//    CircularBuffer<outputState>* getOutputBuffer(){
//        return &outputBuffer;
//    }
//    CircularBuffer<inputState>* getInputBuffer(){
//        return &inputBuffer;
//    }
private:
    vector<target> targets = {};
    float reloadTime = 0.0f;
    bool reloading = false;
//    CircularBuffer<outputState> outputBuffer;
//    CircularBuffer<inputState> inputBuffer;
    vector<bulletHole> bulletHoles = {};
    int cooldownSlidingMax = -1;
    int currentHealth;
    int maxHealth;
    int currGun;
    array<gun,3> inventory;
    bool firstJump=false;
    playerState prevState;
    playerState currState;
    Vector3 crouchingOffset = {0.0f,-2.0f,0.0f};
    float fov;
    float roll = 10.0f;
    bool crouching = false;
    bool sliding = false;
    string character;
    Vector3 head_hitbox{};
    float sens;
    Vector3 separationVector{};
    Vector3 Gravity = {0.0f,-0.03f,0.0f};
    Vector3 Jump = {0.0f,0.8f,0.0f};
    float lateralSpeed;
    float airSpeed = 0.01f;
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
    float slidingFriction = friction;
    BoundingBox sweptAABB;
};
