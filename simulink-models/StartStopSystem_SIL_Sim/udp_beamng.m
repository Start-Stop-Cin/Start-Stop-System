function [speed, rpm, throttle, brake, clutch, pitch] = udp_beamng()
%#codegen

persistent u last_pitch

if isempty(u)
    u = udpport("datagram","IPV4","LocalPort",4444);
    last_pitch = 0;
end

speed = 0;
rpm = 0;
throttle = 0;
brake = 0;
clutch = 0;
pitch = last_pitch;

while u.NumDatagramsAvailable > 0
    
    data = read(u,1,"uint8");
    bytes = data.Data;
    len = numel(bytes);

    % =========================
    % OUTGAUGE (96 bytes)
    % =========================
    if len == 96
        
        % Byte offsets (MATLAB is 1-based)
        speed    = typecast(uint8(bytes(13:16)),'single') * 3.6;
        rpm      = typecast(uint8(bytes(17:20)),'single');
        throttle = typecast(uint8(bytes(45:48)),'single');
        brake    = typecast(uint8(bytes(49:52)),'single');
        clutch   = typecast(uint8(bytes(53:56)),'single');

    % =========================
    % BNG1 (88 bytes)
    % =========================
    elseif len == 88
        
        tag = char(bytes(1:4));
        
        if isequal(tag','BNG1')
            
            % pitchPos index (correct)
            pitch_rad = typecast(uint8(bytes(53:56)),'single');
            pitch = double(pitch_rad) * 180/pi;
            
            last_pitch = pitch;
        end
    end
end