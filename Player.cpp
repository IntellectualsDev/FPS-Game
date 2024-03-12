//
// Created by josep on 2/8/2024.
//

#include "Player.h"
#include "Bullet.h"
#include <raymath.h>
#include <algorithm>
void Player::UpdatePlayer(bool w, bool a, bool s, bool d,Vector2 mouseDelta,bool shoot,bool space,float dt, Vector3 prevPosition, vector<BoundingBox> &terrainList,vector<BoundingBox> &topBoxVector) {
    //TODO check collision here to map objects


    if(CheckCollisionBoxes(playerBox,terrainList[3])){
        setGrounded(true);
    }
    else if(topCollision){
        grounded = true;
        topCollision = false;
    }else if (colliding && velocity.y != 0){
        colliding = false;
        grounded = false;
    }else{
        grounded = false;
        space = false;
    }
    if(space && grounded){
        grounded = false;
        velocity = (Vector3){(w)*0.3f -(s)*0.3f,((space)*Jump),(d)*0.3f -(a)*0.3f};
    }else if (!grounded){

        velocity = (Vector3){(w)*0.3f -(s)*0.3f,velocity.y + Gravity,(d)*0.3f -(a)*0.3f};
    }else{
        velocity = (Vector3){(w)*0.3f -(s)*0.3f,0,(d)*0.3f -(a)*0.3f};
    }





    //TODO check for case to set grounded == true
    //TODO implement grounded/jumping movement
    UpdateCameraPro(&camera,
                    (Vector3){velocity.x,velocity.z,velocity.y},
                    (Vector3){
                            mouseDelta.x*0.1f,                            // Rotation: yaw
                            mouseDelta.y*0.1f,                            // Rotation: pitch
                            0.0f                                                // Rotation: roll
                    },
                    GetMouseWheelMove()*2.0f);
    position = camera.position;





    if(shoot && coolDown <= 0.0f){
        coolDown = 0.3;
        cout << "SHOOT!" << endl;
        Bullet temp(Vector3Add(camera.position, Vector3Scale(camera_direction(&camera),0.7f)), Vector3Scale(camera_direction(&camera),5.0f),(Vector3){0.1f,0.1f,0.1f},
                    true);
//        temp.getBulletModel().transform =  MatrixRotateXYZ((Vector3){ DEG2RAD*temp.getVelocity().x, DEG2RAD*temp.getVelocity().y, DEG2RAD*temp.getVelocity().z});
        entities.push_back(temp);
        //TODO deque object instead of vector
    }

    for(int i = 0;i < terrainList.size();i++){
        if(i <3){
            if(CheckCollisionBoxes(playerBox,topBoxVector[i])&&!space){
                position.y = 2+topBoxVector[i].max.y;
                camera.position.y = position.y;
                topCollision = true;
            }else if(CheckCollisionBoxes(getPlayerBox(),terrainList[i]) && i != 3 && !space){
//                if(playerBox.max.x >= terrainList[i].min.x){
//                    position.x = terrainList[i].max.x + hitbox.x/2;
//                } else if(playerBox.min.x <= terrainList[i].max.x){
//                    position.x = terrainList[i].min.x - hitbox.x/2;
//                }
//                if(playerBox.max.z >= terrainList[i].min.z){
//                    position.z = terrainList[i].min.z - hitbox.z/2;
//                } else if(playerBox.min.z <= terrainList[i].max.z){
//                    position.z = terrainList[i].max.z + hitbox.z/2;
//                }
//                camera.position = position;
                position = Vector3Add(Vector3Scale(Vector3Subtract(position,prevPosition),3*dt),position);

            }
        }


    }
    playerBox.min = (Vector3){position.x - hitbox.x/2,
                              position.y - hitbox.y/2-1.0f,
                              position.z - hitbox.z/2};
    playerBox.max = (Vector3){position.x + hitbox.x/2,
                              position.y + hitbox.y/2-0.5f,
                              position.z + hitbox.z/2};
}
Vector3 Player::getHitBox() {
    return this->hitbox;
}
Camera3D * Player::getCamera(){
    return &camera;
}
void Player::setCameraMode(int temp) {

}

vector<Bullet>* Player::getEntities() {
    return &entities;
}

Vector3 Player::camera_direction(Camera *tcamera) {
    return Vector3Normalize(Vector3Subtract(tcamera->target, tcamera->position));
}

void Player::updateEntities(float dt) {
    for (int i = 0; i < entities.size(); i++) {
        //Vector3Subtract(entities[i].getPosition(),this->position)
        if(entities[i].getAlive()){
            Vector3 temp = Vector3Add(
                    entities[i].getPosition(),
                    Vector3Scale(entities[i].getVelocity(), dt*50));
            entities[i].UpdatePosition(temp.x,temp.y,temp.z) ;
        }else{
            entities.erase(entities.begin()+i);


        }
        BoundingBox tempBoundingBox = (BoundingBox){(Vector3){entities[i].getPosition().x - entities[i].getHitbox().x/2,
                                                              entities[i].getPosition().y - entities[i].getHitbox().y/2,
                                                              entities[i].getPosition().z - entities[i].getHitbox().z/2},
                                  (Vector3){entities[i].getPosition().x + entities[i].getHitbox().x/2,
                                            entities[i].getPosition().y+ entities[i].getHitbox().y/2,
                                            entities[i].getPosition().z + entities[i].getHitbox().z/2}};
        entities[i].setBulletBox(tempBoundingBox);

    }
    coolDown -= dt*2;

}
bool Player::CheckCollision(BoundingBox playerBB, BoundingBox wallBB, Vector3& separationVector) {
    // Check if the player's bounding box is not entirely to the left of the wall
    bool notLeftOfWall = playerBB.max.x >= wallBB.min.x;
    float leftSeparation = wallBB.min.x - playerBB.max.x;

    // Check if the player's bounding box is not entirely to the right of the wall
    bool notRightOfWall = playerBB.min.x <= wallBB.max.x;
    float rightSeparation = wallBB.max.x - playerBB.min.x;

    // Check if the player's bounding box is not entirely above the wall
    bool notAboveWall = playerBB.max.y >= wallBB.min.y;
    float aboveSeparation = wallBB.min.y - playerBB.max.y;

    // Check if the player's bounding box is not entirely below the wall
    bool notBelowWall = playerBB.min.y <= wallBB.max.y;
    float belowSeparation = wallBB.max.y - playerBB.min.y;

    // Check if the player's bounding box is not entirely in front of the wall
    bool notInFrontOfWall = playerBB.max.z >= wallBB.min.z;
    float frontSeparation = wallBB.min.z - playerBB.max.z;

    // Check if the player's bounding box is not entirely behind the wall
    bool notBehindWall = playerBB.min.z <= wallBB.max.z;
    float behindSeparation = wallBB.max.z - playerBB.min.z;

    // If all conditions are true, then there is a collision
    if (notLeftOfWall && notRightOfWall && notAboveWall && notBelowWall && notInFrontOfWall && notBehindWall) {
        // Calculate the minimum separation vector
        float minSeparation = std::min({ abs(leftSeparation), abs(rightSeparation), abs(aboveSeparation), abs(belowSeparation), abs(frontSeparation), abs(behindSeparation)});

        if (minSeparation == abs(leftSeparation)) {
            separationVector = { leftSeparation, 0.0f, 0.0f };
        } else if (minSeparation == abs(rightSeparation)) {
            separationVector = { -rightSeparation, 0.0f, 0.0f };
        } else if (minSeparation == abs(aboveSeparation)) {
            separationVector = { 0.0f, aboveSeparation, 0.0f };
        } else if (minSeparation == abs(belowSeparation)) {
            separationVector = { 0.0f, -belowSeparation, 0.0f };
        } else if (minSeparation == abs(frontSeparation)) {
            separationVector = { 0.0f, 0.0f, frontSeparation };
        } else {
            separationVector = { 0.0f, 0.0f, -behindSeparation };
        }

        return true;
    }

    return false;
}

Vector3 Player::getPosition() {
    return position;
}

float Player::getJumpTimer() {
    return JumpTimer;
}

void Player::startJumpTimer(float dt) {

}

BoundingBox Player::getPlayerBox() {
    return playerBox;
}

void Player::setPosition(Vector3 temp) {
    position.x = temp.x;
    position.y = temp.y;
    position.z = temp.z;

}

void Player::setGrounded(bool temp) {
    grounded = temp;
}

bool Player::getGrounded() {
    return grounded;
}

Vector3 Player::getVelocity() {
    return velocity;
}



