

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
#include "DummyClient.h"
#include "Gateway.h"
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
mutex consoleMutex;
int main(void)
{
//    if (enet_initialize () != 0)
//    {
//        fprintf (stderr, "An error occurred while initializing ENet.\n");
//        return EXIT_FAILURE;
//    }
//    atexit (enet_deinitialize);
//
//    DummyClient client = DummyClient();
//
//     this_thread::sleep_for(chrono::seconds(3));
//
//    while(true){
//        client.connect("192.168.1.12", 5450);
//        client.sendPacket("You Smell");
//        client.sendPacket("Hehe");
//    }
//    return 0;






    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet\n";
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);


    PacketBuffer outputBuffer(consoleMutex);
    Transmitter transmitter("192.168.56.1",5452,outputBuffer,consoleMutex);
    PacketBuffer inputBuffer(consoleMutex);
    Gateway gateway("192.168.56.1",5453,inputBuffer);
    //TODO
    //implement join sequence

    //TODO
    //assign stuff based on server
    //also interpolate
    FPSClientState player2;

    //init local player object
    Player temp((Vector3){0,5,1},(Vector3){0,0,0},(Vector3){1.0f,2.0f,1.0f});

    //init terrain vector
    float wallWidth = 1.0f;
    float wallHeight = 5.0f;
    float wallLength = 32.0f;
    float floorLength = 50.0f;
    BoundingBox wallBox1 = (BoundingBox){(Vector3){-16.0f - wallWidth/2, 2.5f - wallHeight/2, 0.0f -wallLength/2},(Vector3){-16.0f + wallWidth/2, 2.0f + wallHeight/2, 0.0f +wallLength/2}};
    BoundingBox wallBox2 = (BoundingBox){(Vector3){16.0f - wallWidth/2, 2.5f - wallHeight/2, 0.0f -wallLength/2},(Vector3){16.0f + wallWidth/2, 2.0f + wallHeight/2, 0.0f +wallLength/2}};
    BoundingBox wallBox3 = (BoundingBox){(Vector3){0.0f - wallLength/2, 2.5f - wallHeight/2, 16.0f -wallWidth/2},(Vector3){0.0f + wallLength/2, 2.0f + wallHeight/2, 16.0f +wallWidth/2}};
    BoundingBox floorBox = (BoundingBox){(Vector3){-floorLength/2,-1,-floorLength/2},(Vector3){floorLength/2,0,floorLength/2}};
    vector<BoundingBox> terrainVector = {wallBox1,wallBox2,wallBox3,floorBox};
    BoundingBox topBox1 = (BoundingBox){(Vector3){-16.0f - 0.5+0.1f, 4.99f, 0.0f +0.1f-wallLength/2},(Vector3){-16.0f + 0.5-0.1f, 2.5f + wallHeight/2, 0.0f -0.1f+wallLength/2}};
    BoundingBox topBox2 = (BoundingBox){(Vector3){16.0f - 0.5+0.1f, 4.99f, 0.0f +0.1f-wallLength/2},(Vector3){16.0f + 0.5-0.1f, 2.5f + wallHeight/2, 0.0f -0.1f+wallLength/2}};
    BoundingBox topBox3 = (BoundingBox){(Vector3){0.0f+0.1f - wallLength/2, 4.99f, 16.0f -0.5+0.1f},(Vector3){0.0f -0.1f+ wallLength/2, 2.5f + wallHeight/2, 16.0f -0.1f+ 0.5}};
    vector<BoundingBox> topBoxVector = {topBox1,topBox2,topBox3};
    //init window
    //TODO shit code
    InitWindow(0, 0, "Shooter Game");
    const int screenWidth = GetMonitorWidth(0);
    const int screenHeight = GetMonitorHeight(0);
    Rl_CloseWindow();
    InitWindow(screenWidth-600, screenHeight-400, "Shooter Game");
    //set fps
    SetTargetFPS(60);
    //disable cursor
    DisableCursor();
    //init previous position
    Vector3 prevPosition;
    //vector of all bullets that have been spawned by local player
    vector<Bullet>& ref = *temp.getEntities();
    //Main Loop
    //START THE TRANSMITTER AND GATEWAY
    transmitter.start();
    gateway.start();
    int tick = 1;
    while (!WindowShouldClose()){
        //get previous position
        if(tick%60 == 0){
            tick = 1;
        }else{
            tick +=1;
        }
        //TODO
        //implement checking incoming packets against past packet buffer and update client position based off that
        prevPosition = temp.getPosition();

        //read local inputs and update player positions/spawn bullet entities
        flatbuffers::FlatBufferBuilder builder(1024);
        auto destAddr = builder.CreateString("192.168.1.12");
        auto srcAddr = builder.CreateString("192.168.56.1");
        const auto waste = OD_Vector2(GetMouseDelta().x,GetMouseDelta().y);
        const auto waste2 = OD_Vector3(prevPosition.x,prevPosition.y,prevPosition.z);
        auto dest = CreateDestPoint(builder,destAddr,5450);
        auto src = CreateSourcePoint(builder,srcAddr,6565);
        auto input = CreateInput(builder,0,IsKeyDown(KEY_W),IsKeyDown(KEY_A),
                                 IsKeyDown(KEY_S),IsKeyDown(KEY_D),&waste,IsMouseButtonDown(MOUSE_BUTTON_LEFT),IsKeyDown(KEY_SPACE)
                ,GetFrameTime(),&waste2,IsKeyDown(KEY_LEFT_SHIFT)).Union();
        const auto ticked = Tick(tick,GetFrameTime());
        OD_PacketBuilder packetBuilder(builder);

        packetBuilder.add_reliable(false);
        packetBuilder.add_dest_point(dest);
        packetBuilder.add_source_point(src);
        packetBuilder.add_packet_type(PacketType_Input);
        packetBuilder.add_lobby_number(0);
        packetBuilder.add_payload_type(PacketPayload_Input);
        packetBuilder.add_tick(&ticked);
        packetBuilder.add_payload(input);
        auto packet = packetBuilder.Finish();
        builder.Finish(packet);
        uint8_t* buffer = builder.GetBufferPointer();
        size_t bufferSize = builder.GetSize();
        enet_uint32 flags = 0;
        if(GetOD_Packet(buffer)->reliable()){
            flags = ENET_PACKET_FLAG_RELIABLE;
        }
        // Create an ENetPacket from the serialized data
        ENetPacket* packetToSend = enet_packet_create(buffer, bufferSize, flags);
        std::unique_ptr<ENetPacket>finalPacket(packetToSend);
//        {
//            std::lock_guard<std::mutex> guard(consoleMutex);
//            cout << "Tick in main:" << tick << endl;
//        }
        outputBuffer.addPacket(std::move(finalPacket));

        temp.UpdatePlayer(IsKeyDown(KEY_W),IsKeyDown(KEY_A),IsKeyDown(KEY_S),IsKeyDown(KEY_D),GetMouseDelta(),
                          IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_SPACE),GetFrameTime(),prevPosition,terrainVector,topBoxVector,
                          IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL),outputBuffer);

        //update bullet entities position based on a multiple of the frame time(delta T)
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

    Rl_CloseWindow();        // Close window and OpenGL context

    return 0;
}

