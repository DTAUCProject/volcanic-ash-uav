clc,clear,close all

mass_needed = 3e-6;      % kg
speed = 70/3.6;          % m/s
opening = 0.01;         % m
density = 4e-6;          % kg/m^3

area = pi*(opening/2)^2; % m^2

flow_rate = speed*area;  %m^3/s

time_open = mass_needed/(density*flow_rate);
flow_rate
time_open/60








