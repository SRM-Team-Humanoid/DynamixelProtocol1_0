#include "libdxlalpha.h"
#include<iostream>
#include<vector>
#include<stdlib.h>
#include<map>
#include<iterator>

int main(int argc, char *argv[]){
  // int dxl_id[] = {1,4};
  int dxl_id,i;
  float goal_position;
  connect();
  float angle;
  
  //std::cin>>dxl_id;
  //std::cin>>goal_position;
  std::map<int,float>dxl;
  // std::vector<int> dxl_v(dxl_id, dxl_id + sizeof(dxl_id) / sizeof(dxl_id[0]) );
  for(i=1; i<20; i++){
	if(i==4 || i==6)
		continue;
	write_angle(i, 180.0);
       	std::cout<<i<<"\n";
  }
  // for(goal_position = 0; goal_position<100000;goal_position++){
  //   std::cout <<"Waiting" << '\n';
  // }
  //angle = read_angle(dxl_id);
  // dxl.insert(std::pair<int,double>(21,200.0));
   //dxl.insert(std::pair<int,double>(22,160.0));
   //write_angle(dxl);
  // std::map<int,double>dxl2;
  // dxl2.insert(std::pair<int,double>(6,4095));
  // dxl2.insert(std::pair<int,double>(8,1023));
  // write_angle(dxl2);
  // dxl = read_angle(dxl_v);
  // std::map<int, float>::iterator itr;
  // for(itr = dxl.begin(); itr!=dxl.end(); itr++){
  //   std::cout<<itr->first<<" "<<itr->second<<"\n";
  // }
  // write_angle(dxl_id,360);
  //std::cout << "angle:  "<<angle << '\n';
  disconnect();
  return 0;
}
