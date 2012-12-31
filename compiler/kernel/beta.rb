# Vector#map
VM.types[:vector].map = -> fn {
  new = []
  each(-> element {
    new.push fn.apply(element)
  })
  new
}