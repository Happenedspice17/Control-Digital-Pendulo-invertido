% Ogata, Ejemplo 4-9: controlador digital por lugar de las raices en el plano z
clear; clc;

T = 0.2; zeta = 0.5; ts = 2;
wn = 4/(zeta*ts);
wd = wn*sqrt(1 - zeta^2);

% Polo deseado en z
z1 = exp(-T*zeta*wn) * exp(1j*T*wd);
fprintf('z1 = %.4f %+.4fj  (|z|=%.4f, ang=%.2f deg)\n', real(z1), imag(z1), abs(z1), rad2deg(angle(z1)));

% Planta discretizada con ZOH
Gs = tf(1, [1 2 0]);
Gz = c2d(Gs, T, 'zoh')

% Controlador Gd(z) = K (z - a)/(z - b); el cero cancela el polo e^{-2T}
a = exp(-2*T);
[nz, dz] = tfdata(Gz, 'v');
Gc = minreal(tf(conv([1 -a], nz), dz, T));   % (z-a)G(z), sin K ni polo b

% Condicion de angulo: el polo b aporta lo que falta para -180 deg
thb = angle(evalfr(Gc, z1)) + pi;
thb = mod(thb + pi, 2*pi) - pi;
b = real(z1) - imag(z1)/tan(thb);

% Condicion de magnitud
Gd0 = tf([1 -a], [1 -b], T);
L0 = minreal(Gd0*Gz);
K = 1/abs(evalfr(L0, z1));
Gd = K*Gd0
L = minreal(K*L0);

fprintf('Gd(z) = %.4f (z - %.4f)/(z - %.4f)\n', K, a, b);

% Lazo cerrado y respuesta al escalon
M = feedback(L, 1)
figure; step(M, 4); grid on;
title('Respuesta al escalon unitario - Ejemplo 4-9');

figure; rlocus(L/K); zgrid(zeta, wn*T); hold on;
plot(real(z1), [imag(z1) -imag(z1)], 'rx', 'MarkerSize', 12, 'LineWidth', 2);
title('Lugar de las raices en z');

% Constante de error de velocidad estatica
Kv = dcgain(minreal(tf([1 -1], [1 0], T)*L)) / T;
fprintf('Kv = %.4f\n', Kv);
