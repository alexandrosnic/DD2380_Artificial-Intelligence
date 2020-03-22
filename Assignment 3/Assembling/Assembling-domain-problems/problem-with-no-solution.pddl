;; Problem definition
(define (problem problem-4)

  ;; Specifying the domain for the problem
  (:domain assembling-domain)

  ;; Objects definition
  (:objects
    ; People
    Alex
    Jessica
    Maggie
    ; Tools
    brush
    hammer
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
    ; Tools
    (TOOL brush) 
    (TOOL hammer)
    ; Woods
    (WOOD wood-1) 
    (WOOD wood-2) 
    (WOOD wood-3)
    ; Tools capabilities
    (CAN-PAINT brush) 
    (CAN-HIT hammer) 

    ;; Declaration of the predicates of the objects
    ; We set the tools as free
    (free-tool brush)
    (free-tool hammer)
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
