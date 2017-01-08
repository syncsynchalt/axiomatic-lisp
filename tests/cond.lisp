; test cond implementation
(def testcond (x) (
    cond ((eq x (QUOTE a)) (QUOTE 1))
         ((eq x (QUOTE b)) (QUOTE 2))
         ((QUOTE t) (QUOTE 3))
))
(testcond a)
(testcond b)
(testcond c)
