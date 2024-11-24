%: %.l
	sbcl --disable-debugger --load $< \
		--eval "(save-lisp-and-die \"$@\" :toplevel #'main :executable t)"
