

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
    Transmitter transmitter("192.168.1.13",5452,outputBuffer,consoleMutex);
    PacketBuffer inputBuffer(consoleMutex);
    Gateway gateway("192.168.1.13",5453,inputBuffer);
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
    BoundingBox topBox1 = (BoundingBox){(Vector3){-16.0f - 0.5+0.05f, 4.99f, 0.0f +0.05f-wallLength/2},(Vector3){-16.0f + 0.5-0.05f, 2.5f + wallHeight/2, 0.0f -0.05f+wallLength/2}};
    BoundingBox topBox2 = (BoundingBox){(Vector3){16.0f - 0.5+0.05f, 4.99f, 0.0f +0.05f-wallLength/2},(Vector3){16.0f + 0.5-0.05f, 2.5f + wallHeight/2, 0.0f -0.05f+wallLength/2}};
    BoundingBox topBox3 = (BoundingBox){(Vector3){0.0f+0.05f - wallLength/2, 4.99f, 16.0f -0.5+0.05f},(Vector3){0.0f -0.05f+ wallLength/2, 2.5f + wallHeight/2, 16.0f -0.05f+ 0.5}};
    vector<BoundingBox> topBoxVector = {topBox1,topBox2,topBox3};
    //init window
    //TODO shit code
    InitWindow(0, 0, "Shooter Game");
    const int screenWidth = GetMonitorWidth(0);
    const int screenHeight = GetMonitorHeight(0);
    Rl_CloseWindow();
    InitWindow(screenWidth, screenHeight, "Shooter Game");
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
        auto destAddr = builder.CreateString("192.168.1.13");
        auto srcAddr = builder.CreateString("192.168.56.1");
        const auto mouse = CreateOD_Vector2(builder,GetMouseDelta().x,GetMouseDelta().y);
        auto dest = CreateDestPoint(builder,destAddr,5450);
        auto src = CreateSourcePoint(builder,srcAddr,6565);
        const auto ticked = CreateTick(builder,tick,GetFrameTime());
        const auto prev = CreateOD_Vector3(builder,prevPosition.x,prevPosition.y,prevPosition.z);
        vector<flatbuffers::Offset<Buffer>> buffers{};
        auto buffersVector = builder.CreateVector(buffers);


//        flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<Buffer>>> opponent_buffer_ticks = 0;

//        auto input = CreateInput(builder,0,ticked,IsKeyDown(KEY_W),IsKeyDown(KEY_A),
//                                 IsKeyDown(KEY_S),IsKeyDown(KEY_D),mouse,IsMouseButtonDown(MOUSE_BUTTON_LEFT),IsKeyDown(KEY_SPACE)
//                ,prev,IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL), opponent_buffer_ticks);//implement interpolation buffer
//        vector<flatbuffers::Offset<Input>> historyBuf;
//        flatbuffers::Vector<flatbuffers::Offset<Input>> historyBuf;

//        std::vector<Input> clientInputs;
//        for(auto inputTemp : historyBuf){
//            auto dref = *inputTemp;
//            clientInputs.push_back(*inputTemp);
//        }

//        historyBuf.push_back(input);
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
        historyBufOffsetVector.push_back(input);
        auto bufferVector = builder.CreateVector(historyBufOffsetVector);
//        auto payload = CreatePayload(builder,PayloadTypes_ClientInputs,bufferVector.Union());
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

//        const OD_Packet* checkPacket = GetOD_Packet(buffer);
//        if (checkPacket->payload()->payload_type() == PayloadTypes_ClientInputs) {
//            auto clientInputs = checkPacket->payload()->payload_as_ClientInputs();
//            bool temp = clientInputs ->client_inputs() != nullptr;
//            if (clientInputs && clientInputs->client_inputs()->size() > 0) {
//                auto w = clientInputs->client_inputs()->Get(0)->w();
//                std::cout << "Buffer contains inputs" << std::endl;
//            } else {
//                std::cout << "No inputs in buffer" << std::endl;
//            }
//        }


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
        //TODO
        //client side prediction
        temp.UpdatePlayer(IsKeyDown(KEY_W),IsKeyDown(KEY_A),IsKeyDown(KEY_S),IsKeyDown(KEY_D),GetMouseDelta(),
                          IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_SPACE),GetFrameTime(),prevPosition,terrainVector,topBoxVector,
                          IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL),outputBuffer);

        //update bullet entities position based on a multiple of the frame time(delta T)
        //check for all Collisions

        //TODO put it function or put somewhere else
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

