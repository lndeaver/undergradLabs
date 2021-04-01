
;my-rotate will take a list and move the first element to the end of the list
; (my-rotate '(a b c)) returns (b c a)

;need to make the car of the list actually a list
;otherwise it's just an element and result in . a
(defun my-rotate (lst)
	(cond
		((null lst) ())
		(T (append (cdr lst) (list (car lst))))
	)
)

;my-roate-n takes a number (n) and a list and performs "my-rotate" n times
; (my-rotate-n 3 '(a b c d)) returns (d a b c)

;watch parenthesis, had ((my-rotate-n ... can change the meaning of things

(defun my-rotate-n (n lst)
	(if 
		(equal n 0) lst 
		(my-rotate-n (- n 1) (my-rotate lst)) 
	)
)

;first-sat takes two lists and a function which takes two arguments and returns
;a booleanthe. the result is a list containing the first pair of arguments
;that satisfies the function

;need funcall ... in its own set of () so it tests true or flase

(defun first-sat (lst1 lst2 foo)
	(cond
		((null lst1) '())
		((null lst2) '())
		((funcall foo (car lst1) (car lst2)) (list (car lst1) (car lst2)))
		(T (first-sat (cdr lst1) (cdr lst2) foo))
	)
)

;my-remove takes an atom and a list and returns a list with all instances of the
;atom removed.
; (my-remove 'b '(a b c d)) returns (a c d)

(defun my-remove (x lst) 
	(cond
		((null lst) '())
		((listp (car lst)) (cons (my-remove x (car lst)) (my-remove x (cdr lst))))
		((equal x (car lst)) (my-remove x (cdr lst)))
		(T (cons (car lst) (my-remove x (cdr lst))))

	)
)

;myRev takes in two lists, the second being an empty list, and reverses the first
;list into the second and returns the second list without using the reserve lisp
;function
; (myRev '(a b c) '()) returns (a b c)
(defun myRev (lstOld lstNew)
	(cond
		((null lstOld) lstNew)
		(T (myRev (cdr lstOld) (cons (car lstOld) lstNew)))
	)
)

;takes a list and returns t if the list is a palindrome and nil otherwise
(defun palindromep (lst)
	(equal lst (myRev lst '()))
)
