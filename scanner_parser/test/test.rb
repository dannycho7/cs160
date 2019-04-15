require_relative 'testlib'

prepare do
	executable "./oracle"
	testing "./calculator"


	to :scan, "-s"
	to :parse, ""
	to :eval, "-e"
end

doing :scan do
	pass "3+2", "number3 + number2"
	pass "2+4*3+3", "number2 + number4 * number3 + number3"
	pass "2+4mod3+(2+5)", "number2 + number4 mod number3 + ( number2 + number5 )"
	pass "111*22", "number111 * number22"
	pass "((((((4+2)*3)+4)/5)mod6)+1)", "( ( ( ( ( ( number4 + number2 ) * number3 ) + number4 ) / number5 ) mod number6 ) + number1 )"
	pass "3+2()", "number3 + number2 ( )"
	pass "3++", "number3 + +"
	fail "r+3", "Scan error: found invalid character r at line 1"
	fail "3+2a", "Scan error: found invalid character a at line 1"
	fail "4^2+2", "Scan error: found invalid character ^ at line 1"
	fail "5med2","Scan error: found invalid character e at line 1"
        fail "3 2;4\n\n\n+a", "Scan error: found invalid character a at line 4"
end

doing :parse do
	pass "2+4*3+3", ""
	pass "2+4mod3+(2+5)", ""
	pass "111*22", ""
	pass "((((((4+2)*3)+4)/5)mod6)+1)", ""
	pass "3+2;5+4", ""
	fail "()", "Parse error: found invalid token ) at line 1"
	fail "(1+2", "Parse error: expected ) found EOF at line 1"
	fail "3++", "Parse error: found invalid token + at line 1"

end

doing :eval do
	pass "((2+43)mod6+12)/3; (2*3*2*19)mod8", "5\n4"
	pass "22*2-(9*5-5)", "4"
	pass "18*4-22mod3", "71"
	pass "(14mod  10)*184; 14 mod10/2; 14mod(10*10)", "736\n2\n14"
	pass "44*44*2mod33mod10", "1"
	pass "0-(180*180mod454-(10*10*2-(2*2+2)*5))", "4"
	pass "81mod3+10/3*3mod3", "0"
	pass "5+2mod3+22*7+(16+(2*4))", "185"
	pass "5+8mod(0-3)","7"
	fail "1534893*5555550", "Semantic error: number 8527174806150 out of bounds at line 1"
	fail "1/2;\n3/0", "Semantic error: dividing 3 with zero at line 2"
	fail "3+3;\n3/(3-3)", "Semantic error: dividing 3 with zero at line 2"
	fail "1/1;\n0/0", "Semantic error: dividing 0 with zero at line 2"
end

test!
