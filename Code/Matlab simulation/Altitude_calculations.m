clear,clc,close all

mass = 2;
height = 12192;
sea_level_roh = 1.225;
front_area = 0.05*1.5;
velocity = 50/3.6;
gradiant = 1/10;
vertical_velocity = velocity*gradiant;
time_s = 12000/vertical_velocity;

for height = 1:12000/1000
    velocity(height) = (-100*sea_level_roh/height+120)/3.6;
    roh(height) = sea_level_roh/height;
    drag_power(height) = 0.5*roh(height)*(velocity(height))^3*front_area*0.5;
    heigh_energy(height) = mass*height*9.81*1000;
    
end

% drag_energy = zeros(1,12000);
% clear('height')
% for height = 2:12000
%     if height < 1000 
%         drag_power_c = drag_power(1);
%     elseif height < 2000 && height > 1001
%         drag_power_c = drag_power(2);
%     elseif height < 3000 && height > 2001
%         drag_power_c = drag_power(3);
%     elseif height < 4000 && height > 3001
%         drag_power_c = drag_power(4);
%     elseif height < 5000 && height > 4001
%         drag_power_c = drag_power(5);
%     elseif height < 6000 && height > 5001
%         drag_power_c = drag_power(6);
%     elseif height < 7000 && height > 6001
%         drag_power_c = drag_power(7);
%     elseif height < 8000 && height > 7001
%         drag_power_c = drag_power(8);
%     elseif height < 9000 && height > 8001
%         drag_power_c = drag_power(9);
%     elseif height < 10000 && height > 9001
%         drag_power_c = drag_power(10);
%     elseif height < 11000 && height > 10001
%         drag_power_c = drag_power(11);
%     end
%     drag_energy(height) = drag_energy(height-1) + drag_power_c*height;
%     
% end


plot(drag_power)

grid on







