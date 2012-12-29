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

(I have to write the new one here as well.)

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
