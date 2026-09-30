%% Diseño LQR discreto, simulación no lineal y exportación a firmware
clear; clc; close all;
aqui = fileparts(mfilename('fullpath'));
run(fullfile(aqui, 'parametros.m'));

%% Modelo lineal  s = [x; x_dot; theta; theta_dot],  u = aceleración del carro
A = [0 1 0     0;
     0 0 0     0;
     0 0 0     1;
     0 0 alpha -gam];
B = [0; 1; 0; -beta];
sysc = ss(A, B, eye(4), zeros(4,1));

fprintf('Polos lazo abierto: %s\n', mat2str(eig(A).', 4));
fprintf('Rango controlabilidad: %d / 4\n', rank(ctrb(A, B)));

sysd = c2d(sysc, Ts, 'zoh');
Ad = sysd.A;  Bd = sysd.B;

%% LQR discreto
Q = diag([200 10 100 1]);
R = 1;
[K, ~, pz] = dlqr(Ad, Bd, Q, R);
fprintf('K = [%.4f  %.4f  %.4f  %.4f]\n', K);
fprintf('|polos z| = %s\n', mat2str(abs(pz).', 4));
fprintf('Polos equivalentes s = %s\n', mat2str((log(pz)/Ts).', 4));

%% Exportar ganancias al firmware
gh = fullfile(aqui, '..', 'firmware', 'include', 'gains.h');
fid = fopen(gh, 'w');
fprintf(fid, '#pragma once\n// Generado por matlab/diseno_lqr.m  (%s)\n', datestr(now));
fprintf(fid, '// Q = diag(%s), R = %g, Ts = %g s\n', mat2str(diag(Q).'), R, Ts);
fprintf(fid, '#define CTL_HZ %d\n', round(1/Ts));
fprintf(fid, 'static const float K_LQR[4] = {%.6ff, %.6ff, %.6ff, %.6ff};\n', K);
fclose(fid);
fprintf('Ganancias escritas en %s\n', gh);

%% Simulación no lineal con efectos digitales
T_fin = 2*ref_T;
n     = round(T_fin/Ts);
sub   = 10;  h = Ts/sub;
q     = 2*pi/enc_cpr;

s    = [0; 0; deg2rad(3); 0];
hist = zeros(1, N_vel+1);
w_f  = 0;
lp   = Ts/(Ts+tau_w);

log_t = zeros(n,1); log_s = zeros(n,4); log_u = zeros(n,1); log_r = zeros(n,1);

f = @(s,u) [s(2); u; s(4); alpha*sin(s(3)) - beta*cos(s(3))*u - gam*s(4)];

for k = 1:n
    t  = (k-1)*Ts;
    xr = ref_amp * (1 - 2*(mod(t, ref_T) >= ref_T/2));

    th_q = round(s(3)/q)*q;
    hist = [hist(2:end) th_q];
    w_f  = w_f + lp*((hist(end) - hist(1))/(N_vel*Ts) - w_f);

    e = [s(1)-xr; s(2); th_q; w_f];
    u = min(max(-K*e, -a_max), a_max);

    for j = 1:sub
        k1 = f(s,u); k2 = f(s+h/2*k1,u); k3 = f(s+h/2*k2,u); k4 = f(s+h*k3,u);
        s  = s + h/6*(k1 + 2*k2 + 2*k3 + k4);
    end
    s(2) = min(max(s(2), -v_max), v_max);

    log_t(k) = t; log_s(k,:) = s.'; log_u(k) = u; log_r(k) = xr;
end

figure('Name', 'Respuesta');
subplot(4,1,1); plot(log_t, log_s(:,1), log_t, log_r, '--'); ylabel('x [m]'); grid on; legend('x','x_{ref}');
subplot(4,1,2); plot(log_t, log_s(:,2)); ylabel('v [m/s]'); grid on;
subplot(4,1,3); plot(log_t, rad2deg(log_s(:,3))); ylabel('\theta [°]'); grid on;
subplot(4,1,4); stairs(log_t, log_u); ylabel('a [m/s^2]'); xlabel('t [s]'); grid on;

%% Animación
figure('Name', 'Animación'); axis equal; grid on; hold on;
axis([-x_lim-0.1 x_lim+0.1 -0.1 L_var+0.1]);
plot([-x_lim x_lim], [0 0], 'k', 'LineWidth', 2);
carro = rectangle('Position', [-0.04 -0.02 0.08 0.04], 'FaceColor', [0.3 0.5 0.8]);
varil = plot([0 0], [0 L_var], 'r', 'LineWidth', 3);
refm  = plot(0, -0.05, 'k^', 'MarkerFaceColor', 'k');
for k = 1:10:n
    x = log_s(k,1); th = log_s(k,3);
    set(carro, 'Position', [x-0.04 -0.02 0.08 0.04]);
    set(varil, 'XData', [x x+L_var*sin(th)], 'YData', [0 L_var*cos(th)]);
    set(refm,  'XData', log_r(k));
    title(sprintf('t = %.2f s', log_t(k)));
    drawnow limitrate;
end
