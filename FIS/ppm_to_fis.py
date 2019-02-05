import sys

def geti(file, separator):
	
	number = 0

	while True:

		c = file.read(1)

		if(c == separator):
			break
		else:
			number = number*10 + int(c)

	return number

def find_highest_index_lowest_value(list_values):
	lowest_valued_index = 0
	index = 0
	for value in list_values:
		if(value <= list_values[lowest_valued_index]):
			lowest_valued_index = index
		index += 1

	return lowest_valued_index

file = open(sys.argv[1], 'r')

c = file.read(2)

if(c != 'P3'): 
	print("WARNING, FILE PROBABLY NOT PPM, BUT CONTINUING...\n")

c = file.read(1)
c = file.read(1)

if(c == '#'): 
	c = file.readline()

size_x = geti(file, ' ')
size_y = geti(file, '\n')

c = geti(file, '\n')

image = []

for y in range(size_y):
	image.append([])
	for x in range(size_x):
		image[y].append(0)

indexed_color = []
index_times_used = []

for i in range(256):
	indexed_color.append(0)
	index_times_used.append(0)

last_index_used = -1

for y in range(size_y):
	for x in range(size_x):
		current_color = [0, 0, 0]
		color_already_used = False

		for w in range(3):
			current_color[w] = geti(file, '\n')

		for i in range(last_index_used+1):
			if(indexed_color[i] == current_color):
				image[y][x] = i
				index_times_used[i] += 1
				color_already_used = True
				break
		
		if(color_already_used == False):
			last_index_used += 1
			if(last_index_used >= 256):
				print("OVER PALETTE LIMIT OF 256!")
				exit()
			index_times_used[last_index_used] += 1
			indexed_color[last_index_used] = current_color
			image[y][x] = last_index_used

file.close()

fisfile = open(sys.argv[2], 'wb')

sequence_indicator = find_highest_index_lowest_value(index_times_used)

fisfile.write(bytes([	0xF0, 0xF0, 0x00, 
						size_x>>8, size_x & 0xFF, 
						size_y>>8, size_y & 0xFF, 
						last_index_used, sequence_indicator]))
for i in range(23):
	fisfile.write(bytes([ 0x00 ]))

sequence_counter = 0
last_index = image[0][0]

for y in range(size_y):
	for x in range(size_x):
		current_index = image[y][x]
		if(current_index == last_index and sequence_counter < 255):
			sequence_counter += 1
		else:
			if(sequence_counter < 3):
				for i in range(sequence_counter):
					fisfile.write(bytes([last_index]))
			else:
				fisfile.write(bytes([sequence_indicator, last_index, sequence_counter]))

			sequence_counter = 1

		last_index = current_index

if(sequence_counter < 3):
	for i in range(sequence_counter):
		fisfile.write(bytes([last_index]))
else:
	fisfile.write(bytes([sequence_indicator, last_index, sequence_counter]))


for c in range(last_index_used+1):
	fisfile.write(bytes(indexed_color[c]))

print("Fofonso's PPM to FIS converter")
print("Converting " + str(sys.argv[1]) + " to " + str(sys.argv[2]) + "...")
print("Size X = " + str(size_x))
print("Size Y = " + str(size_y))
print("Sequence Indicator = " + str(sequence_indicator))
print("Last Used Color Index = " + str(last_index_used))
print("Done!")

fisfile.close()
