function StartStopAppSIL()
    % STARTSTOPAPPSIL Cria e gerencia o dashboard interativo para o modelo SIL
    % do sistema Auto Start-Stop.
    
    %% 1. Configuração Inicial do Modelo
    modelName = 'tests_model';
    load_system(modelName);
    
    %% 2. Configuração da Janela Principal
    fig = uifigure('Name', 'Dashboard: Start/Stop System (Tests)', ...
                   'Position', [100, 50, 480, 880], ... 
                   'Color', '#F0F2F5', ...
                   'Scrollable', 'off'); 
    
    % Garante que a aplicação e a figura sejam encerradas corretamente
    fig.CloseRequestFcn = @(src, event) closeApp(src);
    
    
    %% 3. Construção dos Painéis de Interface
    
    % --- PAINEL: Controle de Simulação ---
    pnlSim = uipanel(fig, 'Position', [20, 800, 440, 60], 'Title', 'Simulation Control', ...
        'BackgroundColor', 'white', 'ForegroundColor', 'black', 'FontWeight', 'bold');
        
    btnStart = uibutton(pnlSim, 'Position', [100, 5, 110, 30], ...
        'Text', '▶ Start', 'BackgroundColor', '#4CAF50', 'FontColor', 'white', 'FontWeight', 'bold', ...
        'ButtonPushedFcn', @(btn,event) startSimulation());
        
    btnStop = uibutton(pnlSim, 'Position', [230, 5, 110, 30], ...
        'Text', '■ Stop', 'BackgroundColor', '#F44336', 'FontColor', 'white', 'FontWeight', 'bold', ...
        'ButtonPushedFcn', @(btn,event) stopSimulation());
        
        
    % --- PAINEL: Controles do Motorista ---
    pnlDriver = uipanel(fig, 'Position', [20, 380, 440, 400], 'Title', 'Driver Controls', ...
        'BackgroundColor', 'white', 'ForegroundColor', 'black', 'FontWeight', 'bold');
    
    % Ignição
    uilabel(pnlDriver, 'Position', [60, 340, 100, 22], 'Text', 'Ignition:', 'FontColor', 'black');
    swIgnition = uiswitch(pnlDriver, 'Position', [60, 315, 45, 20], 'Items', {'Off', 'On'}, ...
        'FontColor', 'black', ...
        'ValueChangedFcn', @(sw,event) updateSimulink(modelName, 'ignition_on', sw.Value));
        
    % Botão Start/Stop
    uilabel(pnlDriver, 'Position', [260, 340, 120, 22], 'Text', 'SS Button:', 'FontColor', 'black');
    btnSSEnable = uibutton(pnlDriver, 'Position', [260, 310, 100, 30], ...
        'Text', 'Push (Pulse)', 'FontColor', 'white', 'FontWeight', 'bold', 'BackgroundColor', '#333333', ...
        'ButtonPushedFcn', @(btn,event) pulseButton(modelName, 'button_input'));
        
    % Pedal de Freio
    uilabel(pnlDriver, 'Position', [40, 270, 150, 22], 'Text', 'Brake Position (%):', 'FontColor', 'black');
    brkValueLabel = uilabel(pnlDriver, 'Position', [200, 270, 50, 22], 'Text', '0', 'FontWeight', 'bold', 'FontColor', '#D95319'); 
    sldBrake = uislider(pnlDriver, 'Position', [40, 230, 360, 3], 'Limits', [0 100], ...
        'FontColor', 'black', ...
        'ValueChangedFcn', @(sld,event) updateSlider(modelName, 'brake_position', sld.Value, brkValueLabel));
    
    % Pedal do Acelerador
    uilabel(pnlDriver, 'Position', [40, 170, 150, 22], 'Text', 'Accelerator Position (%):', 'FontColor', 'black');
    acValueLabel = uilabel(pnlDriver, 'Position', [200, 170, 50, 22], 'Text', '0', 'FontWeight', 'bold', 'FontColor', '#D95319'); 
    sldAC = uislider(pnlDriver, 'Position', [40, 130, 360, 3], 'Limits', [0 100], ...
        'FontColor', 'black', ...
        'ValueChangedFcn', @(sld,event) updateSlider(modelName, 'ac_position', sld.Value, acValueLabel));
        
    % Seletor de Marcha
    uilabel(pnlDriver, 'Position', [40, 70, 150, 22], 'Text', 'Gear Position:', 'FontColor', 'black');
    sldGear = uislider(pnlDriver, 'Position', [40, 30, 360, 3], ...
        'Limits', [0 3], 'MajorTicks', [0 1 2 3], 'MajorTickLabels', {'P', 'R', 'N', 'D'}, 'MinorTicks', [], ...
        'FontColor', 'black', ...
        'ValueChangedFcn', @(sld,event) updateGear(modelName, 'gear_position', sld));
    
        
    % --- PAINEL: Dinâmica do Veículo ---
    pnlDynamics = uipanel(fig, 'Position', [20, 150, 440, 210], 'Title', 'Vehicle Dynamics', ...
        'BackgroundColor', 'white', 'ForegroundColor', 'black', 'FontWeight', 'bold');
    
    % Velocidade do Veículo
    uilabel(pnlDynamics, 'Position', [40, 150, 150, 22], 'Text', 'Vehicle Speed (km/h):', 'FontColor', 'black');
    spdValueLabel = uilabel(pnlDynamics, 'Position', [200, 150, 50, 22], 'Text', '0', 'FontWeight', 'bold', 'FontColor', '#0072BD'); 
    sldSpeed = uislider(pnlDynamics, 'Position', [40, 130, 360, 3], 'Limits', [0 120], ...
        'FontColor', 'black', ...
        'ValueChangedFcn', @(sld,event) updateSlider(modelName, 'v_speed', sld.Value, spdValueLabel));
    
    % Ângulo de Inclinação
    uilabel(pnlDynamics, 'Position', [40, 70, 150, 22], 'Text', 'Inclination Angle:', 'FontColor', 'black');
    incValueLabel = uilabel(pnlDynamics, 'Position', [200, 70, 50, 22], 'Text', '0', 'FontWeight', 'bold', 'FontColor', '#D95319'); 
    sldInc = uislider(pnlDynamics, 'Position', [40, 30, 360, 3], 'Limits', [0 100], ...
        'FontColor', 'black', ...
        'ValueChangedFcn', @(sld,event) updateSlider(modelName, 'inclination', sld.Value, incValueLabel));
    
        
    % --- PAINEL: Sensores de Segurança ---
    pnlSafety = uipanel(fig, 'Position', [20, 20, 440, 110], 'Title', 'Safety Sensors', ...
        'BackgroundColor', 'white', 'ForegroundColor', 'black', 'FontWeight', 'bold');
    
    % Status da Porta
    uilabel(pnlSafety, 'Position', [40, 65, 100, 22], 'Text', 'Door Status:', 'FontColor', 'black');
    swDoor = uiswitch(pnlSafety, 'Position', [40, 30, 60, 20], 'Items', {'Open', 'Closed'}, ...
        'FontColor', 'black', ...
        'ValueChangedFcn', @(sw,event) updateSimulink(modelName, 'Door_closed', sw.Value));
        
    % Status do Cinto de Segurança
    uilabel(pnlSafety, 'Position', [250, 65, 100, 22], 'Text', 'Seatbelt:', 'FontColor', 'black');
    swSeatbelt = uiswitch(pnlSafety, 'Position', [250, 30, 80, 20], 'Items', {'Unfastened', 'Fastened'}, ...
        'FontColor', 'black', ...
        'ValueChangedFcn', @(sw,event) updateSimulink(modelName, 'seatbelt_fastened', sw.Value));
    
        
    %% 4. Funções de Callback e Controle
    
    function startSimulation()
        % Sincroniza os valores atuais da interface com os blocos do Simulink
        updateSimulink(modelName, 'ignition_on', swIgnition.Value);
        updateSimulink(modelName, 'button_input', '0'); % Garante estado inicial não pressionado
        updateSimulink(modelName, 'brake_position', sldBrake.Value);
        updateSimulink(modelName, 'ac_position', sldAC.Value); 
        updateGear(modelName, 'gear_position', sldGear); 
        updateSimulink(modelName, 'v_speed', sldSpeed.Value);
        updateSimulink(modelName, 'inclination', sldInc.Value); 
        updateSimulink(modelName, 'Door_closed', swDoor.Value);
        updateSimulink(modelName, 'seatbelt_fastened', swSeatbelt.Value);
        
        set_param(modelName, 'SimulationCommand', 'start');
    end
    
    function stopSimulation()
        set_param(modelName, 'SimulationCommand', 'stop');
    end
    
    function pulseButton(mdl, blockName)
        % Simula o comportamento físico de um botão momentâneo (300ms)
        updateSimulink(mdl, blockName, '1'); 
        pause(0.3);                          
        updateSimulink(mdl, blockName, '0'); 
    end
    
    function updateGear(mdl, blockName, sld)
        % Arredonda e trava visualmente o slider da marcha em valores discretos (0 a 3)
        discreteValue = round(sld.Value);
        sld.Value = discreteValue; 
        
        fullPath = sprintf('%s/%s', mdl, blockName);
        try 
            set_param(fullPath, 'Value', num2str(discreteValue)); 
        catch E
            fprintf('Aviso - Erro ao atualizar marcha: %s\n', E.message);
        end
    end
    
    function updateSimulink(mdl, blockName, value)
        % Converte estados textuais da UI para valores binários/numéricos do Simulink
        fullPath = sprintf('%s/%s', mdl, blockName);
        
        if strcmp(value, 'On') || strcmp(value, 'Closed') || strcmp(value, 'Fastened')
            simValue = '1';
        elseif strcmp(value, 'Off') || strcmp(value, 'Open') || strcmp(value, 'Unfastened')
            simValue = '0';
        else
            simValue = num2str(value);
        end
        
        try 
            set_param(fullPath, 'Value', simValue); 
        catch E
            fprintf('Aviso - Erro no bloco [%s]: %s\n', fullPath, E.message);
        end
    end
    
    function updateSlider(mdl, blockName, value, labelHandle)
        % Atualiza a label do slider e envia o novo valor ao modelo
        labelHandle.Text = num2str(round(value, 2));
        updateSimulink(mdl, blockName, value);
    end
    
    function closeApp(figHandle)
        % Limpa a figura da memória ao fechar
        delete(figHandle);
    end
end