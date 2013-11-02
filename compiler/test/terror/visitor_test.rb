$: << 'lib'
require 'terror/visitor'
require 'rubinius/melbourne'
require 'terror/core_ext/node'

module Terror
  describe Visitor do
    include Instructions
    def visit(code)
      ast = code.to_ast
      visitor = Visitor.new
      ast.lazy_visit(visitor, ast)
      visitor.generator
    end

    def compiles(code, &block)
      g = Generator.new
      g.setline 1
      g.instance_eval(&block)
      visit(code).instructions.must_equal g.instructions
    end

    def compiles_block(code, &block)
      g = Generator.new
      g.setline 1
      g.instance_eval(&block)
      visit(code).children.first.instructions.must_equal g.instructions
    end

    it 'assigns local variables' do
      compiles("a = 3; b = 3; c = 4") do
        _push literal(3)
        _setlocal local(:a).index
        _pop 1

        _push literal(3)
        _setlocal local(:b).index
        _pop 1

        _push literal(4)
        _setlocal local(:c).index
      end
    end

    it 'remembers local names' do
      compiles("a = 3; a = 4.3") do
        _push literal(3)
        _setlocal local(:a).index
        _pop 1

        _push literal(4.3)
        _setlocal local(:a).index
      end
    end

    it 'compiles nested assignment' do
      compiles("a = b = 3") do
        _push literal(3)
        _setlocal local(:b).index
        _setlocal local(:a).index
      end
    end

    it 'compiles local variable access' do
      compiles("a = 3; a") do
        _push literal(3)
        _setlocal local(:a).index
        _pop 1

        _pushlocal local(:a).index
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
        _push literal('hey')
      end
    end

    it 'compiles messages without arguments' do
      compiles("a") do
        _pushself
        _send literal(:a), 0
      end
    end

    it 'compiles messages with arguments' do
      compiles("puts 1, 2, 3") do
        _pushself
        _push literal(1)
        _push literal(2)
        _push literal(3)
        _send literal(:puts), 3
      end
    end

    describe 'branching' do
      it 'compiles if' do
        compiles("if 1 then 3; end") do
          _push literal(1)
          _jif 4
          _push literal(3)
          _jmp 1
          _pushnil
        end
      end

      it 'compiles if-else' do
        compiles("if 1 then 3 else 4 end") do
          _push literal(1) # condition
          _jif 4
          _push literal(3) # body
          _jmp 2
          _push literal(4) # else body
        end
      end

      it 'compiles while' do
        compiles("while 1; 3; end") do
          _push literal(1) # condition
          _jif 4
          _push literal(3) # body
          _goto 5
        end
      end
    end

    describe 'slots' do
      it 'compiles objects with setters' do
        compiles("a = 3; a.foo = 9") do
          _push literal(3)
          _setlocal local(:a).index
          _pop 1

          _pushlocal local(:a).index
          _push literal(9)
          _setslot literal(:foo)
        end
      end
    end

    describe 'constants' do
      it 'are compiled down to normal slots on lobby' do
        compiles("Object.clone") do
          _pushlobby
          _getslot literal(:Object)

          _send literal(:clone), 0
        end
      end

      it 'compiles assignments too' do
        compiles("Object = 3") do
          _pushlobby
          _push literal(3)
          _setslot literal(:Object)
        end
      end
    end

    describe 'functions' do
      it 'are compiled' do
        compiles("a = -> { 3 }") do
          _defn 0
          _setlocal literal(:a)
        end
      end

      it 'are compiled with arguments' do
        compiles("bar = 1; a = -> foo { bar }") do
          _push literal(1)
          _setlocal local(:bar).index
          _pop 1

          _defn 1
          _setlocal local(:a).index
        end

        compiles_block("bar = 1; a = -> foo { foo + bar }") do
          _pushlocal 0
          _pushlocaldepth 1, 0
          _send literal(:+), 1
        end
      end

      it 'work with iterators' do
        code = "[1,2,3].each(-> num { puts num })"
        compiles(code) do
          _push literal 1
          _push literal 2
          _push literal 3
          _makevec 3
          _defn literal(:block)
          _send literal(:each), 1
        end

        compiles_block(code) do
          _pushself
          _pushlocal local(:num).index
          _send literal(:puts), 1
        end
      end

      it 'works' do
        code = "fn = -> msg { m = msg }"

        compiles(code) do
          _defn 0
          _setlocal local(:fn).index
        end

        compiles_block(code) do
          _pushlocal 0
          _setlocal 1
        end
      end
    end

    describe 'vectors' do
      it 'are compiled' do
        compiles("a = [false,true]") do
          _pushfalse
          _pushtrue
          _makevec 2
          _setlocal local(:a).index
        end
      end
    end

    describe 'maps' do
      it 'are compiled' do
        compiles("a = { false => true }") do
          _pushtrue
          _pushfalse
          _makevec 2
          _send literal(:to_map), 0
          _setlocal local(:a).index
        end
      end
    end

    describe 'assigning stuff to self' do
      it 'is compiled' do
        compiles("self.foo = 'bar'") do
          _pushself
          _push literal('bar')
          _setslot literal(:foo)
        end
      end

      it 'is compiled with hash syntax' do
        compiles("a = 2; a[:foo] = 'bar'; a[:foo]") do
          _push literal 2
          _setlocal local(:a).index
          _pop 1

          _pushlocal local(:a).index
          _push literal 'bar'
          _setslot literal(:foo)
          _pop 1

          _pushlocal local(:a).index
          _push literal(:foo)
          _send literal(:[]), 1
        end
      end
    end
  end
end
