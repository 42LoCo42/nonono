;;; emacs-stuff --- Summary
;;; Commentary:
;;; Code:

(require 'cl-macs)

(defun my/haskell-eval-all ()
  "Evaluate all inline Haskell expressions.
They are of the form \"-- >>> expression\" and will be sent to the current
interactive-haskell-mode REPL."
  (interactive)
  (haskell-interactive-mode-clear)
  (save-excursion
    (goto-char (point-min))
    (unless (= (copy-matching-lines "^-- >>> ") 0)
      (let ((commands (split-string (string-replace "-- >>> " "" (car kill-ring)) "\n")))
        (with-selected-window (get-buffer-window (haskell-interactive-buffer))
          (cl-loop for cmd in commands do
           (haskell-interactive-mode-do-expr cmd)))))))

(provide 'emacs-stuff)
;;; emacs-stuff.el ends here
