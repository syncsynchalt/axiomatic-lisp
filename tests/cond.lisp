; test cond implementation
(def testcond (x) (
    cond (
        ((eq x a) 1)
        ((eq x b) 2)
        (t 3)
    )
))
(testcond a)
(testcond b)
(testcond c)
