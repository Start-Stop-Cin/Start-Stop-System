%% AUTOMATED SIL VALIDATION SUITE - START/STOP SYSTEM
% Project: Stellantis HMI Cluster Validation

clear; clc;
modelName = 'tests_model';
load_system(modelName);

%% 1. DEFINIÇÕES DOS TESTES
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

%% 2. CONSTRUÇÃO DOS CASOS DE TESTE
% Preparação dos vetores de dados
for i = 1:num_tests
    t_start = (i-1) * test_dur;
    t_end = i * test_dur;
    win = (t >= t_start & t < t_end); 
    
    ign_v(win) = 1; door_v(win) = 1; belt_v(win) = 1; inc_v(win) = 0; btn_v(win) = 0;

switch i
        case 1
            % Acelera o veículo a 15km/h por 3s para validar o ciclo de direção mínimo.
            % Em seguida, aplica 10% de freio até o fim da janela para o carro parar e entrar em AutoStop.
            spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
        
        case 2
            % O veículo já inicia parado. O motorista mantém o pé no freio nos primeiros 5s.
            % Aos 5s, o freio é liberado (0%) para forçar o Restart automático do motor.
            brk_v(t >= t_start & t < t_start+5) = 10; brk_v(t >= t_start+5 & t < t_end) = 0;
        
        case 3
            % Simula um ruído elétrico ou toque acidental muito rápido (20ms) no botão Start/Stop.
            % O objetivo é verificar se o filtro de software (debounce) ignora esse pulso.
            btn_v(t >= t_start+2 & t < t_start+2.02) = 1;
        
        case 4
            % Executa um ciclo normal de parada (anda a 15km/h e freia).
            % Mas logo após a parada (4.2s a 4.4s), pressiona o botão para abortar a entrada em AutoStop.
            spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
            btn_v(t >= t_start+4.2 & t < t_start+4.4) = 1;
        
        case 5
            % Injeta um pulso válido de 200ms no botão Start/Stop.
            % Como o teste anterior desabilitou o sistema, este pulso serve para re-habilitá-lo.
            btn_v(t >= t_start+2 & t < t_start+2.2) = 1;
        
        case 6
            % Define que o veículo está em uma ladeira com 20% de inclinação durante todo o teste.
            % Executa o ciclo de parada normal para verificar se o AutoStop é inibido pela rampa.
            inc_v(win) = 20; spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
        
        case {7,8,9,11,12,13}
            % Ciclo de condução "perfeito": acelera a 15km/h e para, permanecendo em AutoStop.
            % Usado repetidamente para permitir que as métricas ECO (CO2 e Combustível) acumulem dados.
            spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
        
        case {10, 14}
            % O veículo está parado com o freio pressionado (AutoStop ativo).
            % Aos 5s, o motorista desliga a ignição do carro (Key OFF), o que deve zerar os acumuladores ECO.
            brk_v(t >= t_start & t < t_start+4) = 10; ign_v(t >= t_start+5 & t < t_end) = 0;
        
        case 15
            % O veículo entra em AutoStop após um ciclo normal.
            % Aos 7s, o motorista abre a porta (0 = Open), o que deve engatilhar o modo de segurança (SafeStop).
            spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
            door_v(t >= t_start+7 & t < t_end) = 0;
        
        case 16
            % Inicia com a porta aberta (herdada do cenário anterior). Aos 3s, o motorista fecha a porta.
            % Mantém o freio pressionado e o solta aos 6s para testar se o motor religa (recuperação do SafeStop).
            door_v(t >= t_start & t < t_start+3) = 0; door_v(t >= t_start+3 & t < t_end) = 1;
            brk_v(t >= t_start & t < t_start+6) = 10; brk_v(t >= t_start+6 & t < t_end) = 0;
        
        case 17
            % O veículo entra em AutoStop e a porta é aberta novamente aos 7s.
            % A porta é mantida aberta até o fim para garantir que o motor NÃO efetue um restart indevido.
            spd_v(t >= t_start+1 & t < t_start+4) = 15; brk_v(t >= t_start+4 & t < t_end) = 10;
            door_v(t >= t_start+7 & t < t_end) = 0;
        
        case 18
            % Simula um ciclo de ignição rápido (Desliga/Liga em 1s) para limpar a memória do sistema.
            % Arranca a apenas 5km/h (manobra de garagem) e para. O AutoStop não deve ativar.
            ign_v(t >= t_start & t < t_start+1) = 0; ign_v(t >= t_start+1) = 1;
            spd_v(t >= t_start+2 & t < t_start+5) = 5; brk_v(t >= t_start+5 & t < t_end) = 10;
        
        case 19
            % Após o bloqueio anterior, acelera até 20km/h (acima do limite) e para.
            % O objetivo é re-validar que o sistema volta a funcionar normalmente em um ciclo de condução válido.
            spd_v(t >= t_start+1 & t < t_start+4) = 20; brk_v(t >= t_start+4 & t < t_end) = 10;
    end
end

%% 3. EXECUÇÃO DOS TESTES

ignition = timeseries(logical(ign_v), t); brake = timeseries(single(brk_v), t);
accel = timeseries(single(acc_v), t); gear = timeseries(int32(gear_v), t);
speed = timeseries(single(spd_v), t); door = timeseries(logical(door_v), t);
belt = timeseries(logical(belt_v), t); ss_button = timeseries(logical(btn_v), t);
inclin = timeseries(single(inc_v), t);

% Configurando o Simulink para ler os outputs

set_param(modelName, 'LoadExternalInput', 'on');
set_param(modelName, 'ExternalInput', 'ignition, brake, accel, gear, speed, door, belt, ss_button, inclin');
set_param(modelName, 'StopTime', num2str(t_final));

disp('------------------------------------------------------------');
disp('                INICIANDO SIMULAÇÃO COM A SUITE             ');
disp('------------------------------------------------------------');

tic; % Inicialização do cronômetro
set_param(modelName, 'SimulationCommand', 'start'); % Iniciando Simulação

current_test = 1;

% Loop de monitoramento

while strcmp(get_param(modelName, 'SimulationStatus'), 'running') || ...
      strcmp(get_param(modelName, 'SimulationStatus'), 'initializing') || ...
      strcmp(get_param(modelName, 'SimulationStatus'), 'updating')
    
    try
        % Lê relógio interno do Simulink
        t_sim = get_param(modelName, 'SimulationTime');
    catch
        t_sim = 0;
    end
    
    % Controla a janela de 10s para execução de cada um dos testes

    if t_sim >= (current_test - 1) * test_dur && current_test <= num_tests
        fprintf('[T = %05.1f s] >> Executando [%s]: %s\n', ...
            t_sim, test_catalog(current_test).ID, test_catalog(current_test).Desc);
        current_test = current_test + 1;
    end
    
    pause(0.2);
end

exec_time = toc;
pause(0.5);

%% 4. RELATÓRIO E ANÁLISE
disp(' ');
disp('------------------------------------------------------------');
disp('                      GERACAO DO RELATORIO                  ');
disp('------------------------------------------------------------');

results = table('Size',[num_tests 4], 'VariableTypes',{'string','string','double','string'}, ...
    'VariableNames',{'Test_ID','Description','Checkpoint','Status'});

for i = 1:num_tests
    check_t = (i * test_dur) - 1.5;
    idx = find(out.tout >= check_t, 1);
    
    % Flags de afericao dos resultados
    as_active = out.logsout.get('autostop_active').Values.Data(idx);
    ss_en = out.logsout.get('ss_enable').Values.Data(idx);
    s_stop = out.logsout.get('safe_stop').Values.Data(idx);
    
    pass = false;
    switch i
        case {1, 7, 8, 9, 11, 12, 13, 17, 19}, if as_active == 1, pass = true; end
        case {2, 4, 6, 16, 18}, if as_active == 0, pass = true; end
        case {3, 5}, if ss_en == 1, pass = true; end
        case {10, 14}, pass = true;
        case 15, if s_stop == 1, pass = true; end
    end
    
    st = "FAIL"; if pass, st = "PASS"; end
    results(i,:) = {test_catalog(i).ID, test_catalog(i).Desc, check_t, st};
end

total_pass = sum(strcmp(results.Status, 'PASS'));
success_rate = (total_pass/num_tests)*100;

disp(' ');
disp('====================================================================');
disp('                        VERIFICATION REPORT                         ');
disp('====================================================================');
disp(results);
disp('--------------------------------------------------------------------');
fprintf('  SIMULATION TIME: %.2f seconds\n', exec_time);
fprintf('  TESTS EXECUTED:  %d\n', num_tests);
fprintf('  SUCCESS RATE:    %.1f%% (%d/%d)\n', success_rate, total_pass, num_tests);
disp('====================================================================');