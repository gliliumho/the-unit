import socket

traffic_data = """<msg><?xml version="1.0" encoding="utf-8"?>
<TrafficStatisticMessage>
<MessageID>20131025103419335707</MessageID>
<MessageType>TrafficStatistic</MessageType>
<SenderIP>127.0.0.1</SenderIP>
<CreatedDateTime>2013-10-25T10:34:19.335707</CreatedDateTime>
<TrafficStatistic>
<VIDSName>TDC_002</VIDSName>
<VIDSLocation>VIDSLocation</VIDSLocation>
<CameraPreset>1</CameraPreset>
<GroupID>1_N</GroupID>
<TrafficDateTime>2013-10-25T10:34:19.335707</TrafficDateTime>
<DataRetrievalEndDateTime>2013-10-25T10:34:19.335707</DataRetrievalEndDateTime>
<DurationInMinutes>1</DurationInMinutes>
<CountTotal>6</CountTotal>
<CountClassA>1</CountClassA>
<CountClassB>1</CountClassB>
<CountClassC>1</CountClassC>
<CountClassD>1</CountClassD>
<CountClassE>1</CountClassE>
<CountClassF>1</CountClassF>
<AverageSpeed>231</AverageSpeed>
<AverageTimeOccupancy>1.21</AverageTimeOccupancy>
<AverageFlowRate>3221</AverageFlowRate>
<AverageTimeHeadway>1.45</AverageTimeHeadway>
<LevelOfService>F</LevelOfService>
<CongestionLevel>2</CongestionLevel>
<AverageSpaceOccupancy>0.5600000000000001</AverageSpaceOccupancy>
<Density>0.474</Density>
<SpaceMeanSpeed>135</SpaceMeanSpeed>
</TrafficStatistic>
</TrafficStatisticMessage>
</msg>"""


s = socket.socket()
host = socket.gethostbyname(socket.getfqdn())
port = 20203
s.bind((host, port))
s.listen(5)

data = bytes(traffic_data, 'utf-8')

while True:
    c, addr = s.accept()
    print("Got connection from " + str(addr))
    c.send(data)
    c.close()
