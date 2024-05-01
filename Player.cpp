//
// Created by josep on 2/8/2024.
//

#include "Player.h"
#include "Bullet.h"

#include <raymath.h>
#include <algorithm>
//TODO IMPLEMENT CIRCULAR BUFFER OF HISTORY OF PREDICTIONS
//make a class that implements this so its easier to genralize
//one buffer includes history of predictions from the last ack'd packet(updated by server from input buffer)
//one buffer is for the out buffer handled by the output thread and size is detrmined based on tick rate
//TODO IMPLEMENT OUTPUT BUFFER FOR PACKETS
//render player locally same as before
//send a packet every (MATH) or if a collision/shoot has been encountered
//put the packet into the output buffer (queue)
//output thread handles sending
//packet format includes previous inputs inorder to mitigate packet loss
//send tick number and dt and tick (on join most likely)
//TODO IMPLEMENT INPUT BUFFER FOR PACKETS
//on reception of a packet read the header (idk)
//on SNAPSHOT:
//parse the packet and place the other player packet into its input buffer
//parse the packet and find the part relating to the local player
//check the tick number of the packet and (MATH) determine which local tick that is with the dt parameter
//check the packet you locally predicted against the received packet and if they are the same then set the tail of the buffer to that packet
//if the states are different then you must revert to that state
//TODO INTERPOLATE OTHER PLAYER
//make a class inheriting entity and define it to only be updated according to packest recieved from the server
//it should have a buffer (queue) of snapshot packets from the server and render a (MATH) amount of time behind the most recently recieved packet
//it should have x y z positions and direction x y z
//it also has a vector of bullet entities that will be rendered in the main loop
//once you pull out a packet look into the queue and see where the next position should be and interpolate to that position
//this part requires the packet to contain the other clients dt in order to ensure you interpolate between snapshots correctly
void Player::UpdatePlayer(bool w, bool a, bool s, bool d,Vector2 mouseDelta,bool shoot,bool space,float dt, BoundingBox prevBoundingBox, vector<BoundingBox> &terrainList,vector<BoundingBox> &topBoxVector,bool sprint,bool crouch,PacketBuffer& outputBuffer,int tick) {

    //always apply gravity
    //camera_direction accounts for direction
    //speed of character *velocity vector for input is resulting velocity from movement
    //if a character is colliding with the ground they have friction in the opposing direction of velocity which scales (maybe?)
    //if a character is not colliding with anything they have gravity ontop of their input velocity
    //if a player jumps they gain an impulse to their y velocity
    //TODO: decrease x and z velocity if jumps are successive
    if(tick == cooldownSlidingMax){
        cooldownSlidingMax = -1;
    }
    if(crouch && !crouching){
//        position = {0.0f,5.0f,1.0f};
//        camera.position = position;
//        camera.target = {10.0f, 2.0f, 10.0f};
//        velocity = Vector3Zero();
        //TODO:implement sliding/crouching
        camera.position = Vector3Add(camera.position,crouchingOffset);
        camera.target = Vector3Add(camera.target,crouchingOffset);
        body_hitbox = Vector3Add(body_hitbox,crouchingOffset);
        crouching = true;
//        friction = 0.015f;

        if(grounded && sprint && cooldownSlidingMax == -1){
            velocityCrouchCounter = max(abs(velocity.x),abs(velocity.z))*0.7f;
            cout << "SLIDING!!!!!!!!!!!!!!!!! "<<velocityCrouchCounter << endl;
            cooldownSlidingMax = (tick+50)%60;
        }else{

        }
        lateralSpeed = lateralSpeed/2.0f;
    }else if(!crouch && crouching){
        camera.position = Vector3Subtract(camera.position,crouchingOffset);
        camera.target = Vector3Subtract(camera.target,crouchingOffset);
        body_hitbox = Vector3Subtract(body_hitbox,crouchingOffset);

        crouching = false;
        lateralSpeed = lateralSpeed*2.0f;
    }
    if(velocity.y == 0.0f){
        velocity = Vector3Add(velocity, Vector3Scale(Jump,(float)space));
    }
    velocity = Vector3Add(velocity,Gravity);

    velocity = Vector3Add(velocity,Vector3Scale((Vector3){(float)-(velocity.x),0.0f,(float)-(velocity.z)}, friction));
    velocity = Vector3Add(velocity,Vector3Scale((Vector3){(float)(w-s),0.0f,(float)(d-a)},lateralSpeed));
    velocity = Vector3Add(velocity,Vector3Multiply((Vector3){(float)sprint,0.0f,(float)sprint}, Vector3Scale(velocity,lateralSpeed)));
    velocity = Vector3Multiply(velocity,(Vector3){velocityCrouchCounter+1.0f,1.0f,velocityCrouchCounter+1.0f});
//    friction += velocityCrouchCounter/500.0f;
//    cout << "friction : " << friction << endl;
//    if(friction >= 0.15f){
//        friction = 0.15f;
//    }
    velocityCrouchCounter -= friction/4.0f;
    if(velocityCrouchCounter <= 0.0f){
        velocityCrouchCounter = 0.0f;
    }
    if(((w || a || s || d || space) && !crouching )|| camera.fovy != fov){
        if(sprint){
            camera.fovy = camera.fovy+1.0f;
            if(camera.fovy > fov*1.2f){
                camera.fovy = fov*1.2f;
            }
        }else{
            camera.fovy = camera.fovy-3.0f;
            if(camera.fovy < fov){
                camera.fovy = fov;
            }
        }
    }else{
        camera.fovy = fov;
    }

    Vector3 prevPosition = position;
    UpdateCameraPro(&camera,
                    (Vector3){velocity.x,velocity.z,velocity.y},
                    (Vector3){
                            mouseDelta.x*sens ,                            // Rotation: yaw
                            mouseDelta.y*sens,                            // Rotation: pitch
                            0.0f                                                // Rotation: roll
                    },
                    GetMouseWheelMove());

    if(shoot && coolDown <= 0.0f){
        coolDown = 0.3;
        //TODO
        //add weapon slots and check current inventory index to spawn correct bullet
        Bullet temp(Vector3Add(camera.position, Vector3Scale(camera_direction(&camera),0.7f)), Vector3Scale(camera_direction(&camera),5.0f),
                    Vector3Scale((Vector3){0.1f,0.1f,0.1f},10.0f),true,1.0f,(Vector3){0.0f,0.0f,0.0f});// TODO figure out how to rotate hitboxes
        entities.push_back(temp);
        //TODO deque object instead of vector
    }


    position = camera.position;
    playerBox.min = (Vector3){position.x - body_hitbox.x / 2.0f,
                              position.y - body_hitbox.y-head_hitbox.y/2.0f ,
                              position.z - body_hitbox.z / 2.0f};
    playerBox.max = (Vector3){position.x + body_hitbox.x / 2.0f,
                              position.y-head_hitbox.y/2.0f,
                              position.z + body_hitbox.z / 2.0f};
    headbox = (BoundingBox){(Vector3){position.x - head_hitbox.x / 2.0f,
                                      position.y-head_hitbox.y/2.0f,
                                      position.z - head_hitbox.z / 2.0f},
                            (Vector3){position.x + head_hitbox.x / 2.0f,
                                      position.y + head_hitbox.y/2.0f,
                                      position.z + head_hitbox.z / 2.0f}};

    sweptAABB.min = Vector3Min(prevBoundingBox.min,playerBox.min);
    sweptAABB.max = Vector3Max(prevBoundingBox.max,playerBox.max);

    grounded = false;
//    for(auto & i : terrainList){
//        if(CheckCollision(playerBox,i,separationVector)){
//            position = Vector3Add(position,separationVector);
//            velocity = (Vector3){separationVector.x != 0.0f ? 0.0f : velocity.x,
//                                 separationVector.y != 0.0f ? 0.0f : velocity.y,
//                                 separationVector.z != 0.0f ? 0.0f : velocity.z};
//            camera.position = position;
//            camera.target = Vector3Add(camera.target,separationVector);
//        }
//    }

//    sweptAABB.min = Vector3Subtract(Vector3Min(prevPosition,position), (Vector3){body_hitbox.x / 2,
//                                                                                 body_hitbox.y-head_hitbox.y/2 ,
//                                                                                 body_hitbox.z / 2});
//    sweptAABB.max = Vector3Add(Vector3Max(prevPosition,position), (Vector3){body_hitbox.x / 2,
    cout <<"Min: " <<sweptAABB.min.y << endl;
    cout << "Pre: x-" << position.x << ", y-"<< position.y<< ", z-"<< position.z<<endl;
    for(auto & i : terrainList){
        if(CheckCollision(sweptAABB,i,separationVector)){

            cout << separationVector.y << endl;
            cout << "working" << endl;
            position = Vector3Add(position,separationVector);
            velocity = (Vector3){separationVector.x != 0.0f ? 0.0f : velocity.x,
                                 separationVector.y != 0.0f ? 0.0f : velocity.y,
                                 separationVector.z != 0.0f ? 0.0f : velocity.z};
            camera.position = position;
            camera.target = Vector3Add(camera.target,separationVector);
        }

    }
    cout << "Post: x-" << position.x << ", y-"<< position.y<< ", z-"<< position.z<<endl;
    playerBox.min = (Vector3){position.x - body_hitbox.x / 2.0f,
                              position.y - body_hitbox.y-head_hitbox.y/2.0f,
                              position.z - body_hitbox.z / 2.0f};
    playerBox.max = (Vector3){position.x + body_hitbox.x / 2.0f,
                              position.y -head_hitbox.y/2.0f,
                              position.z + body_hitbox.z / 2.0f};
    headbox = (BoundingBox){(Vector3){position.x - head_hitbox.x / 2.0f,
                                      position.y-head_hitbox.y/2.0f,
                                      position.z - head_hitbox.z / 2.0f},
                            (Vector3){position.x + head_hitbox.x / 2.0f,
                                      position.y + head_hitbox.y/2.0f,
                                      position.z + head_hitbox.z / 2.0f}};

    updateEntities(dt,terrainList);
    coolDown -= dt;
}
Vector3 Player::getBodyHitBox() {
    return this->body_hitbox;
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

void Player::updateEntities(float dt,vector<BoundingBox> &terrainList) {
    for(auto & j : terrainList){
        for(auto & entity : entities){
            entity.setSweptBulletBox((BoundingBox){{Vector3Subtract(Vector3Min(entity.getPrevPosition(),entity.getPosition()), Vector3Scale(entity.getHitbox(),0.5f))},
                                                   {Vector3Add(Vector3Max(entity.getPrevPosition(),entity.getPosition()), Vector3Scale(entity.getHitbox(),0.5f))}});
            //check for bullet collisions with all terrain
            if(CheckCollisionBoxes(entity.getSweptBulletBox(), j)) {
                cout << "Hit:" << entity.getPosition().x << " " << entity.getPosition().y << " " << entity.getPosition().z << endl;
                entity.kill();
                cout << entity.getAlive() << endl;
            }
        }
    }
    for (int i = 0; i < entities.size(); i++) {
        //Vector3Subtract(entities[i].getPosition(),this->position)
        if(entities[i].getAlive()){
            Vector3 temp = Vector3Add(
                    entities[i].getPosition(),
                    Vector3Scale(entities[i].getVelocity(), entities[i].getSpeed()));
            entities[i].UpdatePosition(temp.x,temp.y,temp.z) ;
        }else{
            entities.erase(entities.begin()+i);


        }
        BoundingBox tempBoundingBox = (BoundingBox){Vector3Subtract((Vector3){entities[i].getPosition().x - entities[i].getHitbox().x/2,
                                                                              entities[i].getPosition().y - entities[i].getHitbox().y/2,
                                                                              entities[i].getPosition().z - entities[i].getHitbox().z/2},entities[i].getRotation()),
                                                    Vector3Add((Vector3){entities[i].getPosition().x + entities[i].getHitbox().x/2,
                                                                         entities[i].getPosition().y+ entities[i].getHitbox().y/2,
                                                                         entities[i].getPosition().z + entities[i].getHitbox().z/2},entities[i].getRotation())};
        entities[i].setBulletBox(tempBoundingBox);

    }

}
bool Player::CheckCollision(BoundingBox playerBB, BoundingBox wallBB, Vector3& separationVector) {
    // Check if the player's bounding box is not entirely to the left of the wall
    bool notLeftOfWall = playerBB.max.x >= wallBB.min.x;
    float leftSeparation = wallBB.min.x - playerBB.max.x;

    // Check if the player's bounding box is not entirely to the right of the wall
    bool notRightOfWall = playerBB.min.x <= wallBB.max.x;
    float rightSeparation =  - wallBB.max.x + playerBB.min.x;

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
    float behindSeparation =  - wallBB.max.z + playerBB.min.z;

    // If all conditions are true, then there is a collision
    if (notLeftOfWall && notRightOfWall && notAboveWall && notBelowWall && notInFrontOfWall && notBehindWall) {
        // Calculate the minimum separation vector
        float minSeparation = std::min({ abs(leftSeparation), abs(rightSeparation), abs(aboveSeparation), abs(belowSeparation), abs(frontSeparation), abs(behindSeparation)});

        if (minSeparation == abs(leftSeparation)) {
            separationVector = { leftSeparation, 0.0f, 0.0f };

        } else if (minSeparation == abs(rightSeparation)) {
            separationVector = { -rightSeparation, 0.0f, 0.0f };

        } else if (minSeparation == abs(aboveSeparation)) {
            separationVector = { 0.0f, -aboveSeparation, 0.0f };

        } else if (minSeparation == abs(belowSeparation)) {
            separationVector = { 0.0f, belowSeparation, 0.0f };
        } else if (minSeparation == abs(frontSeparation)) {
            separationVector = { 0.0f, 0.0f, frontSeparation };

        } else {
            separationVector = { 0.0f, 0.0f, -behindSeparation };

        }
        if(Vector3Equals(separationVector ,(Vector3){0.0f, belowSeparation, 0.0f })){
            grounded = true;

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



Vector3 Player::getVelocity() {
    return velocity;
}

void Player::setSens(float temp) {
    if(temp > 0.0f){
        sens = temp;
    }else{
        //do error
    }
}

float Player::getSense() {
    return sens;
}
float Player::getFOV(){
    return fov;
}
void Player::setFOV(float temp){
    fov = temp;
}

Vector3 Player::getHeadHitBox() {
    return head_hitbox;
}

BoundingBox Player::getHeadBox() {
    return headbox;
}