; cause a lot of cells to be cons'd and lost
(def build_list (len) (
    cond (
        ((eq len 0) NIL)
        (T (cons Z (build_list (sub len 1))))
    )
))
(build_list 10)
(car (build_list 30))
(car (build_list 30))
(car (build_list 30))
(car (build_list 30))
(car (build_list 30))
(car (build_list 30))
(car (build_list 30))
(car (build_list 30))
(car (build_list 30))
(car (build_list 30))
