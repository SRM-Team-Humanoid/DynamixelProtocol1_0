#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <termios.h>
#define STDIN_FILENO 0
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include<iostream>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <map>
#include <iterator>
#include <vector>

#include "dynamixel_sdk.h"                                  // Uses Dynamixel SDK library


// Control table address
#define ADDR_MX_TORQUE_ENABLE           24                  // Control table address is different in Dynamixel model
#define ADDR_MX_GOAL_POSITION           30
#define ADDR_MX_PRESENT_POSITION        36


// Data Byte Length
#define LEN_MX_GOAL_POSITION            2
#define LEN_MX_PRESENT_POSITION         2

// Protocol version
#define PROTOCOL_VERSION                1.0                 // See which protocol version is used in the Dynamixel
// #define DEVICENAME                      "/dev/ttyUSB0"
// #define BAUDRATE                        1000000

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
// #define DXL_MINIMUM_POSITION_VALUE      100                 // Dynamixel will rotate between this value
// #define DXL_MAXIMUM_POSITION_VALUE      4000                // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     10                  // Dynamixel moving status threshold

#define ESC_ASCII_VALUE                 0x1b

//define MX-28T and AX-12A resolution
#define MX_RESOLUTION                   0.08791208791
#define AX_RESOLUTION                   0.35190615836


//Create portHandler and packetHandler instance
dynamixel::PortHandler *portHandler;
dynamixel::PacketHandler *packetHandler;

long BAUDRATE = 1000000;
std::string DEVICENAME = "/dev/ttyUSB0";

int getch()
{
#if defined(__linux__) || defined(__APPLE__)
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
#elif defined(_WIN32) || defined(_WIN64)
  return _getch();
#endif
}


int kbhit(void)
{
#if defined(__linux__) || defined(__APPLE__)
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
#elif defined(_WIN32) || defined(_WIN64)
  return _kbhit();
#endif
}

void connect(){
  portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME.c_str());
  packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
  int dxl_comm_result = COMM_TX_FAIL;             // Communication result

  uint8_t dxl_error = 0;                          // Dynamixel error
  uint16_t dxl_model_number;                      // Dynamixel model number

  // Open port
  if (portHandler->openPort())
  {
    printf("Succeeded to open the port!\n");
  }
  else
  {
    printf("Failed to open the port!\n");
    printf("Press any key to terminate...\n");
    getch();
    return;
  }

  // Set port baudrate
  if (portHandler->setBaudRate(BAUDRATE))
  {
    printf("Succeeded to change the baudrate!\n");
  }
  else
  {
    printf("Failed to change the baudrate!\n");
    printf("Press any key to terminate...\n");
    getch();
    return;
  }

}

void disconnect(){
  //Close the serial port
  portHandler->closePort();
}


//Ping the dynamixel
void ping(int dxl_id){
  // Try to ping the Dynamixel
  // Get Dynamixel model number
  uint8_t dxl_error = 0;
  uint16_t dxl_model_number;
  int dxl_comm_result = COMM_TX_FAIL;

  dxl_comm_result = packetHandler->ping(portHandler, dxl_id, &dxl_model_number, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
  }
  else if (dxl_error != 0)
  {
    printf("%s\n", packetHandler->getRxPacketError(dxl_error));
  }
  else
  {
    printf("[ID:%03d] ping Succeeded. Dynamixel model number : %d\n", dxl_id, dxl_model_number);
  }
  return;
}

//Enable dynamixel Torque
void enable_torque(int dxl_id){
  uint8_t dxl_error = 0;
  int dxl_comm_result = COMM_TX_FAIL;
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, dxl_id, ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    //printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
  }
  else if (dxl_error != 0)
  {
    //printf("%s\n", packetHandler->getRxPacketError(dxl_error));
  }
}

//Disable Dynamixel Torque
void disable_torque(int dxl_id){
  uint8_t dxl_error = 0;
  int dxl_comm_result = COMM_TX_FAIL;
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, dxl_id, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
  }
  else if (dxl_error != 0)
  {
    printf("%s\n", packetHandler->getRxPacketError(dxl_error));
  }
}


// Set goal position
void write_angle(int dxl_id, float goal_position){
  uint8_t dxl_error = 0;
  uint16_t dxl_present_position;
  int dxl_comm_result = COMM_TX_FAIL;
  uint16_t dxl_goal_position = goal_position/MX_RESOLUTION;
  std::cout<<"position: "<<dxl_goal_position<<" "<<sizeof(dxl_goal_position)<<"\n";

  //Enable Dynamixel Torque
  enable_torque(dxl_id);

  // Write goal position
  dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, dxl_id, ADDR_MX_GOAL_POSITION, dxl_goal_position, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    //printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
  }
  else if (dxl_error != 0)
  {
    //printf("%s\n", packetHandler->getRxPacketError(dxl_error));
  }

  do{
      // Read present position
      dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, dxl_id, ADDR_MX_PRESENT_POSITION, &dxl_present_position, &dxl_error);
      if (dxl_comm_result != COMM_SUCCESS)
      {
        printf("%d ",dxl_id);
        printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
      }
      else if (dxl_error != 0)
      {
	printf("%d ",dxl_id);
        printf("%s\n", packetHandler->getRxPacketError(dxl_error));
      }
      else{
        //printf("%d",dxl_present_position);
      }
  }while((abs(dxl_goal_position - dxl_present_position) > DXL_MOVING_STATUS_THRESHOLD));
  // disable_torque(dxl_id);
}



// Read present angle of given id
float read_angle(int id){
  uint8_t dxl_error = 0;
  uint16_t dxl_present_position = 0;
  uint8_t dxl_id = id;

  float dxl_position_degree;

  int dxl_comm_result = COMM_TX_FAIL;

  dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, dxl_id, ADDR_MX_PRESENT_POSITION, &dxl_present_position, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
    return -999;
  }
  else if (dxl_error != 0)
  {
    printf("%s\n", packetHandler->getRxPacketError(dxl_error));
    return -999;
  }
  dxl_position_degree = dxl_present_position*MX_RESOLUTION;
  return dxl_position_degree;
}




// Read angles in a dynamixel chain
std::map<int, float> read_angle(std::vector <int> dxl_chain_id){
  uint8_t dxl_error = 0;
  uint16_t dxl_present_position;
  uint8_t dxl_id;

  float dxl_position_degree;

  std::map<int,float> dxl_chain_map;
  std::vector<int>::iterator vector_itr;

  for(vector_itr = dxl_chain_id.begin(); vector_itr != dxl_chain_id.end(); vector_itr++){
    dxl_id = *vector_itr;
    int dxl_comm_result = COMM_TX_FAIL;
    dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, dxl_id, ADDR_MX_PRESENT_POSITION, &dxl_present_position, &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
      printf("first wala");
      printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
    }
    else if (dxl_error != 0)
    {
      printf("%s\n", packetHandler->getRxPacketError(dxl_error));
      printf("second wala");
    }

    dxl_position_degree = dxl_present_position*MX_RESOLUTION;

    dxl_chain_map.insert(std::pair<int,float>((int)dxl_id,dxl_position_degree));
  }
  return dxl_chain_map;
}




// Sync Write Function
void write_angle(std::map<int, float>dxl_chain_map){
  // Initialize GroupSyncWrite instance
  dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR_MX_GOAL_POSITION, LEN_MX_GOAL_POSITION);

  std::map<int ,float>::iterator itr;
  int dxl_comm_result = COMM_TX_FAIL;
  bool dxl_addparam_result = false;

  uint8_t dxl_error = 0;
  uint8_t param_goal_position[2], dxl_id, max_id;
  uint16_t dxl_goal_position, dxl_present_position;

  float angle,diff,max_angle;


  //Add Dynamixel Goal Positions to Syncwrite parameter
  for(itr = dxl_chain_map.begin() ; itr != dxl_chain_map.end(); itr++){
    dxl_id = itr->first;
    enable_torque(dxl_id);
    angle = read_angle(dxl_id);
    diff = abs(angle-itr->second);
    if (diff>max_angle){
      max_angle = diff;
      max_id = dxl_id;
    }
    dxl_goal_position = itr->second/MX_RESOLUTION;
    std::cout << dxl_goal_position << '\n';

    param_goal_position[0] = DXL_LOBYTE(dxl_goal_position);
    param_goal_position[1] = DXL_HIBYTE(dxl_goal_position);
    dxl_addparam_result = groupSyncWrite.addParam(dxl_id,param_goal_position);

    if (dxl_addparam_result != true)
    {
      fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", itr->first);
      return;
    }
  }



  // Syncwrite goal position
  dxl_comm_result = groupSyncWrite.txPacket();
  if (dxl_comm_result != COMM_SUCCESS) printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));

  // Clear syncwrite parameter storage
  groupSyncWrite.clearParam();

  // Wait for all motors to reach goal Position
  do{
      // Read present position
      dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, max_id, ADDR_MX_PRESENT_POSITION, &dxl_present_position, &dxl_error);
      if (dxl_comm_result != COMM_SUCCESS)
      {
        printf("Yeh wala\n");
        printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
      }
      else if (dxl_error != 0)
      {
        printf("%s\n", packetHandler->getRxPacketError(dxl_error));
      }
  }while((abs(dxl_chain_map[max_id] - dxl_present_position) > DXL_MOVING_STATUS_THRESHOLD));

}
