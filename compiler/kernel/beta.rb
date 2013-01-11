# Vector#map
VM.types[:vector].map = -> fn {
  new = []
  each(-> element {
    new.push fn.apply(element)
  })
  new
}

VM.types[:vector].length = -> fn {
  len = 0
  each(-> element { len += 1 })
  len
}