(defun myrand (upper-bound)
  (let ((res (random upper-bound)))
    (format t "myrand ~d = ~d~%" upper-bound res)
    res))

(defun myid (val)
  (format t "debug ~a~%" val)
  val)

(defmacro with-gensyms (syms &body body)
  `(let ,(loop for s in syms collect `(,s (gensym)))
     ,@body))

(defmacro check (form)
  (with-gensyms (val)
    `(let ((,val ,form))
       (format t "~:[FAIL~;pass~] ... ~a~%" ,val ',form)
       ,val)))

(defun foo () (print "foo"))
