//
// Created by josep on 2/14/2024.
//
#include "raylib.h"
#include "Bullet.h"

void Bullet::UpdatePosition(float x, float y, float z) {
    prevPosition = position;
    position.x = x;
    position.y = y;
    position.z = z;

}

Vector3 Bullet::getPosition() {
    return position;
}

Vector3 Bullet::getHitbox() {
    return hitbox;
}

Vector3 Bullet::getVelocity() {
    return velocity;
}

bool Bullet::getAlive() {
    return alive;
}

BoundingBox Bullet::getBulletBox() {
    return bulletBox;
}

void Bullet::setBulletBox(BoundingBox temp) {
    bulletBox = temp;
}

void Bullet::kill() {
    this->alive = false;
}

void Bullet::setBulletModel() {
    bulletModel = LoadModel("../resources/uploads_files_2888820_9mm+Luger.obj");
    Texture2D bulletTexAlbedo = LoadTexture("../resources/9mm Luger Albedo.png");
    bulletModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = bulletTexAlbedo;
    Texture2D bulletTexRough = LoadTexture("../resources/9mm Luger Roughness.png");
    bulletModel.materials[0].maps[MATERIAL_MAP_ROUGHNESS].texture = bulletTexRough;
    Texture2D bulletTexNormal = LoadTexture("../resources/9mm Luger Normal.png");
    bulletModel.materials[0].maps[MATERIAL_MAP_NORMAL].texture = bulletTexNormal;

    Texture2D bulletTexSpec = LoadTexture("../resources/9mm Luger Specular.png");
    bulletModel.materials[0].maps[MATERIAL_MAP_SPECULAR].texture = bulletTexSpec;


}

Model Bullet::getBulletModel() {
    return bulletModel;
}

float Bullet::getSpeed() {
    return speed;
}

Vector3 Bullet::getPrevPosition() {
    return prevPosition;
}

void Bullet::setSweptBulletBox(BoundingBox temp) {
    sweptBulletBox = temp;
}

BoundingBox Bullet::getSweptBulletBox() {
    return sweptBulletBox;
}
