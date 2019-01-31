#!/usr/bin/python


currentColor = [63,0,0]
currentIndex = 0

currentColorIndex = 0

skipCounter = 0

skipValue = 2.08

foutplt = open("rainbow.plt", "w")

for i in range(3):
	for j in range(63):
		currentIndexP1 = currentIndex+1
		if(currentIndexP1 == 3):
			currentIndexP1 = 0
		currentColor[currentIndexP1] += 1
		
		if(skipCounter < skipValue):
			foutplt.write(str(currentColorIndex) +" ")
			foutplt.write(str(currentColor[0])+" ")
			foutplt.write(str(currentColor[1])+" ")
			foutplt.write(str(currentColor[2])+"\n")
			currentColorIndex +=1
			skipCounter+=1
		else:
			skipCounter = skipCounter - skipValue
	
	for j in range(63):
		currentColor[currentIndex] -= 1

		if(skipCounter < skipValue):
			foutplt.write(str(currentColorIndex) +" ")
			foutplt.write(str(currentColor[0])+" ")
			foutplt.write(str(currentColor[1])+" ")
			foutplt.write(str(currentColor[2])+"\n")
			currentColorIndex +=1
			skipCounter+=1
		else:
			skipCounter = skipCounter - skipValue

	currentIndex += 1
