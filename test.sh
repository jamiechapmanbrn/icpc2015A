#!/bin/bash
INFILES=in/*.in
for f in $INFILES
do
	test=$(./icpc2015A.out < $f)
	ans="${f%.in}.ans"
	ans="$(<$ans)"
	
	test "$ans" != "$test" && (echo -e Test failure for $f, '\n' expected: $ans got: $test )
done
echo Test complete! If you saw no error messages all tests passed
