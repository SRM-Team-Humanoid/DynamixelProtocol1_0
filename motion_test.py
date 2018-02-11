import os
import time
os.system('cd && ./project/Dxl_alpha/dxlwrite 5 200')
time.sleep(1)

#
# for i in range(200,600,20):
#     os.system('cd && ./project/Dxl_alpha/dxlwrite 5 '+str(i))
#     #time.sleep(0.001)

for i in range(10):
    os.system('cd && ./project/Dxl_alpha/dxlwrite 5 600')
    os.system('cd && ./project/Dxl_alpha/dxlwrite 5 200')
