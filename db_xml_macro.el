;;; Put this function into your .emacs files
;;; Instructions found here re how this function was generated:
;;; https://emacs.stackexchange.com/questions/70/how-to-save-a-keyboard-macro-as-a-lisp-function

;;; Open db.xml side by side with the calibration output table file generated
;;; by the visualC++ code (calibration_table.csv). These should be the only two emacs buffers open.
;;; Then call M-x gen_db_xml with your cursor at the start of the id_1 line of the db.xml and the
;;; other cursor on the beginning of the id 1 line of the table output. Enclose this command in
;;; a kbd macro to repeat it for all lasers
(fset 'gen_db_xml
   (lambda (&optional arg) "Keyboard macro." (interactive "p") (kmacro-exec-ring-item (quote ([5 right 24 111 down 19 58 right 67108896 5 134217847 right 24 111 1 right 19 118 101 114 116 C-right right right 25 67108896 19 60 left 23 up up right 19 62 right left 24 111 19 58 right 67108896 5 134217847 right 24 111 25 67108896 19 60 left 23 5 down down 1 19 95 62 right left 67108896 M-right left left 19 60 left 23 24 111 19 58 right 67108896 5 134217847 right 24 111 25 5 right 19 95 62 right left 67108896 19 60 left 23 24 111 19 58 right 67108896 5 134217847 right 24 111 25 5 right 19 62 right left 67108896 19 60 left 23 24 111 19 58 right 67108896 5 134217847 right 24 111 25 5 right 19 95 62 right left 67108896 19 60 left 23 24 111 19 58 right 67108896 5 134217847 right 24 111 25 5 right 24 111 19 58 right 67108896 5 134217847 right 24 111 19 62 right left 25 67108896 19 60 23 left left 5 60 backspace 1 19 62 right left 25 19 60 left 18 62 right 25 67108896 19 60 left 23 5 right 24 111 19 58 right 67108896 5 134217847 24 111 19 62 right backspace 25 5 right 24 111 right 19 58 right 67108896 5 134217847 right 24 111 19 95 62 right backspace 25 18 62 right 25 67108896 19 60 left 23 5 right 19 105 100 19 19 1 24 111 down down down 24 111] 0 "%d")) arg)))


;;; This function populates the column in db xml with min intensities.
;; Open the db.xml file and calibration table output file side by side in 
;;; buffers. Then have cursor at the beginning of the first <item>
;;; in db.xml's minIntensity list and the other cursor at the beginning
;;; of the file in table_output.csv. Enclose in a kbd macro to repeatedly
;;; populate the min intensities with this command
(fset 'minIntensity_populate
   (lambda (&optional arg) "Keyboard macro." (interactive "p") (kmacro-exec-ring-item (quote ([19 60 105 116 101 109 62 right left 67108896 19 60 left 23 24 111 19 109 105 110 95 105 110 116 101 110 115 105 116 121 58 right 67108896 5 134217847 24 111 25 5 right 24 111 19 108 97 115 101 114 1 24 111] 0 "%d")) arg)))

;;; This function populates the column in db xml with max intensities.
;; Open the db.xml file and calibration table output file side by side in 
;;; buffers. Then have cursor at the beginning of the first <item>
;;; in db.xml's maxIntensity list and the other cursor at the beginning
;;; of the file in table_output.csv. Enclose in a kbd macro to repeatedly
;;; populate the min intensities with this command
(fset 'maxIntensity_populate
   (lambda (&optional arg) "Keyboard macro." (interactive "p") (kmacro-exec-ring-item (quote ([19 60 105 116 101 109 62 right left 67108896 19 60 left 23 24 111 19 109 97 120 95 105 110 116 101 110 115 105 116 121 58 right 67108896 5 134217847 24 111 25 5 right 24 111 19 108 97 115 101 114 1 24 111] 0 "%d")) arg)))