################ READ ME ################
#
# Save into your project folder, then do
# 'python compare.py' to run it. It will
# show the number and name of tests that
# fail. It will automatically run make &
# make run.

from subprocess import Popen, PIPE
from re import match

p = Popen(["make"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
(out, err) = p.communicate()
if err:
	print("make failed.")
	exit(1)

p = Popen(["make", "run"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
(out, err) = p.communicate()
if err:
	print("make run failed.")
	exit(1)

inputlines = [line.strip() for line in out.decode("utf-8").split("\n") if len(line.strip())]

outputfile = open("output.txt", "r")
expectedlines = [line.strip() for line in outputfile if len(line.strip())]

inputtests = []
for line in [line for line in inputlines if line != "Output:"]:
	if match(r"\.\/lang \< tests\/[0-9]{1,2}\.(good|bad)\.lang", line):
		inputtests.append([])
	else:
		inputtests[len(inputtests) - 1].append(line)

expectedtests = []
for line in [line for line in expectedlines if line != "Output:"]:
	if match(r"\.\/lang \< tests\/[0-9]{1,2}\.(good|bad)\.lang", line):
		expectedtests.append([])
	else:
		expectedtests[len(expectedtests) - 1].append(line)

failed = [str(i).rjust(2) + ".good" if i < 85 else str(i - 85).rjust(2) + ".bad" for i in range(len(inputtests)) if inputtests[i] != expectedtests[i]]

print(str(len(expectedtests) - len(failed)) + "/" + str(len(expectedtests)) + " tests passed.")

if len(failed):
	print("Failed tests:")
	for test in failed:
		print(" " + test)
