$: << 'lib'
require 'terror/scope'

module Terror
  describe Scope do
    before do
      @s = Scope.new
    end

    describe '#search_local' do
      describe 'when the local exists in the current scope' do
        it 'returns it' do
          @s.instance_variable_set(:@locals, [:foo, :blah])
          local = @s.search_local(:blah)
          local.name.must_equal :blah
          local.index.must_equal 1
          local.depth.must_equal 0
        end
      end

      describe 'when the local exists in a parent scope' do
        it 'returns it with the correct depth' do
          @s.instance_variable_set(:@locals, [:foo, :blah])
          child = Scope.new(@s)
          subchild = Scope.new(child)

          local = subchild.search_local(:blah)
          local.name.must_equal :blah
          local.index.must_equal 1
          local.depth.must_equal 2
        end
      end

      describe 'when the local does not exist' do
        it 'vivifies it in the current scope' do
          local = @s.search_local(:foo)
          local.name.must_equal :foo
          local.index.must_equal 0
          local.depth.must_equal 0

          @s.locals.first.must_equal :foo
        end
      end
    end
  end
end
