(define (f return)
  (return 2)
  3)

(f (lambda (x) x))
(call/cc f)
(display 4)
(begin
  (display 1)
  (display 2))


(define continuations '())

(define (push arg)
  (set! continuations (cons arg continuations)))

(define (capture-from-map arg)
  (call/cc
   (lambda (cc) (push cc) arg)))

(define numbers (map capture-from-map '(1 2 3 4 5 6)))

numbers
continuations
(length continuations)
((car (reverse continuations)) 76)
numbers

(call/cc (lambda (f) (f 42) (display 1337)))

(use-modules (ice-9 control))
(reset
 ((lambda (x) (display "aeiou\n") (+ 3 x)) (shift noisy (noisy (noisy 5)))))

(reset
 ((lambda (x) (display "+1\n") (+ 1 x))
  (shift i1
		 ((lambda (x) (display "*2\n") (* 2 x))
		  (shift i2
				 (i1 (i2 42)))))))

(define (needs-stuff inc dup initial)
  (inc (dup initial)))

(needs-stuff
 (lambda (x) (+ 1 x))
 (lambda (x) (* 2 x))
 42)
