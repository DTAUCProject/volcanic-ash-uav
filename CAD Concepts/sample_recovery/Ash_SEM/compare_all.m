clc,clear,close all

load('data_ash_take_1.mat');
diameter_small = diameter;
clear('diameter');
load('data_ash_take_2.mat');
diameter_large = diameter;
clear('diameter');
load('data_ash_take_316.mat');
large = diameter';
clear('diameter');
load('data_ash_take_317.mat');
large_2 = diameter';
clear('diameter');

l = length(large);
for i = 1:length(large_2)
    large(l + i) = large_2(i);
end

large

mean(diameter_small);
mean(diameter_large);

% histogram(diameter_large,17);
% title('Large cyclone');
% xlabel('Particle size um');
% ylabel('Particle count');
% figure
% histogram(diameter_small,17);
% title('Small cyclone');
% xlabel('Particle size um');
% ylabel('Particle count');
histogram(large,20);
title('Rotary cyclone');
xlabel('Particle size um');
ylabel('Particle count');
% set(gca,'XScale','log');











