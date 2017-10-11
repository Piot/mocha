(defn reduce [f val coll]
  (let [s (seq coll)]
    (if s
      (recur f (f val (first s)) (next s))
      val)))

(defn mapping [f]
  (fn [step]
    (fn [r x] (step r (f x)))))

(defn filtering [pred]
  (fn [step]
    (fn [r x]
      (if (pred x)
        (step r x)
        r))))

(defn mapl [f coll]
  (reduce
   ((mapping f) conj)
   []
   coll))

(defn filterl [f coll]
  (reduce
   ((filtering f) conj)
   []
   coll))
