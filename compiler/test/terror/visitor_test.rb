$: << 'lib'
require 'terror/visitor'
require 'terror/core_ext/node'
require 'minitest/autorun'

module Terror
  describe Visitor do
    include Instructions
    def visit(code)
      ast = Rubinius::Melbourne19.parse_string(code)
      visitor = Visitor.new
      ast.lazy_visit(visitor, ast)
      visitor.generator
    end

    def compiles(code, &block)
      g = Generator.new
      g.instance_eval(&block)
      visit(code).instructions.must_equal g.instructions
    end

    it 'assigns local variables' do
      compiles("a = 3; b = 3; c = 4") do
        _push 0
        _setlocal 0
        _push 0
        _setlocal 1
        _push 1
        _setlocal 2
      end
    end

    it 'remembers local names' do
      compiles("a = 3; a = 4") do
        _push 0
        _setlocal 0
        _push 1
        _setlocal 0
      end
    end

    it 'compiles nested assignment' do
      compiles("a = b = 3") do
        _push 0
        _setlocal 0
        _setlocal 1
      end
    end

    it 'compiles local variable access' do
      compiles("a = 3; a") do
        _push 0
        _setlocal 0
        _pushlocal 0
      end
    end

    it 'compiles boolean literals' do
      compiles("true") do
        _pushtrue
      end

      compiles("false") do
        _pushfalse
      end

      compiles("nil") do
        _pushnil
      end
    end

    it 'compiles string literals' do
      compiles("'hey'") do
        _push 0
      end
    end

    it 'compiles messages without arguments' do
      compiles("a") do
        _pushself
        _send 0, 0
      end
    end

    it 'compiles messages with arguments' do
      compiles("puts 1, 2, 3") do
        _pushself
        _push 0
        _push 1
        _push 2
        _send 3, 3
      end
    end

    describe 'branching' do
      it 'compiles if' do
        compiles("if 1 then 3; end") do
          _push 0
          _jif 3
          _push 1
          _jmp 2
          _pushnil
        end
      end

      it 'compiles if-else' do
        compiles("if 1 then 3 else 4 end") do
          _push 0 # condition
          _jif 3
          _push 1 # body
          _jmp 2
          _push 2 # else body
        end
      end
    end

    describe 'slots' do
      it 'compiles objects with setters' do
        compiles("a = 3; a.foo = 9") do
          _push 0
          _setlocal 0
          _pushlocal 0
          _push 1
          _setslot 2
        end
      end

      it 'compiles objects with getters and setters' do
        compiles("a = 3; a.foo = 9; a.foo") do
          # a = 3
          _push 0
          _setlocal 0

          # a.foo = 9
          _pushlocal 0
          _push 1
          _setslot 2

          # a.foo
          _pushlocal 0
          _getslot 2
        end
      end
    end

    describe 'constants' do
      it 'are compiled down to normal identifiers' do
        compiles("Object.clone") do
          _pushself
          _getslot 0

          _send 1, 0
        end
      end
    end

    describe 'functions' do
      it 'are compiled' do
        compiles("a = -> { 3 }") do
          _defn 0
          _setlocal 0
        end
      end
    end
  end
end
