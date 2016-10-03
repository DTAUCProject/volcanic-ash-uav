% Power calculations
clear, clc

rho_0 = 1.2250; % air density at sea level
T_0 = 288.15; % temperature at sea level
g = 9.80665; % Acceleration due to gravity
M = 0.0289644; % Molar mass of air on earth (Kg/mol)
R = 8.31432; % Universal gas constant for air

h_max = 10e3;
h_step = 100;
j = 1;
for i = 1:h_step:h_max
    h(j) = h_max - i;
    j = j + 1;
end

rho = rho_0*exp(-g*M*h/R/T_0);

m = 2; %kg
height = 10000;


% Assume wind speed is at 20 kmph average
glide_distance = 30000; % m

Ep = m * g * height;
Vbatt = 11.1;
Echem = 3600 * Vbatt * 10;

%from last years project
CdAd = 0.0212;
ClAl = 0.0625;

% glide slope
theta = atan(CdAd/ClAl);

K = ClAl*cos(theta) + CdAd*sin(theta);
for i = 1:length(rho)
   a(i) = sqrt(2*m*g/rho(i)/K);
end

plot(h,a)
xlabel('altitude (m)');
ylabel('airspeed (m/s)');
%a = sqrt(2*m*g/rho/K) % airspeed

OPC_I_testing = 175; %mA when testing
OPC_I_idle = 95; %mA when idle
OPC_weigt = 105; %grams
OPC_voltage = 5; %V
OPC_power = (175 * 5)/1000; %W

telem_I = 1;

PIXHAWK_I = 280/1000; % mA (Pixhawk running APM:Plane 3.3.0, 
                    %GPS/Compass modules, safety switch, 
                    %buzzer, 433 MHz telemetry radio)
                    
RPi_temp_requirement = 0; % some peripherals aren't guaranteed below zero.
RPi_I = 350/1000; % mA

Motor_I = 20; %????? A

% P = E / s
% Power consumed -> P = I*V
const_current_drawn = PIXHAWK_I + RPi_I + telem_I + OPC_I_testing/1000;
consumed_power = OPC_power + Motor_I * Vbatt + RPi_I * 5 + PIXHAWK_I * 5;
consumed_energy = consumed_power * 3600;

% Distance travelled
dist_out = 30000; %km
glide_dist = 30000; %km
rise_time = 30*60; % 30 minutes
battery_size = 8; % Ah

descent_time = dist_out / 10; % 10 m/s to travel 30km
tot_time = rise_time + descent_time
tot_const_current_drawn = const_current_drawn *(tot_time / 3600);

Ah_left = battery_size - tot_const_current_drawn;
powered_dist =  (Ah_left / 15) * 36000

%% Heat Calculations
clc
% Four walls to lose heat from

v = 5; % m/s 
wall_thickness = 0.01; % m
body_length = 0.8; % m
body_width = 0.13; % m
A_plane = body_width * body_length; % m^2
outside_temp = -20; %C
inside_temp = 10; %C

% Convection: Qcv = h * A * theta
% eqn from http://www.engineeringtoolbox.com/convective-heat-transfer-d_430.html
hc = 10.45 - v + 10 * v^(1/2); %W / (m^2)*K

Qcv = hc * A_plane * (inside_temp - outside_temp)

% Conduction: Qcn = K * A * (dT / s) where s = wall thickness
% eqn from http://www.engineeringtoolbox.com/conductive-heat-transfer-d_428.html
K = 0.033; %Thermal conductivity of polystyrene (W / m*K)
dT = inside_temp - outside_temp;

Qcn = K * A_plane * (dT / wall_thickness)

% Radiation Qrn = e * sigma * A * (Tin^4 - Tout^4)
e = 0.6;
sigma = 5.6703e-8; 
inside_temp_K = 273.15 + inside_temp;
outside_temp_K = 273.15 + outside_temp;
Qrn = e * sigma * A_plane * (inside_temp_K^4 - outside_temp_K^4)

% Total heat loss = Qcn + Qcv + Qr
Qtot = Qcn + Qcv + Qrn;
% four sides of plane
Qtot = Qtot * 4

% Heat produced by the components








