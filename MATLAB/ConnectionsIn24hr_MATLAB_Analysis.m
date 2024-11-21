ChannelID = ; 
ConnectionField = 3; 

readAPIKey = ''; 

Connections = thingSpeakRead(ChannelID,'Fields',ConnectionField,'NumMinutes',1440,'ReadKey',readAPIKey); 

TotalConnections= sum(Connections,'omitnan'); 
display(TotalConnections,'Connections'); 
 
WriteChannel = ;
writeAPIKey = '';
thingSpeakWrite(WriteChannel,'Fields',[3],'Values',TotalConnections,'WriteKey',writeAPIKey);