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
void Player::UpdatePlayer(bool w, bool a, bool s, bool d,Vector2 mouseDelta,bool shoot,bool space,float dt, Vector3 prevPosition, vector<BoundingBox> &terrainList,vector<BoundingBox> &topBoxVector,bool sprint,bool crouch,PacketBuffer& outputBuffer) {
//Continious collision detection.

//    if(CheckCollisionBoxes(playerBox,terrainList[3])){
//        setGrounded(true,dt);
//
//    }
//    else if(topCollision){
//        setGrounded(true,dt);
//        topCollision = false;
//    }else{
//        setGrounded(false,dt);
//        space = false;
//    }
//    if(space && grounded && JumpTimer > 5*dt){
//        JumpTimer = 0.0f;
//        setGrounded(false,dt);
//
//        velocity = (Vector3){((w)-(s))*dt*4 ,((space)*Jump*10*dt),((d)-(a))*dt*4 };
//    }else if (!grounded){
//        velocity = (Vector3){((w)-(s))*dt*4 ,velocity.y + Gravity*dt*10,((d)-(a))*dt*4 };
//    }else{
//        velocity = (Vector3){((w)-(s))*dt*4 ,0,((d)-(a))*dt*4 };
//    }

    //always apply gravity
    //camera_direction accounts for direction
    //speed of character *velocity vector for input is resulting velocity from movement
    //if a character is colliding with the ground they have friction in the opposing direction of velocity which scales (maybe?)
    //if a character is not colliding with anything they have gravity ontop of their input velocity
    //if a player jumps they gain an impulse to their y velocity
    cout <<"velocity pre-collision: "<< velocity.x << "," << velocity.y << "," << velocity.z << endl;


    if(velocity.y == 0.0f){
        velocity = Vector3Add(velocity, Vector3Scale(Jump,(float)space));
    }
    velocity = Vector3Add(velocity,Gravity);
    velocity = Vector3Add(velocity, Vector3Scale((Vector3){(float)-(velocity.x),0.0f,(float)-(velocity.z)}, friction));
    velocity = Vector3Add(velocity, Vector3Scale((Vector3){(float)(w-s),0.0f,(float)(d-a)},lateralSpeed));
    velocity = Vector3Add(velocity,Vector3Multiply((Vector3){(float)sprint,0.0f,(float)sprint}, Vector3Scale(velocity,lateralSpeed)));

    if(sprint){
        camera.fovy = camera.fovy+1.0f;
        if(camera.fovy > 80.0f){
            camera.fovy = 80.0f;
        }
    }else{
        camera.fovy = camera.fovy-3.0f;
        if(camera.fovy < 60.0f){
            camera.fovy = 60.0f;
        }
    }
    //TODO check for case to set grounded == true
    //TODO implement grounded/jumping movement
    UpdateCameraPro(&camera,
                    (Vector3){velocity.x,velocity.z,velocity.y},//SHIT
                    (Vector3){
                            mouseDelta.x*dt*2.0f ,                            // Rotation: yaw
                            mouseDelta.y*dt*2.0f,                            // Rotation: pitch
                            0.0f                                                // Rotation: roll
                    },
                    GetMouseWheelMove()*dt *2.0f);
    if(shoot && coolDown <= 0.0f){
        coolDown = 0.3;
        //TODO
        //add weapon slots and check current inventory index to spawn correct bullet
        Bullet temp(Vector3Add(camera.position, Vector3Scale(camera_direction(&camera),0.7f)), Vector3Scale(camera_direction(&camera),5.0f),(Vector3){0.1f,0.1f,0.1f},
                    true,40.0f);
        //TODO look into ray casting
//        temp.getBulletModel().transform =  MatrixRotateXYZ((Vector3){ DEG2RAD*temp.getVelocity().x, DEG2RAD*temp.getVelocity().y, DEG2RAD*temp.getVelocity().z});
        entities.push_back(temp);
        //TODO deque object instead of vector
    }
    position = camera.position;
    playerBox.min = (Vector3){position.x - hitbox.x/2,
                              position.y - hitbox.y/2-1.0f,
                              position.z - hitbox.z/2};
    playerBox.max = (Vector3){position.x + hitbox.x/2,
                              position.y + hitbox.y/2-0.5f,
                              position.z + hitbox.z/2};
//    for(int i = 0;i < terrainList.size();i++){
//        if(i <= 3){
//            //TODO fix this
//            if(i!=4 && CheckCollisionBoxes(playerBox,topBoxVector[i])&&!space && !CheckCollisionBoxes(playerBox,terrainList[i])){
//                position.y = 2+topBoxVector[i].max.y;//bad code
//                camera.position.y = position.y;
//                topCollision = true;
//            }
//            else if(CheckCollision(playerBox,terrainList[i],separationVector)){
//
//
//                position = Vector3Add(position,separationVector);
//                camera.position = position;
//                camera.target = Vector3Add(camera.target,separationVector);
//                if(i != 3){
//
//                }
//            }
//        }
//    }
    if(crouch){
        position = {0.0f,5.0f,1.0f};
        camera.position = position;
        camera.target = {10.0f, 2.0f, 10.0f};
        velocity = Vector3Zero();
    }
    cout << "position pre collision: " << position.x << ", " << position.y << ", " << position.z << endl;
    for(auto & i : terrainList){
        if(CheckCollision(playerBox,i,separationVector)){
            position = Vector3Add(position,separationVector);
            velocity = (Vector3){separationVector.x != 0.0f ? 0.0f : velocity.x,
                                 separationVector.y != 0.0f ? 0.0f : velocity.y,
                                 separationVector.z != 0.0f ? 0.0f : velocity.z};
            camera.position = position;
            camera.target = Vector3Add(camera.target,separationVector);
            cout << "velocity post collision : " << velocity.x << ", " << velocity.y << ", " << velocity.z << endl;
            cout << "position post collision: " << position.x << ", " << position.y << ", " << position.z << endl;
        }
    }
    sweptAABB.min = Vector3Subtract(Vector3Min(prevPosition,position), Vector3Scale(hitbox,0.5f));
    sweptAABB.max = Vector3Add(Vector3Max(prevPosition,position), Vector3Scale(hitbox,0.5f));
    for(auto & i : terrainList){
        if(CheckCollision(sweptAABB,i,separationVector)){
            position = Vector3Add(position,separationVector);
            velocity = (Vector3){separationVector.x != 0.0f ? 0.0f : velocity.x,
                                 separationVector.y != 0.0f ? 0.0f : velocity.y,
                                 separationVector.z != 0.0f ? 0.0f : velocity.z};
            camera.position = position;
            camera.target = Vector3Add(camera.target,separationVector);
//            cout << "velocity post collision : " << velocity.x << ", " << velocity.y << ", " << velocity.z << endl;
//            cout << "position post collision: " << position.x << ", " << position.y << ", " << position.z << endl;
        }
    }
    playerBox.min = (Vector3){position.x - hitbox.x/2,
                              position.y - hitbox.y/2-1.0f,
                              position.z - hitbox.z/2};
    playerBox.max = (Vector3){position.x + hitbox.x/2,
                              position.y + hitbox.y/2-0.5f,
                              position.z + hitbox.z/2};

    updateEntities(dt,terrainList);
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

Vector3 Player::camera_direction(Camera *tcamera,) {
    return Vector3Normalize(Vector3Subtract(tcamera->target, tcamera->position));
}

void Player::updateEntities(float dt,vector<BoundingBox> &terrainList) {
    for(auto & j : terrainList){
        for(auto & entity : entities){
            //check for bullet collisions with all terrain
            if(CheckCollisionBoxes(entity.getBulletBox(), j)) {
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
                    Vector3Scale(entities[i].getVelocity(), dt*entities[i].getSpeed()));
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

void Player::setGrounded(bool temp,float dt) {
    if(temp){
        grounded = temp;
        if(JumpTimer < 6*dt){
            JumpTimer += dt;
        }

    }else{

        grounded = temp;
    }

}

bool Player::getGrounded() {
    return grounded;
}

Vector3 Player::getVelocity() {
    return velocity;
}




