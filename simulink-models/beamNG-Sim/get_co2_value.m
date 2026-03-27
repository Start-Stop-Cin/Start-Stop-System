function val = get_co2_value()

val = 0;

try
    data = webread('http://127.0.0.1:23512/');

    if isfield(data, 'instantCO2') && isfield(data.instantCO2, 'value')
        val = data.instantCO2.value;
    end

catch
    val = 0;
end