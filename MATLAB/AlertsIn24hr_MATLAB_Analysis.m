ChannelID = ; 
AlertField = 2; 

readAPIKey = ''; 

Alerts = thingSpeakRead(ChannelID,'Fields',AlertField,'NumMinutes',1440,'ReadKey',readAPIKey); 

TotalAlerts= sum(Alerts,'omitnan'); 
display(TotalAlerts,'Alerts'); 
 
WriteChannel = ;
writeAPIKey = '';
thingSpeakWrite(WriteChannel,'Fields',[2],'Values',TotalAlerts,'WriteKey',writeAPIKey);