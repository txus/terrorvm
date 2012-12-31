default_greeting = 'hello'
greeter = {}
greeter.greet = -> name {
  puts default_greeting + ' ' + name + '!'
}
greeter.greet('world')