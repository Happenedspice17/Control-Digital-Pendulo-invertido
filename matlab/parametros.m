%% Parámetros físicos y de implementación del péndulo invertido
% Modelo con entrada = aceleración del carro (motor a pasos)

g = 9.81;

% Péndulo: varilla uniforme + masa opcional en la punta
m_var = 0.054;   % kg  (Al Ø8 mm x 400 mm)
L_var = 0.40;    % m
m_tip = 0.00;    % kg
L_tip = 0.40;    % m   (pivote -> masa de la punta)
b_p   = 0.0005;  % N·m·s/rad  fricción viscosa del pivote

ml = m_var*L_var/2 + m_tip*L_tip;          % m·l
J  = m_var*L_var^2/3 + m_tip*L_tip^2;      % inercia respecto al pivote
alpha = g*ml/J;
beta  = ml/J;
gam   = b_p/J;

% Implementación digital
Ts      = 1/500;            % s   periodo de muestreo
a_max   = 10;               % m/s^2
v_max   = 0.45;             % m/s
x_lim   = 0.20;             % m   recorrido útil ± desde el centro
enc_ppr = 1000;             % pulsos por vuelta del encoder
enc_cpr = 4*enc_ppr;        % cuentas en cuadratura
N_vel   = 5;                % ventana de derivada (muestras)
tau_w   = 0.004;            % s   filtro pasa-bajas de la velocidad angular

% Mecánica del carro
pasos_rev   = 200;
micropasos  = 16;
dientes     = 20;
paso_correa = 2e-3;         % GT2
pasos_m     = pasos_rev*micropasos/(dientes*paso_correa);

% Referencia tipo demo MathWorks (onda cuadrada)
ref_amp = 0.10;             % m
ref_T   = 8;                % s
