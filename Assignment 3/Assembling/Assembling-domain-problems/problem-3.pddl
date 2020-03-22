;; Problem definition
(define (problem problem-3)

  ;; Specifying the domain for the problem
  (:domain assembling-domain)

  ;; Objects definition
  (:objects
    ; People
    Alex
    Jessica
    Maggie
    John
    ; Tools
    brush-1
    brush-2
    hammer-1
    hammer-2
    ; Woods
    wood-1
    wood-2
    wood-3
  )

  ;; Intial state of problem 1
  (:init
    ;; Declaration of the objects
    ; We initialize the People
    (PERSON Alex)
    (PERSON Jessica)
    (PERSON Maggie)
    (PERSON John)
    ; Tools
    (TOOL brush-1) 
    (TOOL brush-2) 
    (TOOL hammer-1)
    (TOOL hammer-2)
    ; Woods
    (WOOD wood-1) 
    (WOOD wood-2) 
    (WOOD wood-3) 
    ; Tools capabilities
    (CAN-PAINT brush-1) 
    (CAN-PAINT brush-2) 
    (CAN-HIT hammer-1) 
    (CAN-HIT hammer-2) 

    ;; Declaration of the predicates of the objects
    ; We set the tools as free
    (free-tool brush-1)
    (free-tool brush-2)
    (free-tool hammer-2)
    (free-tool hammer-1)
    ; We set the woods as free
    (free-wood wood-1)
    (free-wood wood-2)
    (free-wood wood-3)
  )

  ;; Goal specification
  (:goal
    (and
      ; We want wood-1 assembled and approved
      (approved wood-1) (approved wood-2) (approved wood-3)
    )
  )

)
