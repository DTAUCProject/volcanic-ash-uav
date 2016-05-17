clc,clear,close all

mass_needed = 0.5e-6;       % kg
speed = 5;                  % m/s
opening = 0.01:0.001:0.05;  % m
density = 4e-6;             % kg/m^3

area = pi*(opening./2).^2;  % m^2

flow_rate = speed*area;     % m^3/s

time_open = mass_needed./(density*flow_rate); % s
% plot(flow_rate)
hold on
plot(opening,time_open/60)
title('Time per sample in relation to sample landing area')
ylabel('Time open (min)')
xlabel('Diameter of landing area (m)')
grid on







