; sum a list of numbers
(def sum (numbers) (
    cond (
        ((eq (car numbers) nil) 0)
        (T (add (car numbers) (sum (cdr numbers))))
    )
))
(sum (1))
(sum (1 2))
(sum (1 2 3 4))
