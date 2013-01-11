$: << 'lib'
require 'terror/generator'
require 'minitest/autorun'

module Terror
  describe Generator do
    before do
      @g = Generator.new
    end

    describe 'branching labels' do
      it 'works' do
        @g.push(3)
        label = @g.new_label
        @g.push(3)
        @g.jmp label
        @g.push(3)
        @g.push(3)
        label.set!

        label.offset.must_equal 4
      end
    end

    describe '#push' do
      it 'pushes an integer literal to the stack' do
        @g.push(3)
        @g.literals.must_equal [3]
      end

      it 'pushes a string literal to the stack' do
        @g.push('foo')
        @g.literals.must_equal ['"foo']
      end
    end

    describe '#pushtrue' do
      it 'pushes true' do
        @g.must_respond_to(:pushtrue)
      end
    end

    describe '#pushfalse' do
      it 'pushes false' do
        @g.must_respond_to(:pushfalse)
      end
    end

    describe '#pushnil' do
      it 'pushes nil' do
        @g.must_respond_to(:pushnil)
      end
    end

    describe '#jmp' do
      it 'performs an inconditional jump' do
        @g.jmp(@g.new_label)
      end
    end

    describe '#jif' do
      it 'jumps a instructions if TOS is falsy' do
        @g.jif(@g.new_label)
      end
    end

    describe '#jit' do
      it 'jumps to a label if TOS is truthy' do
        @g.jit(@g.new_label)
      end
    end

    describe '#pushlocal' do
      it 'pushes a local variable' do
        @g.pushlocal :foo
        @g.scope.locals.first.must_equal :foo
      end
    end

    describe '#setlocal' do
      it 'sets a local variable' do
        @g.push 42
        @g.setlocal :foo
        @g.scope.locals.first.must_equal :foo
      end
    end

    describe '#pushlocaldepth' do
      it 'pushes a local variable from an enclosing scope' do
        @g.scope.search_local(:foo) # vivify the local in the parent scope
        g = Generator.new(@g)
        g.pushlocal :foo
        g.scope.locals.first.must_equal nil
        @g.scope.locals.first.must_equal :foo
      end
    end

    describe '#setlocaldepth' do
      it 'sets a local variable in an enclosing scope' do
        @g.scope.search_local(:foo) # vivify the local in the parent scope
        g = Generator.new(@g)
        g.push 42
        g.setlocal :foo
        g.scope.locals.first.must_equal nil
        @g.scope.locals.first.must_equal :foo
      end
    end

    describe '#getslot' do
      it 'pushes a slot from an object' do
        @g.getslot :foo
      end
    end

    describe '#setslot' do
      it 'sets a slot from an object' do
        @g.setslot :foo
      end
    end

    describe '#defn' do
      it 'defines a closure' do
        @g.defn :my_closure
      end
    end

    describe '#send' do
      it 'sends a message to an object' do
        @g.send_message(:foo, 3)
      end
    end

    describe '#ret' do
      it 'returns from the active call frame' do
        @g.ret
      end
    end
  end
end
