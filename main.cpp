
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
Rectangle singlePlayerBtnRect, multiPlayerBtnRect, settingsBtnRect, aboutBtnRect, quitBtnRect, accountBtnRect;
int screenWidth;
int screenHeight;
//TODO DYNAMICALLY FIND CLIENT IP
string clientIP;
string serverIP;

BoundingBox tempHeadBox;

int ammoFontSize = 30;

void DrawLoadingScreen();
void DrawMainMenu(Vector2 mousePos);
void DrawNestedMenu(Rectangle btnRect);
const int healthFontSize = 20;

int main(void)
{
    serverIP ="192.168.1.13";
    clientIP ="192.168.56.1";
    //init enet
//    if (enet_initialize() != 0) {
//        std::cerr << "Failed to initialize ENet\n";
//        return EXIT_FAILURE;
//    }
//    atexit (enet_deinitialize);

    //init buffers and threads for transmitting and recieving packets
//    auto* outputBuffer = new PacketBuffer;
//    Transmitter transmitter(clientIP,5452,outputBuffer,consoleMutex,serverIP,5450);
//    auto* inputBuffer = new PacketBuffer;
//    Gateway gateway(clientIP,5453,inputBuffer);

    //TODO
    //implement join sequence

    //TODO : create new class with appropriate methods, encapsulates pulling from buffer/interpolating


    //init local player object
    Player temp((Vector3){0.0f,10.0f,-

            30.0f},"scout",60.0f);

    //TODO : create tiling so level design can be an easier workflow
    //init terrain vector
    float wallWidth = 3.0f;
    float wallHeight = 20.0f;
    float wallLength = 32.0f;
    float floorLength = 500.0f;
    BoundingBox wallBox1 = (BoundingBox){(Vector3){-16.0f - wallWidth/2, 0, 0.0f -wallLength*2},(Vector3){-16.0f + wallWidth/2, wallHeight, 0.0f +wallLength/2}};
    BoundingBox wallBox2 = (BoundingBox){(Vector3){16.0f - wallWidth/2, 0, 0.0f -wallLength*2},(Vector3){16.0f + wallWidth/2, wallHeight, 0.0f +wallLength/2}};
    BoundingBox wallBox3 = (BoundingBox){(Vector3){0.0f - wallLength/2, 0, 16.0f -wallWidth/2},(Vector3){0.0f + wallLength/2, wallHeight, 16.0f +wallWidth/2}};
    BoundingBox wallBox4 = (BoundingBox){(Vector3){-wallLength/2, 0.0f, -16.0f -wallWidth/2},(Vector3){wallLength/2, 2.0f, -16.0f +wallWidth/2}};
    BoundingBox floorBox = (BoundingBox){(Vector3){-wallLength/2,wallHeight-2.0f,-wallLength*2},(Vector3){wallLength/2,wallHeight,wallLength*2}};
    BoundingBox ceilBox = (BoundingBox){(Vector3){-floorLength/2,-2,-floorLength/2},(Vector3){floorLength/2,0,floorLength/2}};
    BoundingBox ceilBox2 = (BoundingBox){(Vector3){-floorLength/2,100.0f,-floorLength/2},(Vector3){floorLength/2,102.f,-wallLength*2}};
    BoundingBox bigwall1 = (BoundingBox){(Vector3){-floorLength/2-wallWidth/2,0,-floorLength/2},(Vector3){-floorLength/2+wallWidth/2,102.f,floorLength/2}};
    BoundingBox bigwall2 = (BoundingBox){(Vector3){floorLength/2-wallWidth/2,0,-floorLength/2},(Vector3){floorLength/2+wallWidth/2,102.f,floorLength/2}};
    BoundingBox bigwall3 = (BoundingBox){(Vector3){-floorLength/2, 0.0f, -wallLength*2 -wallWidth/2},(Vector3){0-wallLength/2, 102.f, -wallLength*2 +wallWidth/2}};
    BoundingBox bigwall4 = (BoundingBox){(Vector3){wallLength/2, 0.0f, -wallLength*2 -wallWidth/2},(Vector3){floorLength/2, 102.f, -wallLength*2 +wallWidth/2}};
    BoundingBox bigwall5 = (BoundingBox){(Vector3){-wallLength/2, wallHeight, -wallLength*2 -wallWidth/2},(Vector3){wallLength/2, 102.f, -wallLength*2 +wallWidth/2}};
    BoundingBox bigwall6 = (BoundingBox){(Vector3){-floorLength/2, 0, -floorLength/2-wallWidth/2},(Vector3){floorLength/2, 102.0f, -floorLength/2+wallWidth/2}};
    vector<BoundingBox> terrainVector = {wallBox1,wallBox2,wallBox3,floorBox,wallBox4,ceilBox,ceilBox2,bigwall1,bigwall2,bigwall3,bigwall4,bigwall5,bigwall6};
    float maxX = 240;
    float minX = -240;
    float maxZ = 240;
    float minZ = 60;
    int rangeZ = maxZ - minZ + 1;
    int rangeX = maxX - minX + 1;
    float minY = 3;
    int rangeY = 30;
    Vector3 randomBox={};

    cout << terrainVector.size() << endl;
    for(int i = 0; i < 40; i++){
        float sizeY = rand() % 6 + 6 ;
        float sizeX = rand() % 30 + 10;
        float sizeZ = rand() % 30 + 10;
        randomBox = {rand() % rangeX + minX,rand() % rangeY + minY,-(rand() % rangeZ + minZ)};
        BoundingBox whatever = (BoundingBox){(Vector3){randomBox.x - sizeX / 2.0f,
                                                       randomBox.y- sizeY / 2.0f,
                                                       randomBox.z- sizeZ / 2.0f},
                                             (Vector3){randomBox.x + sizeX / 2.0f,
                                                       randomBox.y+ sizeY / 2.0f,
                                                       randomBox.z+ sizeZ / 2.0f},};
        cout << "x: " << randomBox.x << " y:" << randomBox.y << " z: " << randomBox.z << endl;
        for(int j = 0; j <terrainVector.size(); j++){
            while(CheckCollisionBoxes(terrainVector[j],whatever)){
                sizeY = rand() % 6+6;
                sizeX = rand() % 30+10;
                sizeZ = rand() % 30+10;
                randomBox = {rand() % rangeX + minX,rand() % rangeY + minY,-(rand() % rangeZ + minZ)};
                whatever = (BoundingBox){(Vector3){randomBox.x - sizeX / 2.0f,
                                                   randomBox.y- sizeY / 2.0f,
                                                   randomBox.z- sizeZ / 2.0f},
                                         (Vector3){randomBox.x + sizeX / 2.0f,
                                                   randomBox.y+ sizeY / 2.0f,
                                                   randomBox.z+ sizeZ / 2.0f},};
            }
        }
        terrainVector.push_back(whatever);
    }



    //TODO: Remove
    vector<BoundingBox> topBoxVector = {};
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
    Model dummy = LoadModel("resources/swatDir/scene.gltf");
    Model freddy = LoadModel("resources/freddyDir/freddy.gltf");
    Model phil = LoadModel("resources/philDir/scene.gltf");
    Texture2D AR_Texture = LoadTexture("resources/myHands.png");
    Texture2D hands = LoadTexture("resources/hands.png");
    Texture2D pistol = LoadTexture("resources/secondary.png");
    Vector3 handsPosition;
    SetTextureFilter(AR_Texture, TEXTURE_FILTER_BILINEAR);
    GenTextureMipmaps(&AR_Texture);
    const float TICK_RATE = 1.0f / 60.0f;  // 60 updates per second
    float accumulator = 0.0f;
    float frameTime = 0.0f;
    BoundingBox tempPlayerBox;
    Vector2 adjustedMouseDelta;
    Texture2D grass = LoadTexture("resources/grass.png");
    Texture2D skyBlue = LoadTexture("resources/Sky_Blue.png");
    Texture2D sky = LoadTexture("resources/sky.png");
    Texture2D brick = LoadTexture("resources/brick.png");
    float crosshairDimension = 8.0f;
    temp.initTargets(3);
    vector<Model> models = {};
    for (int i = 0; i < terrainVector.size(); i++){
        Vector3 size = {
                fabs(terrainVector[i].max.x - terrainVector[i].min.x),
                fabs(terrainVector[i].max.y - terrainVector[i].min.y),
                fabs(terrainVector[i].max.z - terrainVector[i].min.z)
        };

//        Vector3 position = {
//                (box.min.x + box.max.x) / 2.0f,
//                (box.min.y + box.max.y) / 2.0f,
//                (box.min.z + box.max.z) / 2.0f
//        };
        Mesh cube = GenMeshCube(size.x, size.y, size.z);
        Model cubeModel = LoadModelFromMesh(cube);
        if(i==5){
            cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;
        }else if (i==6){
            cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyBlue;
        }else if(i > 6 && i < 13){
            cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sky;
        }else{
            cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = brick;
        }

        models.push_back(cubeModel);
    }
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
//                flatbuffers::FlatBufferBuilder builder(1024);
//
//
//
//                //SERVER ADDRESS !!!!!!!!!!!!!!!
//                auto destAddr = builder.CreateString(serverIP);
//
//                //SOURCE ADDRESS !!!!!!!!!!!!!!!!!!
//                auto srcAddr = builder.CreateString(clientIP);
//
//
//                const auto mouse = CreateOD_Vector2(builder,GetMouseDelta().x,GetMouseDelta().y);
//                auto dest = CreateDestPoint(builder,destAddr,5450);
//                auto src = CreateSourcePoint(builder,srcAddr,6565);
//                const auto ticked = CreateTick(builder,tick,TICK_RATE);
//                const auto prev = CreateOD_Vector3(builder,prevPosition.x,prevPosition.y,prevPosition.z);
//                vector<flatbuffers::Offset<Buffer>> buffers{};
//                auto buffersVector = builder.CreateVector(buffers);
//
//                //read client inputs and add them to the input builder
//                InputBuilder inputBuilder(builder);
//                inputBuilder.add_tick(ticked);
//                inputBuilder.add_a(IsKeyDown(KEY_A));
//                inputBuilder.add_w(IsKeyDown(KEY_W));
//                inputBuilder.add_s(IsKeyDown(KEY_S));
//                inputBuilder.add_d(IsKeyDown(KEY_D));
//                inputBuilder.add_client_uid(0);
//                inputBuilder.add_crouch(IsKeyDown(KEY_LEFT_CONTROL));
//                inputBuilder.add_mouse_delta(mouse);
//                inputBuilder.add_opponent_buffer_ticks(buffersVector);
//                inputBuilder.add_previous_position(prev);
//                inputBuilder.add_shoot(IsMouseButtonDown(MOUSE_BUTTON_LEFT));
//                inputBuilder.add_space(IsKeyDown(KEY_SPACE));
//                inputBuilder.add_sprint(IsKeyDown(KEY_LEFT_SHIFT));
//                auto input = inputBuilder.Finish();
//
//
//                vector<flatbuffers::Offset<Input>> historyBufOffsetVector;
//                //TODO : append history of inputs
//                historyBufOffsetVector.push_back(input);
//                auto bufferVector = builder.CreateVector(historyBufOffsetVector);
//                auto clients = CreateClientInputs(builder,bufferVector);
//                PayloadBuilder payloadBuilder(builder);
//                payloadBuilder.add_payload_CI(clients);
//                payloadBuilder.add_payload_I(NULL);
//                payloadBuilder.add_payload_PD(NULL);
//                payloadBuilder.add_payload_PS(NULL);
//                auto payload = payloadBuilder.Finish();
//                OD_PacketBuilder packetBuilder(builder);
//                packetBuilder.add_reliable(false);
//                packetBuilder.add_dest_point(dest);
//                packetBuilder.add_source_point(src);
//                packetBuilder.add_packet_type(PacketType_InputHistory);
//                packetBuilder.add_client_id(0);
//                packetBuilder.add_lobby_number(0);
//                packetBuilder.add_client_tick(ticked);
//                packetBuilder.add_payload(payload);
//                auto packet = packetBuilder.Finish();
//                builder.Finish(packet);
//                uint8_t* buffer = builder.GetBufferPointer();
//                size_t bufferSize = builder.GetSize();
//                enet_uint32 flags = 0;
//                if(GetOD_Packet(buffer)->reliable()){
//                    flags = ENET_PACKET_FLAG_RELIABLE;
//                }
//                // Create an ENetPacket from the serialized data
//                std::unique_ptr<ENetPacket>finalPacket(enet_packet_create(buffer, bufferSize, flags));
//                //add packet to output buffer
//                outputBuffer->addPacket(std::move(finalPacket));
                //TODO!!!!!!!!!!!!!!
                //keep track of predicted state(deltas) and upon receiving and parsing the state from the server check against the corresponding predicted state
                //if they match continue but if they do not match return to last approved state and add predicted deltas to said state to get new current state.
                temp.UpdatePlayer(IsKeyDown(KEY_W),IsKeyDown(KEY_A),IsKeyDown(KEY_S),IsKeyDown(KEY_D),adjustedMouseDelta,
                                  IsMouseButtonPressed(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_SPACE),GetFrameTime(),temp.getPlayerBox(),terrainVector,topBoxVector,
                                  IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL),tick,GetMouseWheelMove(),
                                  IsKeyPressed(KEY_R), IsMouseButtonDown(MOUSE_BUTTON_LEFT));

                if (++tick >= 60) tick = 0;
                accumulator -= TICK_RATE;
//                cout <<  atan(temp.camera_direction(temp.getCamera()).x/temp.camera_direction(temp.getCamera()).z) << endl;
                dummy.transform = MatrixRotateXYZ((Vector3) {0.0f, atan2(temp.camera_direction(temp.getCamera()).x,
                                                                         temp.camera_direction(temp.getCamera()).z)+PI,
                                                             0.0f});
//                phil.transform = MatrixRotateXYZ((Vector3) {0.0f, atan2(temp.camera_direction(temp.getCamera()).x,
//                                                                          temp.camera_direction(temp.getCamera()).z),
//                                                              0.0f});

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
//            DrawModel(freddy,(Vector3){0.0f,0.0f,0.0f},0.7f,WHITE);
//            DrawModel(phil,(Vector3){2.0f,2.45f,2.0f},0.025f,WHITE);

            for (int i = 0; i < terrainVector.size();i++){

                Vector3 position = {
                        (terrainVector[i].min.x + terrainVector[i].max.x) / 2.0f,
                        (terrainVector[i].min.y + terrainVector[i].max.y) / 2.0f,
                        (terrainVector[i].min.z + terrainVector[i].max.z) / 2.0f
                };
                if(i > 12){

                }
                DrawModel(models[i],position,1.0f,WHITE);
            }
            for(int i = 0; i < terrainVector.size();i++){
                DrawBoundingBox(terrainVector[i],BLACK);
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
//            DrawBoundingBox(tempHeadBox,BLACK);
//            DrawBoundingBox(tempPlayerBox,PINK);
            //draw terrain and bullet entities
            //TODO render based on if it is in players FOV
//            DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ floorLength, floorLength }, LIGHTGRAY); // Draw ground
//            DrawCube((Vector3){ -16.0f, wallHeight/2.0f, 0.0f }, wallWidth, wallHeight, wallLength, BLUE);     // Draw a blue wall
//            DrawCube((Vector3){ 16.0f, wallHeight/2.0f, 0.0f }, wallWidth, wallHeight, wallLength, LIME);      // Draw a green wall
//            DrawCube((Vector3){ 0.0f, wallHeight/2.0f, 16.0f }, wallLength, wallHeight, wallWidth, GOLD);      // Draw a yellow wall

            for(auto & localPlayerBullet : localPlayerBullets){


//                DrawCube(localPlayerBullet.getPosition(),localPlayerBullet.getHitbox().x
//                        ,localPlayerBullet.getHitbox().y,
//                         localPlayerBullet.getHitbox().z,
//                         RED);
                DrawModel(localPlayerBullet.getBulletModel(),localPlayerBullet.getPosition(),1.0f,YELLOW);

            }
            for(auto & target : temp.getTargets()){
                DrawModel(dummy,Vector3Add(target.position,(Vector3){0.0f,2.0f,0.0f}),0.020f,WHITE);
                DrawBoundingBox(target.headBox,RED);
                DrawBoundingBox(target.bodyBox,BLACK);
            }
            for(auto & bulletHole : temp.getBulletHoles()){

            }
//            //            DrawBillboard(*temp.getCamera(),AR_Texture, Vector3Scale(Vector3Add(temp.getPosition(),temp.getCamera()->target),2.0f),10.0f,WHITE);
//            handsPosition = Vector3Subtract(Vector3Add(temp.getCamera()->position, Vector3Scale(temp.getCamera()->target,2.0f*0.5f)),
//                                            Vector3Scale(Vector3CrossProduct((Vector3){0.0f,0.1f,0.0f},temp.getCamera()->target),2.0f*0.5f));

//            DrawBillboard(*temp.getCamera(),AR_Texture,handsPosition ,2.0f,WHITE);
//            BeginBlendMode(BLEND_ALPHA);
//            DrawBillboard(*temp.getCamera(),AR_Texture,temp.getCamera()->target,2.0f,WHITE);
////            DrawTextureInFrontOfCamera(*temp.getCamera(),AR_Texture, temp.getCamera()->target, 10.0f); // Adjust '0.5f' and '1.0f' as needed
//            EndBlendMode();


            EndMode3D();
            //TODO : ammo count current weapon equipped hp
            //TODO : swapping gun animations knife animations shooting animations
            BeginBlendMode(BLEND_ALPHA);
            DrawRectangle(screenWidth/2.0f,(screenHeight/2.0f)-crosshairDimension/2,crosshairDimension/4,crosshairDimension+crosshairDimension/4,{150,220,255,255});
            DrawRectangle((screenWidth/2.0f)-crosshairDimension/2,screenHeight/2.0f,crosshairDimension+crosshairDimension/4,crosshairDimension/4,{150,220,255,255});
            if(temp.getInventory()[temp.getCurrGun()].type == "primary"){
                DrawTexturePro(AR_Texture, {0.0f, 0.0f, (float)AR_Texture.width, (float)AR_Texture.height},
                               {0,(float)screenHeight/2.0f,(float)screenWidth,(float)screenHeight/2.0f},
                               {0,0}, 0.0f, WHITE);
            }else if(temp.getInventory()[temp.getCurrGun()].type == "melee"){
                DrawTexturePro(hands, {0.0f, 0.0f, (float)hands.width, (float)hands.height},
                               {0,(float)screenHeight/2.0f,(float)screenWidth,(float)screenHeight/2.0f},
                               {0,0}, 0.0f, WHITE);
            }else if(temp.getInventory()[temp.getCurrGun()].type == "secondary"){
                DrawTexturePro(pistol, {0.0f, 0.0f, (float)pistol.width, (float)pistol.height},
                               {0,(float)screenHeight/2.0f,(float)screenWidth,(float)screenHeight/2.0f},
                               {0,0}, 0.0f, WHITE);
            }

            RL_DrawText(TextFormat("Health: %i/",temp.getCurrentHealth()),screenWidth-MeasureText(TextFormat("%i",temp.getMaxHealth()),healthFontSize)-
                                                                          MeasureText(TextFormat("Health: %i/",temp.getCurrentHealth()),healthFontSize)-healthFontSize/2,
                        GetFontDefault().baseSize,healthFontSize,{200,220,200,255});
            RL_DrawText(TextFormat("%i",temp.getMaxHealth()),screenWidth-MeasureText(TextFormat("%i",temp.getMaxHealth()),healthFontSize)-healthFontSize/2,
                        GetFontDefault().baseSize,healthFontSize,{200,220,200,255});
            if(temp.getReloading()){
                RL_DrawText("Reloading...",screenWidth/2.0f- MeasureText("Reloading...",40)/2,screenHeight/2.0f- GetFontDefault().baseSize,40,{200,220,200,255});
            }
            if(temp.getInventory()[temp.getCurrGun()].out){
                RL_DrawText(TextFormat("Ammo: %i/",temp.getInventory()[temp.getCurrGun()].currAmmo),screenWidth-MeasureText(TextFormat("%i",temp.getInventory()[temp.getCurrGun()].maxAmmo),ammoFontSize)-
                                                                                                    MeasureText(TextFormat("Ammo: %i/",temp.getInventory()[temp.getCurrGun()].currAmmo,ammoFontSize),ammoFontSize)-ammoFontSize/2,
                            GetFontDefault().baseSize*3,ammoFontSize,{255,200,200,255});
                RL_DrawText(TextFormat("%i",temp.getInventory()[temp.getCurrGun()].maxAmmo),screenWidth-MeasureText(TextFormat("%i",temp.getInventory()[temp.getCurrGun()].maxAmmo),ammoFontSize)-ammoFontSize/2,
                            GetFontDefault().baseSize*3,ammoFontSize,{255,200,200,255});
                if(!temp.getReloading()){
                    RL_DrawText("Press R to Reload",screenWidth/2.0f- MeasureText("Press R to Reload",40)/2,screenHeight/2.0f- GetFontDefault().baseSize,40,{255,100,100,255});
                }
            }else if(temp.getInventory()[temp.getCurrGun()].type != "melee"){
                RL_DrawText(TextFormat("Ammo: %i/",temp.getInventory()[temp.getCurrGun()].currAmmo),screenWidth-MeasureText(TextFormat("%i",temp.getInventory()[temp.getCurrGun()].maxAmmo),ammoFontSize)-
                                                                                                    MeasureText(TextFormat("Ammo: %i/",temp.getInventory()[temp.getCurrGun()].currAmmo,ammoFontSize),ammoFontSize)-ammoFontSize/2,
                            GetFontDefault().baseSize*3,ammoFontSize,{200,220,200,255});
                RL_DrawText(TextFormat("%i",temp.getInventory()[temp.getCurrGun()].maxAmmo),screenWidth-MeasureText(TextFormat("%i",temp.getInventory()[temp.getCurrGun()].maxAmmo),ammoFontSize)-ammoFontSize/2,
                            GetFontDefault().baseSize*3,ammoFontSize,{200,220,200,255});
            }
            RL_DrawText(temp.getInventory()[temp.getCurrGun()].name.c_str(),screenWidth-MeasureText(temp.getInventory()[temp.getCurrGun()].name.c_str(),ammoFontSize)-ammoFontSize/2,
                        GetFontDefault().baseSize*6,ammoFontSize,{200,220,200,255});
            EndBlendMode();

            EndDrawing();

        }
    }
    for(auto & model : models){
        UnloadModel(model);
    }
    UnloadTexture(grass);
    UnloadModel(dummy);
    UnloadModel(phil);
    UnloadModel(freddy);
    UnloadTexture(AR_Texture);
    UnloadTexture(brick);
    UnloadTexture(sky);
    UnloadTexture(skyBlue);
    UnloadTexture(hands);
    UnloadTexture(pistol);
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
void DrawNestedMenu(Rectangle btnRect) {
    //TODO: display relevant Nested Menu
// Check for button clicks or user input to go back to the main menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        isNestedMenu = false;
    }
}