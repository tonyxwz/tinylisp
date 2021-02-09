;; fn shorthand of lambda
(def {fn}
  (\ {fargs body}
    {def (head fargs) (\ (tail fargs) body) }))


;; Fibonacci
(fn {fib n} {
  if (|| (== n 1) (== n 2))
    {1}
    {+ (fib (- n 1)) (fib (- n 2))}})

(def {nil} {})
(def {true} 1)
(def {false} 0)


;; pack unpack
(fn {pack f & xs}
  {f xs})
;;
(fn {unpack f l}
  {eval (join (list f) l)})

(def {curry} unpack)
(def {uncurry} pack)

;; example
(fn {sum l}
  {unpack + l})

;; log_x (y)
(fn {log x y}
  {div (ln y) (ln x)})

;; unpack first element
(fn {second l}
  {eval (head (tail l))})

(fn {third l}
  {eval (head (tail (tail l)))})

;; lenth in lisp way
(fn {len l} {
  if (== l nil)
    {0}
    {+ 1 (len (tail l))} })


(fn {first l}
  {eval (head l)})

(fn {nth n l} {
  if (== n 0)
    {first l}
    {nth (- n 1) (tail l)} })

(fn {last l}
  {nth (- (len l) 1) l})

;; everything inside of a s-expr will be evaluated
;; so every element in l will be evaluated directly in the env
;; `do` is called. do simply return the last element.
(fn {do & l} {
  if (== l nil)
    {nil}
    {last l}  })

;; create an empty anonymouse function and eval inside its scope
;; useful:
;;    let {do (a) (b) (c)}
(fn {let b}
  { (\ {_} b) ()})

(fn {take n l}{
  if (== n 0)
   {nil}
   {join (head l)  (take (- n 1) (tail l) )} })

(fn {drop n l} {
  if (== n 0)
    {l}
    {drop (- n 1) (tail l)} })

;; split l at nth elem
(fn {split n l}
  {join (take n l) (drop n l) })

;; x is in l
(fn {elem x l} {
  if (== l nil)
    {false}
    {if (== x (first l))
      {true}
      {elem x (tail l)}} })

(fn {map f l} {
  if (== l nil)
    {nil}
    {join (list (f (first l)))  (map f (tail l)) } })

(fn {filter f l} {
  if (== l nil)
    {nil}
    {join (if (f (first l)) {head l} {nil} ) (filter (tail l) ) } })

;; case and switch like functions
(fn {select & pairs} {
  if (== pairs nil)
    {error "no selection found"}
    {if (first (first pairs))
      {second (first pairs)}
      {unpack select (tail pairs)} } })

(def {otherwise} true)

(fn {case x & pairs} {
  if (== nil pairs)
    {error "no matching"} {
    if (== x (first (first pairs)))
      {second (first pairs)}
      {unpack case (joint (list x) (tail pairs))} }})


(fn {fib2 n} {
  select
    { (== n 0) 0 }
    { (== n 1) 1 }
    { otherwise (+ (fib2 (- n 1)) (fib2 (- n 2))) }
})