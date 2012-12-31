symbol = '* '
greetings = ['hello', 'world', 'to', 'everyone!']
cls = -> greeting {
  puts symbol + greeting
}
greetings.each(cls)
puts 'done!'