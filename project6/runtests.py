from subprocess import Popen, PIPE
from os import listdir, path, remove
from sys import platform
from functools import total_ordering

@total_ordering
class NameOrder(object):
	def __init__(self, value):
		self.value = value
	def __eq__(self, other):
		return self.value == other.value
	def __lt__(self, other):
		firstNumber, sep, firstEnding = self.value.partition(".")
		firstNumber = firstNumber.partition("/")[2]
		secondNumber, sep, secondEnding = other.value.partition(".")
		secondNumber = secondNumber.partition("/")[2]
		if (firstEnding != secondEnding):
			return firstEnding > secondEnding
		else:
			return int(firstNumber) < int(secondNumber)

def runTests():
	if (not path.isdir("tests/")):
		print("No tests directory.")
		return

	if (not path.isfile("./lang")):
		print("No `lang` executable.")
		return

	files = sorted(["tests/" + f for f in listdir('tests') if path.isfile("tests/" + f) and f.endswith(".lang")], key=NameOrder)

	for f in files:
		infile = open(f, 'r')
		asm = f + ".s"
		outfile = open(asm, 'w')

		print("./lang < " + f + ":")
		p = Popen(["./lang"], stdin=infile, stdout=outfile, stderr=PIPE)
		(out, err) = p.communicate()

		try:
			if (err):
				if (len(err.decode("utf-8").strip().split("\n")) > 1):
					print("Multiple errors produced.\n")
				else:
					print(err.decode("utf-8"))
			else:
				args = []
				if (platform == "darwin"):
					args = ["-Wl,-no_pie"]

				p = Popen(["gcc"] + args + ["-m32", "-o" ,"tests/exec" ,"tester.c", asm], stdin=PIPE, stdout=PIPE, stderr=PIPE)
				(out, err) = p.communicate()

				compiled = p.returncode
				if (compiled == 0):
					p = Popen(["tests/exec"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
					(out, err) = p.communicate()

					ran = p.returncode
					if (ran == 0):
						print("Output:")
						print(out.decode("utf-8"))
					else:
						print("Exited with an error.\n")
					remove("tests/exec")
				else:
					print("Assembling and linking failed.\n")

		
		except UnicodeDecodeError:
			print("Invalid characters in output.\n")

def main():
	runTests()

if __name__ == "__main__":
	main()
