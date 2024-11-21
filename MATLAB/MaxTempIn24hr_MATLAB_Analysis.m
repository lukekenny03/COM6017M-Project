ChannelID = ; 
TemperatureField = 1; 

readAPIKey = ''; 

Temperature = thingSpeakRead(ChannelID,'Fields',TemperatureField,'NumMinutes',1440,'ReadKey',readAPIKey); 

MaxTemperature = max(Temperature); 
display(MaxTemperature,'Max Temperature'); 

WriteChannel = ;
writeAPIKey = '';
Empty = isempty(MaxTemperature)

if Empty
    thingSpeakWrite(WriteChannel,'Fields',[4],'Values',0,'WriteKey',writeAPIKey);
else
    thingSpeakWrite(WriteChannel,'Fields',[4],'Values',MaxTemperature,'WriteKey',writeAPIKey);
end
