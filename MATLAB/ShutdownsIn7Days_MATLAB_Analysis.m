ChannelID = ; 
ShutdownField = 4; 

readAPIKey = ''; 

Shutdowns = thingSpeakRead(ChannelID,'Fields',ShutdownField,'NumMinutes',10080,'ReadKey',readAPIKey); 

TotalShutdowns= sum(Shutdowns,'omitnan'); 
display(TotalShutdowns,'Shutdowns'); 
 
WriteChannel = ;
writeAPIKey = '';
thingSpeakWrite(WriteChannel,'Fields',[5],'Values',TotalShutdowns,'WriteKey',writeAPIKey);