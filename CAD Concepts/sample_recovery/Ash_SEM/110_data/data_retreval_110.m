clear,clc,close all

area = [14.998,11.019,7.958,12.243,9.183,16.529,6.734,20.508,5.51,6.734,18.059,15.741,34.876,10.802,4.63,1.852,4.63,3.704,6.79,8.642];


diameter = sqrt(area/pi)*2;


histogram(diameter,17)


save('data_ash_take_1','diameter');