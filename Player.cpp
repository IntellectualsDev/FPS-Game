//
// Created by josep on 2/8/2024.
//

#include "Player.h"
#include "Bullet.h"
#include <raymath.h>

void Player::UpdatePlayer(bool w, bool a, bool s, bool d,Vector2 mouseDelta,bool shoot,bool space) {
    if(!grounded && position.y == 2.0f){
        grounded= true;
    }
    if(space && grounded){
        grounded = false;
    } else if(space && !grounded){
        //do nothing
    }

    cout << space <<  " " << camera.position.y << " "<< position.y << endl;
    //TODO check for case to set grounded == true
    //TODO implement grounded/jumping movement
    if(grounded){
        UpdateCameraPro(&camera,
                        (Vector3){
                                (w)*0.1f -      // Move forward-backward
                                (s)*0.1f,
                                (d)*0.1f -   // Move right-left
                                (a)*0.1f,
                                (space)*0.0f                                                // Move up-down
                        },
                        (Vector3){
                                mouseDelta.x*0.1f,                            // Rotation: yaw
                                mouseDelta.y*0.1f,                            // Rotation: pitch
                                0.0f                                                // Rotation: roll
                        },
                        GetMouseWheelMove()*2.0f);
        position = camera.position;
    } else {
        UpdateCameraPro(&camera,
                        (Vector3){
                                (w)*0.1f -      // Move forward-backward
                                (s)*0.1f,
                                (d)*0.1f -   // Move right-left
                                (a)*0.1f,
                                (space)*0.1f                                                // Move up-down
                        },
                        (Vector3){
                                mouseDelta.x*0.1f,                            // Rotation: yaw
                                mouseDelta.y*0.1f,                            // Rotation: pitch
                                0.0f                                                // Rotation: roll
                        },
                        GetMouseWheelMove()*2.0f);
        position = camera.position;
    }




    if(shoot && coolDown <= 0.0f){
        coolDown = 0.3;
        cout << "SHOOT!" << endl;
        Bullet temp(Vector3Add(camera.position, Vector3Scale(camera_direction(&camera),0.1f)), Vector3Scale(camera_direction(&camera),5.0f),(Vector3){0.1f,0.1f,0.1f},
                    true);
//        temp.getBulletModel().transform =  MatrixRotateXYZ((Vector3){ DEG2RAD*temp.getVelocity().x, DEG2RAD*temp.getVelocity().y, DEG2RAD*temp.getVelocity().z});
        entities.push_back(temp);
        //TODO deque object instead of vector
    }
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
                    Vector3Scale(entities[i].getVelocity(), dt*7));
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

Vector3 Player::getPosition() {
    return position;
}

