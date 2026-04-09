function StartStopApp()
    % 1. Initial Configuration
    modelName = 'model'; 
    load_system(modelName);
    
    % 2. Window Creation
    fig = uifigure('Name', 'Dashboard: Auto Start-Stop', ...
                   'Position', [100, 50, 480, 750], ...
                   'Color', '#F0F2F5', ...
                   'Scrollable', 'on'); 
    
    % Configure safe window closing
    fig.CloseRequestFcn = @(src, event) closeApp(src);
    
    % =========================================================================
    % 3. INTERFACE PANELS 
    % =========================================================================
    
    % --- PANEL 1: Simulation Control ---
    pnlSim = uipanel(fig, 'Position', [20, 650, 440, 80], 'Title', 'Simulation Control', 'BackgroundColor', 'white', 'FontWeight', 'bold');
        
    btnStart = uibutton(pnlSim, 'Position', [100, 15, 110, 35], 'Text', '▶ Start', ...
        'BackgroundColor', '#4CAF50', 'FontColor', 'white', 'FontWeight', 'bold', ...
        'ButtonPushedFcn', @(btn,event) startSimulation());
        
    btnStop = uibutton(pnlSim, 'Position', [230, 15, 110, 35], 'Text', '■ Stop', ...
        'BackgroundColor', '#F44336', 'FontColor', 'white', 'FontWeight', 'bold', ...
        'ButtonPushedFcn', @(btn,event) stopSimulation());
        
    % --- PANEL 2: Driver Controls ---
    pnlDriver = uipanel(fig, 'Position', [20, 370, 440, 260], 'Title', 'Driver Controls', 'BackgroundColor', 'white', 'FontWeight', 'bold');
    
    % Line 1: System (Ignition and SS Button)
    uilabel(pnlDriver, 'Position', [60, 210, 100, 22], 'Text', 'Ignition:');
    swIgnition = uiswitch(pnlDriver, 'Position', [60, 180, 45, 20], 'Items', {'Off', 'On'}, 'ValueChangedFcn', @(sw,event) updateSimulink(modelName, 'IgnitionStatus', sw.Value));
        
    uilabel(pnlDriver, 'Position', [260, 210, 120, 22], 'Text', 'SS Button:');
    btnDash = uibutton(pnlDriver, 'Position', [260, 178, 110, 25], 'Text', 'Push (Pulse)', ...
        'FontWeight', 'bold', 'ButtonPushedFcn', @(btn,event) pulseButton(modelName, 'ButtonInput'));
        
    % Line 2: Pedals (Brake and Accelerator)
    uilabel(pnlDriver, 'Position', [60, 135, 100, 22], 'Text', 'Brake Pedal:');
    swBrake = uiswitch(pnlDriver, 'Position', [60, 105, 45, 20], 'Items', {'Off', 'On'}, 'ValueChangedFcn', @(sw,event) updateSimulink(modelName, 'BrakeStatus', sw.Value));
    
    uilabel(pnlDriver, 'Position', [260, 135, 120, 22], 'Text', 'Accelerator:');
    swAccel = uiswitch(pnlDriver, 'Position', [260, 105, 45, 20], 'Items', {'Off', 'On'}, 'ValueChangedFcn', @(sw,event) updateSimulink(modelName, 'AcceleratorStatus', sw.Value));
        
    % Line 3: Gear Position
    uilabel(pnlDriver, 'Position', [40, 60, 100, 22], 'Text', 'Gear Position:');
    sldGear = uislider(pnlDriver, 'Position', [40, 35, 360, 3], 'Limits', [0 3], 'MajorTicks', [0 1 2 3], 'MajorTickLabels', {'P', 'N', 'D', 'R'}, 'MinorTicks', [], 'ValueChangedFcn', @(sld,event) updateGear(modelName, 'GearPosition', sld));
    
    % --- PANEL 3: Vehicle Dynamics ---
    pnlDynamics = uipanel(fig, 'Position', [20, 130, 440, 220], 'Title', 'Vehicle Dynamics', 'BackgroundColor', 'white', 'FontWeight', 'bold');
    
    uilabel(pnlDynamics, 'Position', [40, 165, 150, 22], 'Text', 'Vehicle Speed (km/h):');
    spdValueLabel = uilabel(pnlDynamics, 'Position', [175, 165, 50, 22], 'Text', '0', 'FontWeight', 'bold', 'FontColor', '#0072BD'); 
    sldSpeed = uislider(pnlDynamics, 'Position', [40, 140, 360, 3], 'Limits', [0 120], 'ValueChangedFcn', @(sld,event) updateSlider(modelName, 'VehicleSpeed', sld.Value, spdValueLabel));
    
    uilabel(pnlDynamics, 'Position', [40, 85, 150, 22], 'Text', 'Inclination Angle (°):');
    incValueLabel = uilabel(pnlDynamics, 'Position', [175, 85, 50, 22], 'Text', '0', 'FontWeight', 'bold', 'FontColor', '#D95319'); 
    sldInc = uislider(pnlDynamics, 'Position', [40, 60, 360, 3], 'Limits', [-1 1], 'ValueChangedFcn', @(sld,event) updateSlider(modelName, 'InclinationAngle', sld.Value, incValueLabel));
    
    % --- PANEL 4: Safety Sensors ---
    pnlSafety = uipanel(fig, 'Position', [20, 20, 440, 90], 'Title', 'Safety Sensors', 'BackgroundColor', 'white', 'FontWeight', 'bold');
    
    uilabel(pnlSafety, 'Position', [40, 45, 100, 22], 'Text', 'Door Status:');
    swDoor = uiswitch(pnlSafety, 'Position', [40, 20, 60, 20], 'Items', {'Open', 'Closed'}, 'ValueChangedFcn', @(sw,event) updateSimulink(modelName, 'DoorStatus', sw.Value));
        
    uilabel(pnlSafety, 'Position', [250, 45, 100, 22], 'Text', 'Seatbelt:');
    swSeatbelt = uiswitch(pnlSafety, 'Position', [250, 20, 80, 20], 'Items', {'Unfastened', 'Fastened'}, 'ValueChangedFcn', @(sw,event) updateSimulink(modelName, 'SeatbeltStatus', sw.Value));
    
    % =========================================================================
    % 4. CALLBACK FUNCTIONS
    % =========================================================================
    
    function startSimulation()
        % Initial Synchronization 
        updateSimulink(modelName, 'IgnitionStatus', swIgnition.Value);
        updateSimulink(modelName, 'BrakeStatus', swBrake.Value);
        updateSimulink(modelName, 'AcceleratorStatus', swAccel.Value); 
        updateSimulink(modelName, 'ButtonInput', '0'); 
        updateSimulink(modelName, 'GearPosition', sldGear.Value);
        updateSimulink(modelName, 'VehicleSpeed', sldSpeed.Value);
        updateSimulink(modelName, 'InclinationAngle', sldInc.Value);
        updateSimulink(modelName, 'DoorStatus', swDoor.Value);
        updateSimulink(modelName, 'SeatbeltStatus', swSeatbelt.Value);
        
        % Start simulation
        set_param(modelName, 'SimulationCommand', 'start');
    end
    
    function stopSimulation()
        set_param(modelName, 'SimulationCommand', 'stop');
    end
    
    function pulseButton(mdl, blockName)
        updateSimulink(mdl, blockName, '1'); 
        pause(0.3);                          
        updateSimulink(mdl, blockName, '0'); 
    end
    
    function updateGear(mdl, blockName, sld)
        discreteValue = round(sld.Value);
        sld.Value = discreteValue;
        updateSimulink(mdl, blockName, discreteValue);
    end
    
    function updateSimulink(mdl, blockName, value)
        fullPath = sprintf('%s/%s', mdl, blockName);
        
        % Removido o 'Pulled' e 'Released' da lógica de conversão
        if strcmp(value, 'On') || strcmp(value, 'Closed') || strcmp(value, 'Fastened')
            simValue = '1';
        elseif strcmp(value, 'Off') || strcmp(value, 'Open') || strcmp(value, 'Unfastened')
            simValue = '0';
        else
            simValue = num2str(value);
        end
        
        try set_param(fullPath, 'Value', simValue); catch, end
    end
    
    function updateSlider(mdl, blockName, value, labelHandle)
        labelHandle.Text = num2str(round(value, 2));
        updateSimulink(mdl, blockName, value);
    end
    
    function closeApp(figHandle)
        delete(figHandle);
    end
end