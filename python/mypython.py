#run this my saying python3 before the file
#By:Grace Bullock
#email: bullocgr@oregonstate.edu


import random

#this opens the first file and rewrites to it if it's already created
f=open("file1.txt", "w+")

#store the alphabet in a list
alphabet=['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z']

#loop 10 times and generate a random number each time
#access that index and print it to the file
for i in range(10):
  temp = random.randint(0, 25)
  printToFile = alphabet.index
  f.write('%s' % alphabet[temp])
  print(alphabet[temp], end="")


#print a new line
print()
#write a new line
f.write("\n")
#close the file
f.close()

#open the next file
f=open("file2.txt", "w+")

#loop 10 times and generate a random number each time
#access that index and print it to the file
for i in range(10):
  temp = random.randint(0, 25)
  printToFile = alphabet.index
  f.write('%s' % alphabet[temp])
  print(alphabet[temp], end="")


#print a new line
print()
#write a new line
f.write("\n")
#close the file
f.close()

#open the last file
f=open("file3.txt", "w+")

#loop 10 times and generate a random number each time
#access that index and print it to the file
for i in range(10):
  temp = random.randint(0, 25)
  printToFile = alphabet.index
  f.write('%s' % alphabet[temp])
  print(alphabet[temp], end="")

#print
print()
#print
f.write("\n")
#close
f.close()


#generate random numbers and store them to variables
num1 = random.randint(1, 42)
print(num1)
num2 = random.randint(1, 42)
print(num2)
#multiply the numbers and print it
num3 = num1*num2
print(num3)
