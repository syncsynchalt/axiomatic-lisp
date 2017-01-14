; sum a list of numbers
(defun sum (numbers) (
    cond ((eq (car numbers) nil) (QUOTE 0))
         ((quote T) (add (car numbers) (sum (cdr numbers))))
))
(sum (1))
(sum (1 2))
(sum (1 2 3 4))
