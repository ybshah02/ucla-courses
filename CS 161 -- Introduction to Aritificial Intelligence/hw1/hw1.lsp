;;; HW 1 CS 161 Yash Shah

; Problem 1
;; The function SEQ computes and returns the Nth Padovan number (integer) given an integer N.
(defun SEQ(N)

    (cond ((< N 3) 1) ; base case for values 1, 2, and 3 ==> all equal to 1

        (t (+ (SEQ (- N 1)) (SEQ (- N 2)) (SEQ (- N 3)) )) ; recursively add the sequential of n-1, n-2, and n-3 as shown in Padovan's equation
        
    )
)

; Problem 2
;; The function SUMS computes and returns the number of additions required to compute the Nth Padovan number (integer) given an integer N.
(defun SUMS(N)

    (cond ((< N 3) 0) ; base case for values 1, 2, and 3 ==> all equal to 1

        (t (+ (SUMS (- N 1)) (SUMS (- N 2)) (SUMS (- N 3)) 1)) ; recursively add the number of additions for the n-1, n-2, and n-3 padovan numbers and add 1 for addition of those 3
        
    )
)

; Problem 3
;; The funciton takes a representation of a tree and returns an anonymized version of such tree where all nodes in the tree are replaced with 0.
(defun ANON(TREE)

    (cond 
        ((null TREE) '()) ; base case of the empty tree
        ((atom TREE) '0) ; another base for a singular node in a tree

            (t (cons (ANON (car TREE)) (ANON (cdr TREE)))) ; recursively call the function for the left and right element of each node in the tree until the tree has 1 or no elements and attach those two values
                                                           ; have to use car and cdr for the list instead of first and second, or periods will begin to show up
    )

)