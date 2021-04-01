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
(defun find-n (lst x)
	(position x (get-state lst))
)

;takes in list of form (X (X X X X X X X X X))
;returns the column e is found in (0 based)
(defun find-column (lst x)
	(mod (find-n lst x) 3)
)

;takes in list of form (X (X X X X X X X X X))
;returns the row e is found in (0 based)
(defun find-row (lst x)
	(floor (/ (find-n lst x) 3))
)

;takes in two elements to be swapped and a list in form of (X X X X X X X X X)
;returns lst in form (X X X X X X X X X) where elm1 and elm2 are switched 
(defun swap (elm1 elm2 lst)
	(substitute elm2 'o (substitute elm1 elm2 (substitute 'o elm1 lst)))
)

;takes list in form (X (X X X X X X X X X))
;returns a list where e is swapped with the element on its left
(defun move-L (lst)
	(cons 'L (list (swap 'e (nth (- (find-n lst 'e) 1) (get-state lst)) (get-state lst))))
)

;takes list in form (X (X X X X X X X X X))
;returns a list where e is swapped with the element on its right
(defun move-R (lst)
	(cons 'R (list (swap 'e (nth (+ (find-n lst 'e) 1) (get-state lst)) (get-state lst))))
)

;takes list in form (X (X X X X X X X X X))
;returns a list where e is swapped with the element above it
(defun move-U (lst)
	(cons 'U (list (swap 'e (nth (- (find-n lst 'e) 3) (get-state lst)) (get-state lst))))
)

;takes list in form (X (X X X X X X X X X))
;returns a list where e is swapped with the element below it
(defun move-D (lst)
	(cons 'D (list (swap 'e (nth (+ (find-n lst 'e) 3) (get-state lst)) (get-state lst))))
)

;takes list in form (X (X X X X X X X X X))
;returns T if e can be moved to a space above it
;returns NIL if e cannot be moved to a space above it
(defun can-U (lst)
	(not (equal (find-row lst 'e) 0))
)

;takes list in form (X (X X X X X X X X X))
;returns T if e can be moved to a space below it
;returns NIL if e cannot be moved to a space blow it
(defun can-D (lst)
	(not (equal (find-row lst 'e) 2))
)

;takes list in form (X (X X X X X X X X X))
;returns T if e can be moved to a space to the left of it
;returns NIL if e cannot be moved to a space to the left of it
(defun can-L (lst)
	(not (equal (find-column lst 'e) 0))
)

;takes list in form (X (X X X X X X X X X))
;returns T if e can be moved to a space to the right of it
;returns NIL if e cannot be moved to a space to the right of it
(defun can-R (lst)
	(not (equal (find-column lst 'e) 2))
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
;_______________________________________________________


;(X X X X X X X X X)
;takes one arguement (initial state) and returns as an open list
(defun make-open-init (lst)
	(list (list (list (list) lst)))
)

;(X X X X X X X X X)
;takes one arugment (a path) and reutrns a list of possible extentions to
;the path without revisiting previous states
(defun extend-path (pth)
	(let* 
		((movelist (moves (cadar pth)))
		 (goodmoves (remove-redundant movelist pth))
		)
    
    	(extend-helper goodmoves pth)
    )
)

(defun extend-helper (mves pth)
	(cond
	((null mves) '())
	(T (cons (cons (car mves) pth) (extend-helper (cdr mves) pth)))
	)
)

;takes an open list and returns the path to the goal states using 
;beadth first search
(defun search-bfs (olst)
	(cond
		((null olst) '())
		((goal-state (cadaar olst)) (path (car olst)))
		(T (search-bfs (append (cdr olst) (extend-path (car olst)))))
	)
)

;takes an open list and a depth bound, returns the path to the goal
;state using depth first search within the limit of the depth bound
(defun search-dfs-fd (olst dbound)
	(cond
		((null olst) '())
		((goal-state (cadaar olst)) (path (car olst)))
		((> (list-length (car olst)) dbound) (search-dfs-fd (cdr olst) dbound))
		(T (search-dfs-fd (append (extend-path (car olst)) (cdr olst)) dbound))
	)
)

;implements iterative deepening for searching
;takes open list and optional depth bound and does a depth first search until
;goal path is found
(defun search-id (olst &optional (bound 0))
	(let ((dfsresult (search-dfs-fd olst bound)))
	(cond
		((null olst) '())
		((not (null dfsresult)) dfsresult)
		(T (search-id olst (+ bound 1)))
	)
	)
)

;takes an initial state and two key arguments which are optionl and describe
;which type of search is to be performed and a depth bound when necessary
(defun sss (state &key (type) (depth 7) (f #'out-of-place-f))
	(cond
	((equal type 'DFS) (search-dfs-fd (make-open-init state) depth))
	((equal type 'ID) (search-id (make-open-init state)))
	((equal type 'BFS) (search-bfs (make-open-init state)))
	((equal type 'A*) (search-A* (make-open-init state) f))
	(T (search-bfs (make-open-init state)))
	)
	
)


;_______________________________________________________




(defun out-of-place (state &optional (goal '(1 2 3 8 e 4 7 6 5)) (count 0))
	(cond
		((null goal) count)
		((null state) '())
		((goal-state state) count)
		((equal (car state) 'e) (out-of-place (cdr state) (cdr goal) count))
		((equal (car goal) 'e) (out-of-place (cdr state) (cdr goal) (+ count 1)))
		((equal (car state) (car goal)) (out-of-place (cdr state) (cdr goal) count))
		(T (out-of-place (cdr state) (cdr goal) (+ count 1)))
	)
)

(defun out-of-place-f (path)
	(+ (length (cdr path)) (out-of-place (get-state (car path))))
)


(defun manhattan (state)
	(manhattan-help state state '(1 2 3 8 e 4 7 6 5))
) 

(defun manhattan-help (ostate rstate gstate &optional (total 0))
	(let ((findingvalue (car rstate)))
	(cond
	((equal rstate '()) total)
	((equal (car rstate) 'e) (manhattan-help ostate (cdr rstate) gstate total))
	(T (manhattan-help ostate (cdr rstate) gstate (+ (+ (abs (- (find-column (cons `x (list ostate)) findingvalue) (find-column (cons `x (list gstate)) findingvalue))) (abs (- (find-row (cons `x (list ostate)) findingvalue) (find-row (cons `x (list gstate)) findingvalue)))) total)))
	)
	)
)

(defun manhattan-f (path)
	(+ (length (cdr path)) (manhattan (get-state (car path))))
)

(defun better (fun)
	(lambda (p1 p2)
		(<= (funcall fun p1) (funcall fun p2))
	)
)

(defun search-a* (olst fun)
	(cond
		((null olst) '())
		((goal-state (cadaar olst)) (path (car olst)))
		(T (search-a* (sort (append (cdr olst) (extend-path (car olst))) (better fun)) fun))
	)
)

