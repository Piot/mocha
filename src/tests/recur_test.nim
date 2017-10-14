; (defn everything [a] true)
; (log "everything" everything)
; (filter everything [1 2 3 4])

(defn test-recur [a x]
  (if (zero? a)
    a
    (recur (dec a) (log a))))

(test-recur 99 nil)
