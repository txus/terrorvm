puts VM.primitives[:+].apply(3, 4)
puts VM.primitives[:-].apply(4, 3)
puts VM.primitives[:*].apply(4, 3)
puts VM.primitives[:/].apply(4, 2)