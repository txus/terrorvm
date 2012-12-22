# Terror compiler

This is a proof-of-concept compiler written in Ruby (although it runs only on
Rubinius) that compiles a subset of Ruby to Terror bytecode.

Since writing a compiler targeting a register-based VM is not trivial to me,
register allocation is rather naive and non-efficient. Basically, reusability
of registers is close to zero. If you have a suggestion about this (some book
or article regarding this topic) I'd be really happy if you pointed it out for
me to read about this.

## Usage

    $ rvm use rbx-head
    $ bin/terror examples/hello_world.rb > myprogram.tvm

Or just use the easy shell script to compile and run on the fly:

    $ rvm use rbx-head
    $ ./tvm examples/hello_world.rb