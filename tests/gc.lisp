; cause a lot of cells to be cons'd and lost
(def build_list (len) (
    cond ((eq len (QUOTE 0)) NIL)
         ((QUOTE T) (cons (QUOTE Z) (build_list (sub len (QUOTE 1))))
    )
))
(build_list 10)
(car (build_list 40))
(car (build_list 40))
(car (build_list 40))
(car (build_list 40))
(car (build_list 40))
(car (build_list 40))
(car (build_list 40))
(car (build_list 40))
(car (build_list 40))
(car (build_list 40))
