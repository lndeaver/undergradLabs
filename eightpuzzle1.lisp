;author Lauren Deaver
;CSCI 420 section 2
;Professor: Dr. Schwartz

;takes list in form (X (X X X X X X X X X))
;returns T if the state is the goal state
;return NIL if the state is not the goal state
(defun goal-state (lst)
	(equal lst '(1 2 3 8 e 4 7 6 5))
)

;takes list in form (X (X X X X X X X X X))
;returns the direction of the move
(defun get-direction (lst)
	(car lst)
)

;takes list in form (X (X X X X X X X X X))
;returns the state of the move
(defun get-state (lst)
	(cadr lst)
)

;takes lists in form (X (X X X X X X X X X))
;returns T if the two states are the same
;returns NIL if the two states are different
(defun same-state (lst1 lst2)
	(equal (cdr lst1) (cdr lst2))
)

;takes in list of moves in form (X (X X X X X X X X X))
;returns the list of move directions in the order they're taken
(defun path (lst) 
	(cond 
	((null lst) '()) 
	((equal nil (get-direction (car lst))) '()) 
	(T (append (path (cdr lst)) (list (get-direction (car lst)))))
	)
)

;takes lists with sublists in the form (X (X X X X X X X X X))
;returns the first list with any mvoes with states found in the second list removed
(defun remove-redundant (lst1 lst2)
	(cond
	((null lst1) '())
	((member (car lst1) lst2 :test 'same-state) 
		(remove-redundant (cdr lst1) lst2))
	(T (cons (car lst1) (remove-redundant (cdr lst1) lst2)))
	)
)

;takes in list of form (X (X X X X X X X X X))
;returns the position of e in the state (0 based)
(defun find-e (lst)
	(position 'e (get-state lst) :test 'equal)
)

;takes in list of form (X (X X X X X X X X X))
;returns the column e is found in (0 based)
(defun find-column (lst)
	(mod (find-e lst) 3)
)

;takes in list of form (X (X X X X X X X X X))
;returns the row e is found in (0 based)
(defun find-row (lst)
	(floor (/ (find-e lst) 3))
)

;takes in two elements to be swapped and a list in form of (X X X X X X X X X)
;returns lst in form (X X X X X X X X X) where elm1 and elm2 are switched 
(defun swap (elm1 elm2 lst)
	(substitute elm2 'o (substitute elm1 elm2 (substitute 'o elm1 lst)))
)

;takes list in form (X (X X X X X X X X X))
;returns a list where e is swapped with the element on its left
(defun move-L (lst)
	(cons 'L (list (swap 'e (nth (- (find-e lst) 1) (get-state lst)) (get-state lst))))
)

;takes list in form (X (X X X X X X X X X))
;returns a list where e is swapped with the element on its right
(defun move-R (lst)
	(cons 'R (list (swap 'e (nth (+ (find-e lst) 1) (get-state lst)) (get-state lst))))
)

;takes list in form (X (X X X X X X X X X))
;returns a list where e is swapped with the element above it
(defun move-U (lst)
	(cons 'U (list (swap 'e (nth (- (find-e lst) 3) (get-state lst)) (get-state lst))))
)

;takes list in form (X (X X X X X X X X X))
;returns a list where e is swapped with the element below it
(defun move-D (lst)
	(cons 'D (list (swap 'e (nth (+ (find-e lst) 3) (get-state lst)) (get-state lst))))
)

;takes list in form (X (X X X X X X X X X))
;returns T if e can be moved to a space above it
;returns NIL if e cannot be moved to a space above it
(defun can-U (lst)
	(not (equal (find-row lst) 0))
)

;takes list in form (X (X X X X X X X X X))
;returns T if e can be moved to a space below it
;returns NIL if e cannot be moved to a space blow it
(defun can-D (lst)
	(not (equal (find-row lst) 2))
)

;takes list in form (X (X X X X X X X X X))
;returns T if e can be moved to a space to the left of it
;returns NIL if e cannot be moved to a space to the left of it
(defun can-L (lst)
	(not (equal (find-column lst) 0))
)

;takes list in form (X (X X X X X X X X X))
;returns T if e can be moved to a space to the right of it
;returns NIL if e cannot be moved to a space to the right of it
(defun can-R (lst)
	(not (equal (find-column lst) 2))
)

;takes an atom x and a list and returns a list with
;all instances of x in the list removed
(defun my-remove (x lst) 
	(cond
		((null lst) '())
		((equal x (car lst)) (my-remove x (cdr lst)))
		(T (cons (car lst) (my-remove x (cdr lst))))

	)
)

;takes in list of form (X X X X X X X X X)
;given a state, returns a list of possible moved out of that state
(defun moves (lst)
	(my-remove NIL (list
		(if (can-u (cons `x (list lst))) (move-u (cons `x (list lst))))
		(if (can-d (cons `x (list lst))) (move-d (cons `x (list lst))))
		(if (can-l (cons `x (list lst))) (move-l (cons `x (list lst))))
		(if (can-r (cons `x (list lst))) (move-r (cons `x (list lst))))
	))
)
