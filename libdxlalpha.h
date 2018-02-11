#include<stdint.h>
#include<map>
#include<vector>
int getch();
void initBaudrate(int);
int kbhit();
void write_angle(int, float);
void write_angle(std::map<int,float>);
void ping(int);
void connect();
void disconnect();
float read_angle(int);
std::map<int, float> read_angle(std::vector <int> dxl_chain_id);
