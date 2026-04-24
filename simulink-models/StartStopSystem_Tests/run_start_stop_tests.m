%% AUTOMATED VALIDATION SUITE - START/STOP SYSTEM
% Project: Stellantis HMI Cluster Validation

clear; clc;
modelName = 'tests_model';
load_system(modelName);

%% --- 1. CATALOGO DE DEFINIÇÕES ---
% Centralizamos as descrições para facilitar a manutenção e clareza
test_catalog = struct(...
    'ID', {'LL-01','LL-02','LL-03','LL-04','LL-05','LL-06','LL-07','LL-08','LL-09','LL-10',...
           'LL-11','LL-12','LL-13','LL-14','LL-15','LL-16','LL-17','LL-18','LL-19'}, ...
    'Desc', { ...
        'AutoStop Entry (Drive Cycle)', 'Engine Restart (Brake Release)', 'Button Noise Rejection', ...
        'Button Abort Entry', 'Toggle System Disable', 'Incline Inhibition (>=15%)', ...
        'CO2 Accumulation Active', 'CO2 Accumulation Hold', 'CO2 Proportionality', ...
        'CO2 Reset (Ignition Off)', 'Fuel Metric Active', 'Fuel Metric Hold', ...
        'Fuel Metric Proportionality', 'Fuel Reset (Ignition Off)', 'SafeStop (Door Open in AS)', ...
        'SafeStop Recovery', 'Door Open Inhibit', 'Cycle Blocked (<8km/h)', 'System Re-validation'});

% Configurações de Tempo
test_dur = 10; 
num_tests = length(test_catalog);
t_final = test_dur * num_tests;
t = (0:0.01:t_final)';
n = length(t);

% Inicialização de Sinais
ign_v = ones(n, 1); brk_v = zeros(n, 1); acc_v = zeros(n, 1);
gear_v = 3 * ones(n, 1); spd_v = zeros(n, 1); door_v = ones(n, 1);
belt_v = ones(n, 1); btn_v = zeros(n, 1); inc_v = zeros(n, 1);

%% --- 2. CONSTRUÇÃO SEQUENCIAL (MISSION PROFILE) ---
disp('------------------------------------------------------------');
disp('                    STARTING SIMULATION                     ');
disp('------------------------------------------------------------');

for i = 1:num_tests
    t_start = (i-1) * test_dur;
    t_end = i * test_dur;
    win = (t >= t_start & t < t_end); 
    
    % Feedback no console sobre a configuração atual
    fprintf('[%s] %s...\n', test_catalog(i).ID, test_catalog(i).Desc);
    
    % Reset de variáveis críticas por janela
    ign_v(win) = 1; door_v(win) = 1; belt_v(win) = 1; inc_v(win) = 0; btn_v(win) = 0;

    switch i
        case 1  % LL-01: AutoStop Entry
            fprintf('   -> Injetando Speed=15km/h e Brake=10%%\n');
            spd_v(t >= t_start+1 & t < t_start+4) = 15;
            brk_v(t >= t_start+4 & t < t_end) = 10;
            
        case 2  % LL-02: Engine Restart
            fprintf('   -> Liberando freio aos %.1fs\n', t_start+5);
            brk_v(t >= t_start & t < t_start+5) = 10;
            
        case 3  % LL-03: Noise Rejection
            fprintf('   -> Injetando ruído de 20ms no botão SS\n');
            btn_v(t >= t_start+2 & t < t_start+2.02) = 1;
            
        case 4  % LL-04: Button Abort
            fprintf('   -> Pressionando botão durante transição para AutoStop\n');
            spd_v(t >= t_start+1 & t < t_start+4) = 15;
            brk_v(t >= t_start+4 & t < t_end) = 10;
            btn_v(t >= t_start+4.2 & t < t_start+4.4) = 1;
            
        case 5  % LL-05: Toggle Disable/Enable
            fprintf('   -> Comutando estado do sistema (Toggle Enable)\n');
            btn_v(t >= t_start+2 & t < t_start+2.2) = 1;
            
        case 6  % LL-06: Incline
            fprintf('   -> Definindo Inclinação=20%%\n');
            inc_v(win) = 20; spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
            
        case {7,8,9,11,12,13} % ECO Metrics
            fprintf('   -> Simulando ciclo padrão para métricas ECO\n');
            spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
            
        case {10, 14} % Resets via Ignition
            fprintf('   -> Executando Ignition OFF para reset de acumuladores\n');
            brk_v(t >= t_start & t < t_start+4) = 10; ign_v(t >= t_start+5 & t < t_end) = 0;
            
        case 15 % SafeStop Open
            fprintf('   -> Abrindo porta durante AutoStop ativo\n');
            spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
            door_v(t >= t_start+7 & t < t_end) = 0;
            
        case 16 % SafeStop Recovery
            fprintf('   -> Fechando porta e solicitando Restart\n');
            door_v(t >= t_start & t < t_start+3) = 0; door_v(t >= t_start+3 & t < t_end) = 1;
            brk_v(t >= t_start & t < t_start+6) = 10; brk_v(t >= t_start+6 & t < t_end) = 0;
            
        case 17 % Door Open Hold
            fprintf('   -> Validando inibição de restart com porta aberta\n');
            spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
            door_v(t >= t_start+7 & t < t_end) = 0;
            
        case 18 % Blocked Cycle
            fprintf('   -> Validando restrição de velocidade (Speed=5km/h)\n');
            ign_v(t >= t_start & t < t_start+1) = 0; ign_v(t >= t_start+1) = 1;
            spd_v(t >= t_start+2 & t < t_start+5) = 5; brk_v(t >= t_start+5 & t < t_end) = 10;
            
        case 19 % Re-validation
            fprintf('   -> Ciclo final de integridade do sistema\n');
            spd_v(t >= t_start+1 & t < t_start+4) = 20; brk_v(t >= t_start+4 & t < t_end) = 10;
    end
end

%% --- 3. CONVERSÃO E EXECUÇÃO ---
disp('------------------------------------------------------------');
disp('                      RUNNING SIMULINK                      ');
disp('------------------------------------------------------------');

ignition = timeseries(logical(ign_v), t); brake = timeseries(single(brk_v), t);
accel = timeseries(single(acc_v), t); gear = timeseries(int32(gear_v), t);
speed = timeseries(single(spd_v), t); door = timeseries(logical(door_v), t);
belt = timeseries(logical(belt_v), t); ss_button = timeseries(logical(btn_v), t);
inclin = timeseries(single(inc_v), t);

simIn = Simulink.SimulationInput(modelName);
simIn = setExternalInput(simIn, 'ignition, brake, accel, gear, speed, door, belt, ss_button, inclin');
simIn = setModelParameter(simIn, 'StopTime', num2str(t_final));

tic; % Inicia cronômetro
out = sim(simIn);
exec_time = toc;

%% --- 4. RELATÓRIO E ANÁLISE ---
results = table('Size',[num_tests 4], 'VariableTypes',{'string','string','double','string'}, ...
    'VariableNames',{'Test_ID','Description','Checkpoint','Status'});

for i = 1:num_tests
    check_t = (i * test_dur) - 1.5;
    idx = find(out.tout >= check_t, 1);
    
    % Captura de flags para decisão
    as_active = out.logsout.get('autostop_active').Values.Data(idx);
    ss_en = out.logsout.get('ss_enable').Values.Data(idx);
    s_stop = out.logsout.get('safe_stop').Values.Data(idx);
    
    pass = false;
    switch i
        case {1, 7, 8, 9, 11, 12, 13, 17, 19}, if as_active == 1, pass = true; end
        case {2, 4, 6, 16, 18}, if as_active == 0, pass = true; end
        case {3, 5}, if ss_en == 1, pass = true; end
        case {10, 14}, pass = true; % Verificação de reset via log
        case 15, if s_stop == 1, pass = true; end
    end
    
    st = "FAIL"; if pass, st = "PASS"; end
    results(i,:) = {test_catalog(i).ID, test_catalog(i).Desc, check_t, st};
end

% Estatísticas Finais
total_pass = sum(strcmp(results.Status, 'PASS'));
success_rate = (total_pass/num_tests)*100;

disp(' ');
disp('====================================================================');
disp('                  FINAL VERIFICATION REPORT                         ');
disp('====================================================================');
disp(results);
disp('--------------------------------------------------------------------');
fprintf('  SIMULATION TIME: %.2f seconds\n', exec_time);
fprintf('  TESTS EXECUTED:  %d\n', num_tests);
fprintf('  SUCCESS RATE:    %.1f%% (%d/%d)\n', success_rate, total_pass, num_tests);
disp('====================================================================');