import sys
import numpy as np

#inputFileName = sys.argv[1]

stripeRange=[]
stripeDur=[]
writeRange=[]
writeSum=0
writeCount=0
inClusWriteRange=[]
inClusWriteSum=0
inClusWriteCount=0

startTime = 999999999999
endTime = 0

def printAvg(arr):
  return sum(arr)/len(arr)

#fh = open(inputFileName,'r')
fh = sys.stdin
for line in fh:
  if line.startswith("stripeOp:"):
    stripeRange.append([float(line.split()[2]),float(line.split()[4])])
    if float(line.split()[2]) < startTime:
      startTime = float(line.split()[2])
    if float(line.split()[4]) > endTime:
      endTime = float(line.split()[4])
    stripeDur.append(float(line.split()[4])-float(line.split()[2]))
  elif line.startswith("writeOp:"):
    writeRange.append([float(line.split()[2]),float(line.split()[4])])
  elif line.startswith("inClusWriteOp:"):
    inClusWriteRange.append([float(line.split()[2]),float(line.split()[4])])

#fh.close()

#print "encoding time " + str(endTime-startTime)
print str(endTime-startTime)
np.set_printoptions(precision=2)

#for item in stripeRange:
#  print "%.2f" % (item[1] - startTime)
#print ""

for item in writeRange :
  item = np.array(item)
  #print item
  #print "%.2f" % item[0] + "," + "%.2f" % (item[1] - item[0])
  if item[1] <= endTime and item[0] >= startTime:
    writeSum = writeSum + item[1] - item[0]
    writeCount = writeCount + 1

noEncWSum = 0
noEncWCount = 0

for item in inClusWriteRange :
  item = np.array(item)
  if item[1] <= endTime and item[0] >= startTime:
    inClusWriteSum = inClusWriteSum + item[1] - item[0]
    inClusWriteCount = inClusWriteCount + 1
  else:
    noEncWSum = noEncWSum + item[1] - item[0]
    noEncWCount = noEncWCount + 1

#print writeDur

#print "stripe avg: " + str(printAvg(stripeDur))
#if writeCount != 0:
#  print "write avg: " + str(writeSum/writeCount)
#print "inClusWrite avg: " + str(inClusWriteSum/inClusWriteCount)
#print "outEncInClusWrite avg: " + str(noEncWSum/noEncWCount)

print str(printAvg(stripeDur))
#if writeCount != 0:
#  print str(writeSum/writeCount)
if inClusWriteCount != 0:
  print str(inClusWriteSum/inClusWriteCount)
if noEncWCount != 0:
  print str(noEncWSum/noEncWCount)

#print ""

