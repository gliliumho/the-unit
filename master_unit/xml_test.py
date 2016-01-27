#!/bin/python3

# Written to test XML parsing in python
import xml.etree.ElementTree as ET

f = open("traffic_data.xml", "r", encoding='utf-8')
f_string = f.read(None)

# tree = ET.parse('traffic_data.xml')
# root = tree.getroot()
traffic_data = f_string[5:len(f_string)-6]

root = ET.fromstring(traffic_data)

values = root.find('TrafficStatistic/CongestionLevel').text

print(values)
