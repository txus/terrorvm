# Terror compiler

This is a proof-of-concept compiler written in Ruby (although it runs only on
Rubinius) that compiles a subset of Ruby to Terror bytecode.

## Usage

    $ rvm use rbx-head
    $ bin/terror examples/hello_world.rb > myprogram.tvm

Or just use the easy shell script to compile and run on the fly:

    $ rvm use rbx-head
    $ ./tvm examples/hello_world.rb