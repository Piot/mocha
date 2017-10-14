(def three (fn [x] 3))

(defn two "tjoho" [a] 2)
(let [a (two 22) b (three -1) c 99]
  (log "a" a "b" b "c" c))
