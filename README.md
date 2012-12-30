# TerrorVM

![TerrorVM](https://github.com/txus/terrorvm/raw/master/assets/tvm_large.png)

A lightweight Virtual Machine for dynamic, object-oriented languages.

TODO: Write a project description

## Building the VM

    $ git clone git://github.com/txus/terrorvm.git
    $ cd terrorvm
    $ make

To run the tests:

    $ make dev

And to clean the mess:

    $ make clean

## Running programs

TerrorVM runs `.tvm` bytecode files such as the `hello_world.tvm` under the
`examples` directory.

    $ ./bin/vm examples/hello_world.tvm

It ships with a simple compiler written in Ruby (Rubinius) that compiles a
tiny subset of Ruby to `.tvm` files. Check out the `compiler` directory, which
has its own Readme, and the `compiler/examples` where we have the
`hello_world.rb` file used to produce the `hello_world.tvm`.

TerrorVM doesn't need Ruby to run; even the example compiler is a proof of
concept and could be written in any language (even in C obviously).

## Prelude

TerrorVM tries to implement as much as possible in its own code. You can find a
prelude under `compiler/examples/prelude.rb`, or in native format under
`examples/prelude.tvm`.

This prelude wires up the VM primitives to the real objects at runtime, so that your code can use them conveniently.

To recompile all examples and kernel files from Ruby to Tvm, do this:

    $ make kernel
    $ make examples

## Implementing your own dynamic language running on TerrorVM

TerrorVM is designed to run dynamic languages. You can easily implement a
compiler of your own that compiles your favorite dynamic language down to TVM
bytecode.

I've written a demo compiler in Ruby under the `compiler/` folder, just to
show how easy it is to write your own. This demo compiler compiles a subset of
Ruby down to TerrorVM bytecode, so you can easily peek at the source code or
just copy and modify it.

You can write your compiler in whatever language you prefer, of course.

## Bytecode format

(I rewrote it entirely so I have to write the new format here soon.)

### Examples

* Hello world ([Ruby code](https://github.com/txus/terrorvm/blob/master/compiler/examples/hello_world.rb), [TVM bytecode](https://github.com/txus/terrorvm/blob/master/examples/hello_world.tvm))
* Maps ([Ruby code](https://github.com/txus/terrorvm/blob/master/compiler/examples/maps.rb), [TVM code](https://github.com/txus/terrorvm/blob/master/examples/maps.tvm))
* Vectors ([Ruby code](https://github.com/txus/terrorvm/blob/master/compiler/examples/vectors.rb), [TVM code](https://github.com/txus/terrorvm/blob/master/examples/vectors.tvm))
* Numbers ([Ruby code](https://github.com/txus/terrorvm/blob/master/compiler/examples/numbers.rb), [TVM code](https://github.com/txus/terrorvm/blob/master/examples/numbers.tvm))
* Objects with prototypal inheritance ([Ruby code](https://github.com/txus/terrorvm/blob/master/compiler/examples/objects.rb), [TVM bytecode](https://github.com/txus/terrorvm/blob/master/examples/objects.tvm))
* Functions and closures ([Ruby code](https://github.com/txus/terrorvm/blob/master/compiler/examples/functions.rb), [TVM bytecode](https://github.com/txus/terrorvm/blob/master/examples/functions.tvm))

### Instruction set

* **NOOP**: no operation -- does nothing.

#### Values

* **PUSHSELF**: pushes the current self to the stack.
* **PUSH A**: pushes the literal at index `A` to the stack.
* **PUSHTRUE**: pushes the `true` object to the stack.
* **PUSHFALSE**: pushes the `false` object to the stack.
* **PUSHNIL**: pushes the `nil` object to the stack.

#### Local variables

* **PUSHLOCAL A**: pushes the local at index `A` to the stack.
* **SETLOCAL A**: sets the current top of the stack to the local variable `A`. Does not consume any stack.

#### Branching

* **JMP A**: Jumps forward as much as `A` instructions.
* **JIF A**: Jumps forward as much as `A` instructions if the top of the stack
  is falsy (`false` or `nil`).
* **JIT A**: Jumps forward as much as `A` instructions if the top of the stack
  is truthy (any value other than `false` or `nil`).

#### Slots (attributes)

* **GETSLOT A**: Pops the object at the top of the stack and asks for its slot with name `A` (a literal), pushing it to the stack if found -- if not, it'll raise an error.
* **SETSLOT A**: Pops a value to be set, then pops the object at the top of the stack and sets its slot with name `A` (a literal) to the value that was first popped. Then pushes that value back to the stack.

#### Misc

* **POP**: pops a value off the stack.
* **DEFN A**: takes the closure with the name `A` (a literal) and pushes it to
  the stack.
* **MAKEVEC A**: Pops as much as `A` elements off the stack and pushes a vector with all of them in the order they were popped (the reverse order they were pushed in the first place).

#### Call frames

* **SEND A, B**: Pops as much as `B` arguments off the stack, then the receiver,
  and sends it the message with the name `A` (a literal) with those arguments.

#### Debugging

* **DUMP**: Prints the contents of the value stack to the standard output.

## Who's this

This was made by [Josep M. Bach (Txus)](http://txustice.me) under the MIT
license. I'm [@txustice](http://twitter.com/txustice) on twitter (where you
should probably follow me!).

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
