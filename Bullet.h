//
// Created by josep on 2/14/2024.
//

#ifndef GAME1_BULLET_H
#define GAME1_BULLET_H
namespace raylib{
#include <raylib.h>


}
#include "raymath.h"
#include <iostream>

using namespace std;
//TODO implement TTL on bullets and implent textures and animations on collision
class Bullet {
public:
    Bullet(Vector3 temp_position, Vector3 temp_velocity, Vector3 temp_hitbox,bool temp,float temp_speed,Vector3 rotation){
        position = temp_position;
        velocity = temp_velocity;
        hitbox = temp_hitbox;
        alive = temp;
        speed = temp_speed;
        prevPosition = position;
        rotation = rotation;
//        float yaw = atan2(rotation.x, rotation.z);
//        float pitch = atan2(-rotation.y, sqrt(rotation.x*rotation.x + rotation.z*rotation.z));
//
//        bulletModel = LoadModelFromMesh(GenMeshCube(0.5f, 0.5f, 4.5f)); // Creates a unit cube
//        bulletModel.transform = MatrixRotateXYZ((Vector3){pitch, yaw, 0.0f});
        Vector3 direction = Vector3Normalize(rotation);  // Assuming 'rotation' is the direction vector

        // Default Z-axis
        Vector3 defaultForward = {0.0f, 0.0f, 1.0f};

        // Compute rotation axis and angle
        Vector3 rotAxis = Vector3CrossProduct(defaultForward, direction);
        float angle = acos(Vector3DotProduct(defaultForward, direction));
        rotAxis = Vector3Normalize(rotAxis);

        // Load the cube model as a rectangular prism
        bulletModel = LoadModelFromMesh(GenMeshCube(0.1f, 0.1f, 4.5f)); // Creates a rectangular prism

        // Apply rotation
        bulletModel.transform = MatrixRotate(rotAxis, angle);
        setBulletBox((BoundingBox){Vector3Subtract(position, Vector3Scale(hitbox, 0.5f)),
                                   Vector3Add(position, Vector3Scale(hitbox, 0.5f))});
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
    Vector3 getPrevPosition();
    float getSpeed();
    void setSweptBulletBox(BoundingBox temp);
    BoundingBox getSweptBulletBox();
    Vector3 getRotation();
private:
    Matrix rotationMatrix;
    Vector3 rotation{};
    BoundingBox sweptBulletBox{};
    float speed;
    Model bulletModel;
    bool alive;
    BoundingBox bulletBox{};
    Vector3 position{};
    Vector3 velocity{};
    Vector3 prevPosition{};
    Vector3 hitbox{};
};


#endif //GAME1_BULLET_H