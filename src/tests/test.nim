; (defn everything [a] true)
; (log "everything" everything)
; (filter everything [1 2 3 4])

(defn test-recur [a]
  (if (zero? a)
    a
    (recur test-recur (dec a))))

(test-recur 9)
