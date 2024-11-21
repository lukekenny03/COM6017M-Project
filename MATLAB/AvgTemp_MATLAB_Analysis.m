ChannelID = ; 
TemperatureField = 1; 

readAPIKey = ''; 

Temperature = thingSpeakRead(ChannelID,'Fields',TemperatureField,'ReadKey',readAPIKey); 

avgTemperature = mean(Temperature); 
display(avgTemperature,'Average Temperature'); 
 
WriteChannel = ;
writeAPIKey = '';
thingSpeakWrite(WriteChannel,'Fields',[1],'Values',avgTemperature,'WriteKey',writeAPIKey);