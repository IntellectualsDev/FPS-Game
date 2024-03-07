#include <iostream>
#include "raylib.h"
#include <raymath.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include "vector"
#endif

#include "Player.h"
#include "Bullet.h"




//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Player temp((Vector3){0,2,1},(Vector3){0,0,0},(Vector3){1.0f,2.0f,1.0f});


    const int screenWidth = GetMonitorWidth(GetCurrentMonitor());
    const int screenHeight = GetMonitorHeight(GetCurrentMonitor());

    InitWindow(screenWidth, screenHeight, "Shooter Game");
    ToggleFullscreen();

    SetTargetFPS(60);

    DisableCursor();
    float wallWidth = 1.0f;
    float wallHeight = 5.0f;
    float wallLength = 32.0f;
    BoundingBox wallBox1 = (BoundingBox){(Vector3){-16.0f - wallWidth/2, 2.5f - wallHeight/2, 0.0f -wallLength/2},(Vector3){-16.0f + wallWidth/2, 2.5f + wallHeight/2, 0.0f +wallLength/2}};
    BoundingBox wallBox2 = (BoundingBox){(Vector3){16.0f - wallWidth/2, 2.5f - wallHeight/2, 0.0f -wallLength/2},(Vector3){16.0f + wallWidth/2, 2.5f + wallHeight/2, 0.0f +wallLength/2}};
    BoundingBox wallBox3 = (BoundingBox){(Vector3){0.0f - wallLength/2, 2.5f - wallHeight/2, 16.0f -wallWidth/2},(Vector3){0.0f + wallLength/2, 2.5f + wallHeight/2, 16.0f +wallWidth/2}};

    while (!WindowShouldClose()){
        temp.UpdatePlayer(IsKeyDown(KEY_W),IsKeyDown(KEY_A),IsKeyDown(KEY_S),IsKeyDown(KEY_D),GetMouseDelta(),
                          IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyPressed(KEY_SPACE));
        temp.updateEntities(GetFrameTime());
        vector<Bullet>& ref = *temp.getEntities();
        for(int i = 0; i <temp.getEntities()->size();i++){
            if(CheckCollisionBoxes(ref[i].getBulletBox(),wallBox1)){
                cout << "Hit!" << endl;
                ref[i].kill();
                cout << ref[i].getAlive() << endl;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(*temp.getCamera());
        DrawBoundingBox(wallBox1,BLACK);
        DrawBoundingBox(wallBox2,BLACK);
        DrawBoundingBox(wallBox3,BLACK);
        DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground
        DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
        DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
        DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall
        DrawCube(Vector3Add(temp.getPosition(),Vector3{0.0f,0.0f,0.0f}),temp.getHitBox().x,temp.getHitBox().y,temp.getHitBox().z,PURPLE);
        for(int i = 0;i<ref.size();i++){
            Bullet temp_entity = ref[i];

            DrawCube(temp_entity.getPosition(),temp_entity.getHitbox().x
                    ,temp_entity.getHitbox().y,
                     temp_entity.getHitbox().z,
                     RED);
            DrawBoundingBox(temp_entity.getBulletBox(),BLACK);
        }
        EndMode3D();


        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
