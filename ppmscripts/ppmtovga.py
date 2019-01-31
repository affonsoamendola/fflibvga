#!/usr/bin/python

#ARGUMENTS:
#python ppmtovga.py [filein (with extension, expecting a .ppm file, with no comments)] ...
#					[start index of allocated pallette space] ...
#					[end index of allocated pallette space] ...
#					[fileout (without extension, will generate 2 files, fileout.pgm a version of the .ppm file but with the indexes of the colors, and a .plt file, with the pallette information)]

import sys

def read_uint8(currentLine, posInLine, separator, returnEndPosition):

	out = 0
	done = False
	middleOfNumber = False
	j = posInLine

	while(not(done)):
		currentChar = currentLine[j]
		if((currentChar != '0' or middleOfNumber == True) and currentChar != separator):
			out = out*10 + (int(currentChar))
			middleOfNumber = True	
		elif(currentChar == '0' or currentChar == separator):
			done = True
		j = j+1

	if(returnEndPosition == True):
		return out, j
	else:
		return out

filename = sys.argv[1]

startIndex = int(sys.argv[2])
endIndex = int(sys.argv[3])

fileout = sys.argv[4]

f = open(filename,"r")
foutpgm = open(fileout + ".pgm", "w")
foutplt = open(fileout + ".plt", "w")

foutpgm.write("P2\n")

filetype = f.readline()[1]

if(filetype != '3'):
	print("UNKNOWN FORMAT")
	exit()

currentLine = f.readline()

sizeX = 0
sizeY = 0
i = 0

middleOfNumber = False
done = False

while(not(done)):
	currentChar = currentLine[i]

	if((currentChar != '0' or middleOfNumber == True) and currentChar != ' '):
		sizeX = sizeX*10 + (int(currentChar))
		middleOfNumber = True	
	elif(currentChar == ' '):
		done = True
	i = i+1

middleOfNumber = False
done = False
while(not(done)):
	currentChar = currentLine[i]

	if((currentChar != '0' or middleOfNumber == True) and currentChar != '\n'):
		sizeY = sizeY*10 + (int(currentChar))
		middleOfNumber = True	
	elif(currentChar == '\n'):
		done = True
	i = i+1

f.readline()

foutpgm.write(str(sizeX)+' '+ str(sizeY)+'\n')
foutpgm.write("255\n")

r = 0
g = 0
b = 0

i = 0
j = 0

middleOfNumber = False
done = False

currentIndex = startIndex;

pallette = []
indexes = []

for i in range(sizeX*sizeY):

	currentLine = f.readline()
	r = read_uint8(currentLine, 0, '\n', False)

	currentLine = f.readline()
	g = read_uint8(currentLine, 0, '\n', False)

	currentLine = f.readline()
	b = read_uint8(currentLine, 0, '\n', False)
	color = [r,g,b]
	if(color != [255,0,255]):
		if(not(color in pallette)):
			pallette.append(color)
			indexes.append(currentIndex)
			currentIndex += 1
			if(currentIndex > endIndex):
				print("TO MANY COLORS TO FIT THAT ALLOCATED PALLETTE SPACE")
				exit()
		foutpgm.write(str(indexes[pallette.index(color)])+"\n")
	else:
		foutpgm.write("0\n")

for i in range(len(indexes)):

	foutplt.write(str(indexes[i])+" ")
	foutplt.write(str((pallette[i][0]*64)/256)+" ")
	foutplt.write(str((pallette[i][1]*64)/256)+" ")
	foutplt.write(str((pallette[i][2]*64)/256)+"\n")





	

