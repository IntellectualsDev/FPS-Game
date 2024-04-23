

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
<<<<<<< HEAD
    Transmitter transmitter("172.17.8.211",6565,outputBuffer,consoleMutex);
    PacketBuffer inputBuffer(consoleMutex);
    Gateway gateway("172.17.8.211",5808,inputBuffer);
=======
    Transmitter transmitter("192.168.56.1",5452,outputBuffer,consoleMutex);
    PacketBuffer inputBuffer(consoleMutex);
    Gateway gateway("192.168.56.1",5453,inputBuffer);
>>>>>>> 550e05ef4bdf2e0a8fb9d4f5a36438ce19f2141f
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
<<<<<<< HEAD

    InitWindow(screenWidth-400, screenHeight-400, "Shooter Game");
=======
    InitWindow(screenWidth, screenHeight, "Shooter Game");
>>>>>>> 550e05ef4bdf2e0a8fb9d4f5a36438ce19f2141f
    //set fps
    SetTargetFPS(120);
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
        if(inputBuffer.getNumberOfPackets() > 0){
            auto toProcess = inputBuffer.removePacketInstant();
            {
                {
                    std::lock_guard<std::mutex> guard(consoleMutex);
                    for(auto& packet : toProcess){

                        cout << "Packet Label: " << EnumNamePacketType(packet->getPacketView()->packet_type()) << endl;
                        cout << "Source Point: (IP: " << packet->getPacketView()->source_point()->address() << ", Port: " << packet->getPacketView()->source_point()->port() << ") " << endl;
                        cout << "Destination Point: (IP: " << packet->getPacketView()->dest_point()->address() << ", Port: " << packet->getPacketView()->dest_point()->port() << ") " << endl;
                        cout << "Lobby Number: " << packet->getPacketView()->lobby_number() << endl;
                        cout << "Client Tick Number: " << packet->getPacketView()->client_tick()->tick_number() << endl;
                        cout << "Payload: \n\t: ";
                        cout << "Payload Type: " << EnumNamePayloadTypes(packet->getPacketView()->payload()->payload_type()) << endl;
                        cout << "\tdt: " << packet->getPacketView()->client_tick()->dt() << endl;
                        cout << "\tW: " << packet->getPacketView()->payload()->payload_as_Input()->w() << ", A: " << packet->getPacketView()->payload()->payload_as_Input()->a() << " S: " << packet->getPacketView()->payload()->payload_as_Input()->s() << " D: " << packet->getPacketView()->payload()->payload_as_Input()->d() << endl;
                        cout << "\tMouseDelta: (" << packet->getPacketView()->payload()->payload_as_Input()->mouse_delta()->x() << ", " << packet->getPacketView()->payload()->payload_as_Input()->mouse_delta()->y() << ")" << endl;
                        cout << "\tShoot: " << to_string(packet->getPacketView()->payload()->payload_as_Input()->shoot()) << ", Jump: " << to_string(packet->getPacketView()->payload()->payload_as_Input()->space()) << ", Sprint: " << to_string(packet->getPacketView()->payload()->payload_as_Input()->sprint()) << endl;
                        cout << "\tPrevious State: (" << packet->getPacketView()->payload()->payload_as_Input()->previous_position()->x() << ", " << packet->getPacketView()->payload()->payload_as_Input()->previous_position()->y() << ", " << packet->getPacketView()->payload()->payload_as_Input()->previous_position()->z() << ")\n" << endl;
                    }
                    // TODO: Enter a decision tree determining what authoritative decision is required
                }
            }
        }


        prevPosition = temp.getPosition();

        //read local inputs and update player positions/spawn bullet entities
        flatbuffers::FlatBufferBuilder builder(1024);
        auto destAddr = builder.CreateString("192.168.1.12");
        auto srcAddr = builder.CreateString("192.168.56.1");
//        const auto waste = OD_Vector2(GetMouseDelta().x,GetMouseDelta().y);
        const auto waste = CreateOD_Vector2(builder, GetMouseDelta().x, GetMouseDelta().y);
//        const auto waste2 = OD_Vector3(prevPosition.x,prevPosition.y,prevPosition.z);
        const auto waste2 = CreateOD_Vector3(builder, prevPosition.x, prevPosition.y, prevPosition.z);
        auto dest = CreateDestPoint(builder,destAddr,5808);
        auto src = CreateSourcePoint(builder,srcAddr,6565);
        auto tickOffset = CreateTick(builder, tick, GetFrameTime());
//        auto input = CreateInput(builder,0,IsKeyDown(KEY_W),IsKeyDown(KEY_A),
//                                 IsKeyDown(KEY_S),IsKeyDown(KEY_D),&waste,IsMouseButtonDown(MOUSE_BUTTON_LEFT),IsKeyDown(KEY_SPACE)
//                ,GetFrameTime(),&waste2,IsKeyDown(KEY_LEFT_SHIFT)).Union();
//        auto input = CreateInput(builder
//                ,0
//                ,IsKeyDown(KEY_W)
//                ,IsKeyDown(KEY_A),
//                IsKeyDown(KEY_S),
//                IsKeyDown(KEY_D),
//                waste,IsMouseButtonDown(MOUSE_BUTTON_LEFT),
//                IsKeyDown(KEY_SPACE),
//                GetFrameTime(),
//                waste2,IsKeyDown(KEY_LEFT_SHIFT)
//                ).Union();
        auto mouseDeltaOffset = CreateOD_Vector2(builder, GetMouseDelta().x, GetMouseDelta().y);
        auto previousPositionOffset = CreateOD_Vector3(builder, prevPosition.x, prevPosition.y, prevPosition.z);

        auto input = CreateInput(builder,
                                 0,
                                 tickOffset,
                                 IsKeyDown(KEY_W),
                                 IsKeyDown(KEY_A),
                                 IsKeyDown(KEY_S),
                                 IsKeyDown(KEY_D),
                                 mouseDeltaOffset,
                                 IsMouseButtonDown(MOUSE_BUTTON_LEFT),
                                 IsKeyDown(KEY_SPACE),
                                 previousPositionOffset,
                                 IsKeyDown(KEY_LEFT_SHIFT),
                                 IsKeyDown(KEY_LEFT_CONTROL)
                );

        auto payload = CreatePayload(builder, PayloadTypes_Input, input.Union());
//        const auto ticked = Tick(tick,GetFrameTime());
        const auto ticked = CreateTick(builder, tick, GetFrameTime(), 60);
        OD_PacketBuilder packetBuilder(builder);

        packetBuilder.add_reliable(false);
        packetBuilder.add_dest_point(dest);
        packetBuilder.add_source_point(src);
        packetBuilder.add_packet_type(PacketType_Input);
        packetBuilder.add_lobby_number(0);
        packetBuilder.add_payload(payload);
//        packetBuilder.add_payload_type(PacketPayload_Input);
//        packetBuilder.add_tick(&ticked);
        packetBuilder.add_client_tick(ticked);
//        packetBuilder.add_payload(input);
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

