# TerrorVM

A lightweight Virtual Machine for dynamic, object-oriented languages. It aims to
be fast, as simple as possible, easily optimizable, with LLVM support, and
easily targetable for language designers and implementors. That's why its
interface (instruction set and bytecode format) is extensively documented and an
example compiler is provided under the `compiler` folder.

Before anything, I want to give special thanks to my awesome mentors [Jeremy
Tregunna][jtregunna], [Brian Ford][brixen], [Dirkjan Bussink][dbussink] and
[Evan Phoenix][evanphx]. Without their teachings and patience I would never have
started this in the first place.

### Disclaimer

I'd love to discuss literally **anything** about my choices regarding the design
and the implementation of TerrorVM. Feel free to ping me [on twitter][twitter],
drop me [an email][email], or if you are in Berlin, just grab some beers
together :) After all,

![I have no idea what I'm
doing](http://static2.fjcdn.com/comments/I+have+no+idea+what+I+m+doing+thread+_06af9676f192d84f17cf5a5816dc7523.jpg)

## Object model

In TerrorVM, everything is an object, and every object may have a prototype. The
basic value types that the VM provides are:

* `Number`: Double-precision floating point numbers.
* `String`: Immutable strings.
* `Vector`: Dynamically sized vectors that may contain any type.
* `Map`: Hashmaps (for now only strings are supported as keys).
* `Closure`: A first-class function.
* `True`: True boolean.
* `False`: False boolean.
* `Nil`: Represents nothingness. It is falsy just like `false`.

These basic types are objects themselves (of type `Object`). They are the
prototype for any objects of their own kind, and are provided
with all the functionality that those objects will need -- this is done
in the [prelude][prelude], I'll explain what this is a bit further ahead.

Objects are simply collections of slots that may contain any kind of object.

I'm considering adding Traits, although I'll wait until I see a need for it. In
the simplicity of Terror lies its power.

## The VM runtime object

TerrorVM exposes as much of itself as possible at runtime. The goal of this is
to make it easily targetable and flexible. For example, the toplevel object `VM`
exposes two subobjects (`types` and `primitives`). `VM.types` is a map of all
the VM types like this:

```ruby
{
  :object => Object,
  :vector => Vector,
  :number => Number,
  ...
}
```

Primitives contains all the native functions that the VM exposes (such as
`puts`, `print`, `clone`, arithmetic primitives, etc):

```ruby
{
  :clone => #<Closure ...>,
  :puts => #<Closure ...>,
  ...
}
```

## Prelude

As you already know, TerrorVM tries to implement as much as possible in its own
code, rather than C.
This makes it a perfect candidate as a multi-language VM to implement any
language on top of it. You can find a [high-level prelude][prelude] under
`compiler/examples/prelude.rb` that compiles down to [Terror native
format][prelude_native] under `examples/prelude.tvm`.

This prelude wires up the VM primitives to the real objects at runtime, so that
your code can use them conveniently. This is our current prelude in high-level
Ruby (interpreted by the VM in the bootstrap phase):

```ruby
VM.types[:object].clone = VM.primitives[:clone]
VM.types[:object].print = VM.primitives[:print]
VM.types[:object].puts  = VM.primitives[:puts]

VM.types[:number][:+] = VM.primitives[:'number_+']
VM.types[:number][:-] = VM.primitives[:'number_-']
VM.types[:number][:/] = VM.primitives[:'number_/']
VM.types[:number][:*] = VM.primitives[:'number_*']

VM.types[:vector][:[]] = VM.primitives[:'vector_[]']
VM.types[:vector][:to_map] = VM.primitives[:vector_to_map]
```

Beautiful, isn't it? :)

If you wish to change any kernel files such as the prelude, you'll have to
recompile the files to the native TVM format, like this:

    $ make kernel

And if you add more high-level examples (in Ruby) under the `compiler/examples`
folder, you must recompile them as well:

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

## Garbage collection

The algorithm of choice for TerrorVM is [Baker's treadmill][gc], a real-time,
non-moving GC algorithm. Unfortunately it is not implemented yet. It will be as
soon as I understand how to do it. Remember it's a work in progress :)

## Concurrency

This is a really important topic these days, not to be overlooked. Although its
concurrency support is not in place yet, it will feature forking, threads and
coroutines, but I might change my mind as I learn more.

## Bytecode format

The bytecode format might change to be more compact, but I'll describe what it
is for now. A file **must** contain a **main** block, and may contain other
blocks (functions defined there). This is how a block looks like (if you're
curious, it's just a hello world):

```
_main
:2:8
"hello world
"puts
16 PUSHSELF
17 PUSH
0
128 SEND
1
1
20 PUSHNIL
144 RET
```

As you can see, `_main`, defines the entry point of the file. Then these
mysterious numbers `:2:8` mean that this block has two literals and eight lines
of instructions. There are actually only 5 instructions, but the operands for
these instructions count
as well, so we're in a total of 8.

Right after these counts, we have the literals, each one in its own line. There
are two kinds of literals: numbers and strings. Numbers are just numbers, but
strings must be preceded by a `"`.

And finally we get to eight lines of numbers, namely the instructions and their
operands. The labels you see beside every instruction (`PUSHSELF`) are totally
optional, the VM doesn't read them, but they help debugging when looking at a
bytecode file manually.

After that there might be more functions. Imagine our hello world defined an
empty closure, then we'd have right after `144 RET`:

```
_block_153
:0:2
20 PUSHNIL
144 RET
```

That's it! :)

### Examples (high-level Ruby code and its Terror compiled counterpart)

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
* **PUSHLOCALDEPTH A, B**: pushes the local at index `B` from an enclosing scope
  (at depth `A`) to the stack.
* **SETLOCALDEPTH A, B**: sets the current top of the stack to the local variable `B` in an enclosing scope (at depth `A`). Does not consume any stack.

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

## Building the VM

It uses the latest C standard (C11). It is supported by GCC and Clang, so you'll
be alright.

    $ git clone git://github.com/txus/terrorvm.git
    $ cd terrorvm
    $ make

To run the tests:

    $ make dev

And to clean the mess:

    $ make clean

## Running programs

TerrorVM runs `.tvm` bytecode files such as the `numbers.tvm` under the
`examples` directory.

    $ ./bin/tvm examples/numbers.tvm

It ships with a simple compiler written in Ruby (Rubinius) that compiles a
tiny subset of Ruby to `.tvm` files. Check out the `compiler` directory, which
has its own Readme, and the `compiler/examples` where we have the
`hello_world.rb` file used to produce the `hello_world.tvm`.

**TerrorVM doesn't need Ruby to run**; even the example compiler is a proof of
concept and could be written in any language (even in C obviously).

## Who's this

This was made by [Josep M. Bach (Txus)](http://txustice.me) under the MIT
license. I'm [@txustice][twitter] on twitter (where you should probably follow
me!).

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request

[twitter]: https://twitter.com/txustice
[email]: mailto:josep.m.bach@gmail.com
[prelude]: https://github.com/txus/terrorvm/blob/master/compiler/kernel/prelude.rb
[prelude_native]: https://github.com/txus/terrorvm/blob/master/kernel/prelude.tvm
[gc]: http://www.pipeline.com/~hbaker1/NoMotionGC.html
[jtregunna]: https://twitter.com/jtregunna
[brixen]: https://twitter.com/brixen
[dbussink]: https://twitter.com/dbussink
[evanphx]: https://twitter.com/evanphx
