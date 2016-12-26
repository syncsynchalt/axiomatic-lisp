; cause a lot of cells to be cons'd and lost
(def build_list (len) (
    cond (
        ((eq len 0) NIL)
        (T (cons Z (build_list (sub len 1))))
    )
))
(build_list 10)
(def trigger_gc (num unused) (
    cond (
        ((eq num 0) 0)
        (T (add 1 (trigger_gc (sub num 1) (car (build_list 10)))))
    )
))
(trigger_gc 200)
