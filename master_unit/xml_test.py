#!/bin/python3

# Written to test XML parsing in python
import xml.etree.ElementTree as ET

f = open("traffic_data.xml", "r", encoding='utf-8')
f_string = f.read(None)

# tree = ET.parse('traffic_data.xml')
# root = tree.getroot()
# traffic_data = f_string[5:len(f_string)-6]

root = ET.fromstring(f_string)

values = root.iter()
# print(values.keys())
# print('='*5)
# if len(value_iter.__iter__()) == 0:
#     print("None")
# else:
# first = next(values.__iter__(), None)
# if first:
#     print("There's something")
for value in values:
    if value.tag == "IncidentType" or value.tag == "CongestionLevel":
        print(value.tag, end=" \t")
        print(value.text)


    # try:
    #     value = next(value)
    # except:
    #     break


#if 'TrafficStatisticMessa' in values then something something
# print(values)
