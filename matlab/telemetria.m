%% Captura de telemetría desde la placa (comando 't')
puerto = "COM5";          % Linux: "/dev/ttyUSB0"
dur    = 20;              % s

sp = serialport(puerto, 115200);
configureTerminator(sp, "LF");
flush(sp);
writeline(sp, "t");

d = [];
t0 = tic;
while toc(t0) < dur
    l = readline(sp);
    if startsWith(l, "#") || strlength(l) == 0, continue; end
    v = str2double(split(l, ","));
    if numel(v) == 8 && all(~isnan(v)), d(end+1,:) = v.'; end %#ok<AGROW>
end
writeline(sp, "t");
clear sp;

% columnas: t_ms, x, v, theta, omega, a, x_ref, estado
t = (d(:,1) - d(1,1))/1000;
figure('Name', 'Telemetría');
subplot(3,1,1); plot(t, d(:,2), t, d(:,7), '--'); ylabel('x [m]'); grid on;
subplot(3,1,2); plot(t, rad2deg(d(:,4))); ylabel('\theta [°]'); grid on;
subplot(3,1,3); stairs(t, d(:,6)); ylabel('a [m/s^2]'); xlabel('t [s]'); grid on;
save(fullfile(fileparts(mfilename('fullpath')), 'telemetria.mat'), 'd');
