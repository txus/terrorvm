a = 123
foo = 123
self.fn = -> foo {
  # foo is shadowed because it's a local argument
  a + foo
}
puts fn(2) # => 125, not 246