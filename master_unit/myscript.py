#!/bin/python3

#This script is used to create the idlist.txt
idfile = open("idlist.txt",'w')
idfile.write("# Sample idlist\n")
idfile.write("# '#' can be used to comment out the lines\n")
idfile.write("# IDs will be <groupID>.<uniqueID>\n\n")

for i in range(1,11):
	for j in range(1,101):
		idfile.write(str(i)+'.'+str(j)+'\n')
idfile.close()
