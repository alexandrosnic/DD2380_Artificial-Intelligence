;; Problem definition
(define (problem problem-1)

  ;; Specifying the domain for the problem
  (:domain assembling-domain)

  ;; Objects definition
  (:objects
    ; People
    Alex
    Jessica
    ; Tools
    brush
    hammer
    ; Woods
    wood-1
  )

  ;; Intial state of problem 1
  (:init
    ;; Declaration of the objects
    ; We initialize the People
    (PERSON Alex)
    (PERSON Jessica)
    ; Tools
    (TOOL brush) 
    (TOOL hammer)
    ; Woods
    (WOOD wood-1) 
    ; Tools capabilities
    (CAN-PAINT brush) 
    (CAN-HIT hammer) 

    ;; Declaration of the predicates of the objects
    ; We set the tools as free
    (free-tool brush)
    (free-tool hammer)
    ; We set the woods as free
    (free-wood wood-1)
  )

  ;; Goal specification
  (:goal
    (and
      ; We want wood-1 assembled and approved
      (approved wood-1)
    )
  )

)
