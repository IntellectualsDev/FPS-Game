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
    //init local player object
    Player temp((Vector3){0,2,1},(Vector3){0,0,0},(Vector3){1.0f,2.0f,1.0f});
    //init terrain vector
    float wallWidth = 1.0f;
    float wallHeight = 5.0f;
    float wallLength = 32.0f;
    float floorLength = 50.0f;
    BoundingBox wallBox1 = (BoundingBox){(Vector3){-16.0f - wallWidth/2, 2.5f - wallHeight/2, 0.0f -wallLength/2},(Vector3){-16.0f + wallWidth/2, 2.0f + wallHeight/2, 0.0f +wallLength/2}};
    BoundingBox wallBox2 = (BoundingBox){(Vector3){16.0f - wallWidth/2, 2.5f - wallHeight/2, 0.0f -wallLength/2},(Vector3){16.0f + wallWidth/2, 2.0f + wallHeight/2, 0.0f +wallLength/2}};
    BoundingBox wallBox3 = (BoundingBox){(Vector3){0.0f - wallLength/2, 2.5f - wallHeight/2, 16.0f -wallWidth/2},(Vector3){0.0f + wallLength/2, 2.0f + wallHeight/2, 16.0f +wallWidth/2}};
    BoundingBox floorBox = (BoundingBox){(Vector3){-floorLength/2,-0.5,-floorLength/2},(Vector3){floorLength/2,0.5,floorLength/2}};
    vector<BoundingBox> terrainVector = {wallBox1,wallBox2,wallBox3,floorBox};
    BoundingBox topBox1 = (BoundingBox){(Vector3){-16.0f - 0.5, 4.5f, 0.0f -wallLength/2},(Vector3){-16.0f + 0.5, 2.5f + wallHeight/2, 0.0f +wallLength/2}};
    BoundingBox topBox2 = (BoundingBox){(Vector3){16.0f - 0.5, 4.5f, 0.0f -wallLength/2},(Vector3){16.0f + 0.5, 2.5f + wallHeight/2, 0.0f +wallLength/2}};
    BoundingBox topBox3 = (BoundingBox){(Vector3){0.0f - wallLength/2, 4.5f, 16.0f -0.5},(Vector3){0.0f + wallLength/2, 2.5f + wallHeight/2, 16.0f + 0.5}};
    vector<BoundingBox> topBoxVector = {topBox1,topBox2,topBox3};
    //init window
    //TODO shit code
    InitWindow(0, 0, "Shooter Game");
    const int screenWidth = GetMonitorWidth(0);
    const int screenHeight = GetMonitorHeight(0);
    CloseWindow();
    InitWindow(screenWidth-400, screenHeight-400, "Shooter Game");

    //set fps
    SetTargetFPS(60);
    //disable cursor
    DisableCursor();
    //init previous position
    Vector3 prevPosition;
    //vector of all bullets that have been spawned by local player
    vector<Bullet>& ref = *temp.getEntities();
    //Main Loop
    while (!WindowShouldClose()){
        //get previous position
        prevPosition = temp.getPosition();
        //read local inputs and update player positions/spawn bullet entities
        temp.UpdatePlayer(IsKeyDown(KEY_W),IsKeyDown(KEY_A),IsKeyDown(KEY_S),IsKeyDown(KEY_D),GetMouseDelta(),
                          IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_SPACE),GetFrameTime(),prevPosition,terrainVector,topBoxVector,
                          IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL));
        //update bullet entities position based on a multiple of the frame time(delta T)
        temp.updateEntities(GetFrameTime());


        //check for all Collisions
        for(int j = 0;j <terrainVector.size();j++){
            for(int i = 0; i <temp.getEntities()->size();i++){
                //check for bullet collisions with all terrain
                if(CheckCollisionBoxes(ref[i].getBulletBox(),terrainVector[j])) {
                    cout << "Hit:" << ref[i].getPosition().x << " " << ref[i].getPosition().y  << " " << ref[i].getPosition().z <<  endl;
                    ref[i].kill();
                    cout << ref[i].getAlive() << endl;
                }
            }
            //check for player collisions with terrain

            //if bottom of player is through the top of a terrain
                //set grounded to true and update player position
            //if top of player is through bottom of terrain
            //if left of player is through right of terrain
            //if right of player is through left of terrain
            //if front of player is through back of terrain
            //if back of player is through front of terrain

            }


        //begin rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);
        //begin rendering in 3d
        BeginMode3D(*temp.getCamera());
        //draw bounding boxes
        for(int i = 0; i < terrainVector.size();i++){
            DrawBoundingBox(terrainVector[i],BLACK);
        }
        for(int i = 0; i < topBoxVector.size();i++){
            DrawBoundingBox(topBoxVector[i],RED);
        }
        //draw player bounding box
        DrawBoundingBox(temp.getPlayerBox(),PINK);
        //draw terrain and bullet entities
        //TODO render based on if it is in players FOV
        DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 50.0f, 50.0f }, LIGHTGRAY); // Draw ground
        DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
        DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
        DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

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

