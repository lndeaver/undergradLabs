#Lauren Deaver

from z3 import *
#split apart to get number of poeple (n) and
#	number of conflicts (c)
while True:
	n,c = map(int, input().split(" "))
	#input 0 0 stops program
	if (n == 0 and c == 0):
		break
	comm = Function('comm', IntSort(), IntSort())
	s = z3.Solver()
	maxcomm = z3.Int('maxcomm')
	#creating constraints of committees and max 
	#	number of committees - starting low
	for i in range(1, n+1):
		s.add (comm(i) >= 1)
		s.add (comm(i) <= maxcomm)
	#creating constraints of people with conflicts
	for j in range(c):
		x,y = map(int, input().split(" "))
		s.add (comm(x) != comm(y))
	#check to see if constraints satisfy
	for i in range(1, n+1):
		s.push()
		s.add(maxcomm == i)
		if s.check() == z3.sat:
			print (i)
			break
		s.pop()
	else:
		print(-1)
