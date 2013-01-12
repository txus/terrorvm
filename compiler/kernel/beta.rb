Vector.map = -> fn {
  new = []
  each(-> element {
    new.push fn.apply(element)
  })
  new
}

Vector.length = -> fn {
  len = 0
  each(-> element { len += 1 })
  len
}