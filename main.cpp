

#include <iostream>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include "vector"
#endif
#include "raylib.h"
#include <raymath.h>
#include "Player.h"
#include "Bullet.h"
#include <enet/enet.h>
#include "Transmitter.h"
#include "RemotePlayer.h"
#include "Gateway.h"
#include "flatbuffers/flatbuffer_builder.h"
#include "game_state_generated.h"


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
mutex consoleMutex;
bool initLoading = true;
bool inMenu = false;
bool isNestedMenu = false;
Rl_Rectangle singlePlayerBtnRect, multiPlayerBtnRect, settingsBtnRect, aboutBtnRect, quitBtnRect, accountBtnRect;
int screenWidth;
int screenHeight;
//TODO DYNAMICALLY FIND CLIENT IP
string clientIP;
string serverIP;

BoundingBox tempHeadBox;

void DrawLoadingScreen();
void DrawMainMenu(Vector2 mousePos);
void DrawNestedMenu(Rl_Rectangle btnRect);


int main(void)
{
    serverIP ="192.168.1.13";
    clientIP = "192.168.56.1";
    //init enet
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet\n";
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);

    //init buffers and threads for transmitting and recieving packets
    auto* outputBuffer = new PacketBuffer;
//    Transmitter transmitter(clientIP,5452,outputBuffer,consoleMutex,serverIP,5450);
//    auto* inputBuffer = new PacketBuffer;
//    Gateway gateway(clientIP,5453,inputBuffer);

    //TODO
    //implement join sequence

    //TODO : create new class with appropriate methods, encapsulates pulling from buffer/interpolating


    //init local player object
    Player temp((Vector3){0.0f,20.0f,0.0f},"scout",60.0f);

    //TODO : create tiling so level design can be an easier workflow
    //init terrain vector
    float wallWidth = 3.0f;
    float wallHeight = 4.0f;
    float wallLength = 32.0f;
    float floorLength = 500.0f;
    BoundingBox wallBox1 = (BoundingBox){(Vector3){-16.0f - wallWidth/2, 0, 0.0f -wallLength/2},(Vector3){-16.0f + wallWidth/2, wallHeight, 0.0f +wallLength/2}};
    BoundingBox wallBox2 = (BoundingBox){(Vector3){16.0f - wallWidth/2, 0, 0.0f -wallLength/2},(Vector3){16.0f + wallWidth/2, wallHeight, 0.0f +wallLength/2}};
    BoundingBox wallBox3 = (BoundingBox){(Vector3){0.0f - wallLength/2, 0, 16.0f -wallWidth/2},(Vector3){0.0f + wallLength/2, wallHeight, 16.0f +wallWidth/2}};
    BoundingBox floorBox = (BoundingBox){(Vector3){-floorLength/2,-2,-floorLength/2},(Vector3){floorLength/2,0,floorLength/2}};
    vector<BoundingBox> terrainVector = {wallBox1,wallBox2,wallBox3,floorBox};
    BoundingBox topBox1 = (BoundingBox){(Vector3){-16.0f - 0.5+0.05f, 4.99f, 0.0f +0.05f-wallLength/2},(Vector3){-16.0f + 0.5-0.05f, 2.5f + wallHeight/2, 0.0f -0.05f+wallLength/2}};
    BoundingBox topBox2 = (BoundingBox){(Vector3){16.0f - 0.5+0.05f, 4.99f, 0.0f +0.05f-wallLength/2},(Vector3){16.0f + 0.5-0.05f, 2.5f + wallHeight/2, 0.0f -0.05f+wallLength/2}};
    BoundingBox topBox3 = (BoundingBox){(Vector3){0.0f+0.05f - wallLength/2, 4.99f, 16.0f -0.5+0.05f},(Vector3){0.0f -0.05f+ wallLength/2, 2.5f + wallHeight/2, 16.0f -0.05f+ 0.5}};
    vector<BoundingBox> topBoxVector = {topBox1,topBox2,topBox3};
    //init window
    //TODO : potentially look into creating loading sequence for server
    screenWidth = 1500;
    screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Shooter Game");
    //set target fps
    SetTargetFPS(60);
    //disable cursor
    //init previous position
    Vector3 prevPosition;
    Camera3D prevCamera;
    //vector of all bullets that have been spawned by local player
    vector<Bullet>& localPlayerBullets = *temp.getEntities();
    //Main Loop
    //START THE TRANSMITTER AND GATEWAY
//    transmitter.start();
//    gateway.start();
    //init menu button hitboxes
    singlePlayerBtnRect = { 100, 200, 200, 50 };
    multiPlayerBtnRect = { 100, 260, 200, 50 };
    settingsBtnRect = { 100, 320, 200, 50 };
    aboutBtnRect = { 100, 380, 200, 50 };
    quitBtnRect = { 100, 440, 200, 50 };
    accountBtnRect = { 600, 20, 150, 50 };
    int tick = 1;
    Texture2D hands = LoadTexture("resources/hands.png");
    Vector3 handsPosition;
    SetTextureFilter(hands,TEXTURE_FILTER_BILINEAR);
    GenTextureMipmaps(&hands);
    const float TICK_RATE = 1.0f / 60.0f;  // 60 updates per second
    float accumulator = 0.0f;
    float frameTime = 0.0f;
    BoundingBox tempPlayerBox;
    Vector2 adjustedMouseDelta;
    float crosshairDimension = 8.0f;
    while (!WindowShouldClose()){


        if(initLoading){
            BeginDrawing();
            ClearBackground(RED);
            DrawLoadingScreen();
            EndDrawing();
            if(IsKeyPressed(KEY_ENTER)){
                initLoading = false;
                inMenu = true;
            }
        }else if(inMenu){
            BeginDrawing();
            ClearBackground(BLUE);
            Vector2 mousePos = GetMousePosition();
            DrawMainMenu(mousePos);
            EndDrawing();
        }else if(isNestedMenu){
            //do something
        }else{
            //TODO
            //implement checking incoming packets against past packet buffer and update client position based off that
                //create history of predicted states for local client (for checking against server states)
                //create history of states for remote players
                //create history of inputs for local client (to dump to server)
            frameTime = GetFrameTime();
            accumulator += frameTime;
            prevPosition = temp.getPosition();
            prevCamera = *temp.getCamera();
            adjustedMouseDelta = { GetMouseDelta().x/(frameTime*200.0f), GetMouseDelta().y/(frameTime*200.0f)};
            while(accumulator >= TICK_RATE){

                //create flatbuffer builder and create the source and destination address as well as the tick object and previous position
                flatbuffers::FlatBufferBuilder builder(1024);
                
                
                
                //SERVER ADDRESS !!!!!!!!!!!!!!!
                auto destAddr = builder.CreateString(serverIP);
                
                //SOURCE ADDRESS !!!!!!!!!!!!!!!!!!
                auto srcAddr = builder.CreateString(clientIP);


                const auto mouse = CreateOD_Vector2(builder,GetMouseDelta().x,GetMouseDelta().y);
                auto dest = CreateDestPoint(builder,destAddr,5450);
                auto src = CreateSourcePoint(builder,srcAddr,6565);
                const auto ticked = CreateTick(builder,tick,TICK_RATE);
                const auto prev = CreateOD_Vector3(builder,prevPosition.x,prevPosition.y,prevPosition.z);
                vector<flatbuffers::Offset<Buffer>> buffers{};
                auto buffersVector = builder.CreateVector(buffers);

                //read client inputs and add them to the input builder
                InputBuilder inputBuilder(builder);
                inputBuilder.add_tick(ticked);
                inputBuilder.add_a(IsKeyDown(KEY_A));
                inputBuilder.add_w(IsKeyDown(KEY_W));
                inputBuilder.add_s(IsKeyDown(KEY_S));
                inputBuilder.add_d(IsKeyDown(KEY_D));
                inputBuilder.add_client_uid(0);
                inputBuilder.add_crouch(IsKeyDown(KEY_LEFT_CONTROL));
                inputBuilder.add_mouse_delta(mouse);
                inputBuilder.add_opponent_buffer_ticks(buffersVector);
                inputBuilder.add_previous_position(prev);
                inputBuilder.add_shoot(IsMouseButtonDown(MOUSE_BUTTON_LEFT));
                inputBuilder.add_space(IsKeyDown(KEY_SPACE));
                inputBuilder.add_sprint(IsKeyDown(KEY_LEFT_SHIFT));
                auto input = inputBuilder.Finish();


                vector<flatbuffers::Offset<Input>> historyBufOffsetVector;
                //TODO : append history of inputs
                historyBufOffsetVector.push_back(input);
                auto bufferVector = builder.CreateVector(historyBufOffsetVector);
                auto clients = CreateClientInputs(builder,bufferVector);
                PayloadBuilder payloadBuilder(builder);
                payloadBuilder.add_payload_CI(clients);
                payloadBuilder.add_payload_I(NULL);
                payloadBuilder.add_payload_PD(NULL);
                payloadBuilder.add_payload_PS(NULL);
                auto payload = payloadBuilder.Finish();
                OD_PacketBuilder packetBuilder(builder);
                packetBuilder.add_reliable(false);
                packetBuilder.add_dest_point(dest);
                packetBuilder.add_source_point(src);
                packetBuilder.add_packet_type(PacketType_Input);
                packetBuilder.add_client_id(0);
                packetBuilder.add_lobby_number(0);
                packetBuilder.add_client_tick(ticked);
                packetBuilder.add_payload(payload);
                auto packet = packetBuilder.Finish();
                builder.Finish(packet);
                uint8_t* buffer = builder.GetBufferPointer();
                size_t bufferSize = builder.GetSize();
                enet_uint32 flags = 0;
                if(GetOD_Packet(buffer)->reliable()){
                    flags = ENET_PACKET_FLAG_RELIABLE;
                }
                // Create an ENetPacket from the serialized data
                std::unique_ptr<ENetPacket>finalPacket(enet_packet_create(buffer, bufferSize, flags));
                //add packet to output buffer
//                outputBuffer->addPacket(std::move(finalPacket));
                //TODO!!!!!!!!!!!!!!
                //keep track of predicted state(deltas) and upon receiving and parsing the state from the server check against the corresponding predicted state
                //if they match continue but if they do not match return to last approved state and add predicted deltas to said state to get new current state.
                temp.UpdatePlayer(IsKeyDown(KEY_W),IsKeyDown(KEY_A),IsKeyDown(KEY_S),IsKeyDown(KEY_D),adjustedMouseDelta,
                                  IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_SPACE),GetFrameTime(),temp.getPlayerBox(),terrainVector,topBoxVector,
                                  IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL),*outputBuffer,tick);

                if (++tick >= 60) tick = 0;
                accumulator -= TICK_RATE;
            }
//
//            //TODO
//            //keep track of predicted state(deltas) and upon receiving and parsing the state from the server check against the corresponding predicted state
//            //if they match continue but if they do not match return to last approved state and add predicted deltas to said state to get new current state.
//            temp.UpdatePlayer(IsKeyDown(KEY_W),IsKeyDown(KEY_A),IsKeyDown(KEY_S),IsKeyDown(KEY_D),GetMouseDelta(),
//                              IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_SPACE),GetFrameTime(),prevPosition,terrainVector,topBoxVector,
//                              IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL),outputBuffer);

            //TODO : model!!!!!
            float alpha = accumulator / TICK_RATE;
            Vector3 interpolatedPosition = Vector3Lerp(prevPosition, temp.getPosition(), alpha);
            Vector3 interpolatedCamTarget = Vector3Lerp(prevCamera.target,temp.getCamera()->target,alpha);


            //begin rendering
            BeginDrawing();
            ClearBackground(RAYWHITE);
            //begin rendering in 3d
            Camera3D interpolatedCam = *temp.getCamera();
            interpolatedCam.position = interpolatedPosition;
            interpolatedCam.target = interpolatedCamTarget;
            BeginMode3D(interpolatedCam);





            for(int i = 0; i < terrainVector.size();i++){
                DrawBoundingBox(terrainVector[i],BLACK);
            }
            for(int i = 0; i < topBoxVector.size();i++){
                DrawBoundingBox(topBoxVector[i],RED);
            }
            //draw player bounding box

            tempPlayerBox.min = (Vector3){interpolatedPosition.x - temp.getBodyHitBox().x / 2,
                                          interpolatedPosition.y - temp.getBodyHitBox().y -temp.getHeadHitBox().y/2,
                                      interpolatedPosition.z - temp.getBodyHitBox().z / 2};
            tempPlayerBox.max = (Vector3){interpolatedPosition.x + temp.getBodyHitBox().x / 2,
                                          interpolatedPosition.y -temp.getHeadHitBox().y/2,
                                      interpolatedPosition.z + temp.getBodyHitBox().z / 2};
            tempHeadBox.min = (Vector3){interpolatedPosition.x - temp.getHeadHitBox().x / 2,
                                          interpolatedPosition.y - temp.getHeadHitBox().y / 2,
                                          interpolatedPosition.z - temp.getHeadHitBox().z / 2};
            tempHeadBox.max = (Vector3){interpolatedPosition.x + temp.getHeadHitBox().x / 2,
                                          interpolatedPosition.y + temp.getHeadHitBox().y/2,
                                          interpolatedPosition.z + temp.getHeadHitBox().z / 2};
//            tempPlayerBox.min = Vector3Subtract(interpolatedPosition,temp.getPlayerBox().min);
//            tempPlayerBox.max = Vector3Add(interpolatedPosition,temp.getPlayerBox().max);
//            tempHeadBox.min = Vector3Subtract(interpolatedPosition,temp.getHeadBox().min);
//            tempHeadBox.max = Vector3Add(interpolatedPosition,temp.getHeadBox().max);
//            DrawBoundingBox(temp.getPlayerBox(),PINK);
            DrawBoundingBox(tempHeadBox,BLACK);
            DrawBoundingBox(tempPlayerBox,PINK);
            //draw terrain and bullet entities
            //TODO render based on if it is in players FOV
            DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ floorLength, floorLength }, LIGHTGRAY); // Draw ground
            DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, wallHeight, 32.0f, BLUE);     // Draw a blue wall
            DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, wallHeight, 32.0f, LIME);      // Draw a green wall
            DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, wallHeight, 1.0f, GOLD);      // Draw a yellow wall

            for(auto & localPlayerBullet : localPlayerBullets){


                DrawCube(localPlayerBullet.getPosition(),localPlayerBullet.getHitbox().x
                        ,localPlayerBullet.getHitbox().y,
                         localPlayerBullet.getHitbox().z,
                         RED);
                DrawBoundingBox(localPlayerBullet.getBulletBox(),BLACK);
            }
//            //            DrawBillboard(*temp.getCamera(),hands, Vector3Scale(Vector3Add(temp.getPosition(),temp.getCamera()->target),2.0f),10.0f,WHITE);
//            handsPosition = Vector3Subtract(Vector3Add(temp.getCamera()->position, Vector3Scale(temp.getCamera()->target,2.0f*0.5f)),
//                                            Vector3Scale(Vector3CrossProduct((Vector3){0.0f,0.1f,0.0f},temp.getCamera()->target),2.0f*0.5f));

//            DrawBillboard(*temp.getCamera(),hands,handsPosition ,2.0f,WHITE);
//            BeginBlendMode(BLEND_ALPHA);
//            DrawBillboard(*temp.getCamera(),hands,temp.getCamera()->target,2.0f,WHITE);
////            DrawTextureInFrontOfCamera(*temp.getCamera(),hands, temp.getCamera()->target, 10.0f); // Adjust '0.5f' and '1.0f' as needed
//            EndBlendMode();


            EndMode3D();
            //TODO : ammo count current weapon equipped hp
            //TODO : swapping gun animations knife animations shooting animations
            BeginBlendMode(BLEND_ALPHA);
            DrawRectangle(screenWidth/2.0f,(screenHeight/2.0f)-crosshairDimension/2,crosshairDimension/4,crosshairDimension+crosshairDimension/4,{150,220,255,255});
            DrawRectangle((screenWidth/2.0f)-crosshairDimension/2,screenHeight/2.0f,crosshairDimension+crosshairDimension/4,crosshairDimension/4,{150,220,255,255});
            DrawTexturePro(hands,{0.0f,0.0f,(float)hands.width,(float)hands.height},
                           {0,(float)screenHeight/2.0f,(float)screenWidth,(float)screenHeight/2.0f},
                           {0,0},0.0f,WHITE);
            EndBlendMode();

            EndDrawing();
        }
    }
    UnloadTexture(hands);
    Rl_CloseWindow();        // Close window and OpenGL context

    return 0;
}

void DrawLoadingScreen() {
    RL_DrawText("Loading...", screenWidth / 2 - MeasureText("Loading...", 20) / 2, screenHeight / 2  + 20, 20, GRAY);
}
void DrawMainMenu(Vector2 mousePos) {


    DrawRectangle(singlePlayerBtnRect.x, singlePlayerBtnRect.y,singlePlayerBtnRect.width,singlePlayerBtnRect.height, WHITE);
    RL_DrawText("Single Player", singlePlayerBtnRect.x + 10, singlePlayerBtnRect.y + 15, 20, BLACK);

    DrawRectangle(multiPlayerBtnRect.x, multiPlayerBtnRect.y,multiPlayerBtnRect.width,multiPlayerBtnRect.height, WHITE);
    RL_DrawText("Multi Player", multiPlayerBtnRect.x + 10, multiPlayerBtnRect.y + 15, 20, BLACK);

    DrawRectangle(settingsBtnRect.x, settingsBtnRect.y,settingsBtnRect.width,settingsBtnRect.height, WHITE);
    RL_DrawText("Settings", settingsBtnRect.x + 10, settingsBtnRect.y + 15, 20, BLACK);

    DrawRectangle(aboutBtnRect.x, aboutBtnRect.y,aboutBtnRect.width,aboutBtnRect.height, WHITE);
    RL_DrawText("About", aboutBtnRect.x + 10, aboutBtnRect.y + 15, 20, BLACK);

    DrawRectangle(quitBtnRect.x, quitBtnRect.y,quitBtnRect.width,quitBtnRect.height, WHITE);
    RL_DrawText("Quit", quitBtnRect.x + 10, quitBtnRect.y + 15, 20, BLACK);

    DrawRectangle(accountBtnRect.x, accountBtnRect.y,accountBtnRect.width,accountBtnRect.height, WHITE);
    RL_DrawText("Account", accountBtnRect.x + 10, accountBtnRect.y + 15, 20, BLACK);

    // Check for button clicks
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {


        if (CheckCollisionPointRec(mousePos, singlePlayerBtnRect)) {
            // Handle single player button click
//            isNestedMenu = true;
//            DrawNestedMenu(singlePlayerBtnRect);
            initLoading = false;
            inMenu = false;
            isNestedMenu = false;
            DisableCursor();
        } else if (CheckCollisionPointRec(mousePos, multiPlayerBtnRect)) {
            // Handle multi player button click
            isNestedMenu = true;
            DrawNestedMenu(multiPlayerBtnRect);
        } else if (CheckCollisionPointRec(mousePos, settingsBtnRect)) {
            // Handle settings button click
            isNestedMenu = true;
            DrawNestedMenu(settingsBtnRect);
        } else if (CheckCollisionPointRec(mousePos, aboutBtnRect)) {
            // Handle about button click
            isNestedMenu = true;
            DrawNestedMenu(aboutBtnRect);
        } else if (CheckCollisionPointRec(mousePos, quitBtnRect)) {
            // Handle quit button click
            Rl_CloseWindow();
        } else if (CheckCollisionPointRec(mousePos, accountBtnRect)) {
            // Handle account button click
            isNestedMenu = true;
            DrawNestedMenu(accountBtnRect);
        }
    }
}
void DrawNestedMenu(Rl_Rectangle btnRect) {
    //TODO: display relevant Nested Menu
// Check for button clicks or user input to go back to the main menu
    if (IsKeyPressed(KEY_ESCAPE)) {
    isNestedMenu = false;
    }
}
