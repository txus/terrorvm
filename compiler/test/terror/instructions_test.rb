$: << 'lib'
require 'terror/instructions'
require 'minitest/autorun'

module Terror
  describe Instructions do
    before do
      @g = Class.new { include Instructions }.new
    end

    describe 'instructions with 2 operands' do
      %w(send pushlocaldepth setlocaldepth).each do |instruction|
        describe instruction do
          it "#{instruction} is encoded correctly" do
            inst = @g.__send__ :"_#{instruction}", 2, 3
            value = inst.class.value

            inst.operands.must_equal [2, 3]
            inst.encode.must_equal ["#{value} #{instruction.upcase}", 2, 3]
          end
        end
      end
    end

    describe 'instructions with 1 operand' do
      %w(setline push pushlocal setlocal jmp jif jit getslot pop setslot makevec).each do |instruction|
        describe instruction do
          it "#{instruction} is encoded correctly" do
            inst = @g.__send__ "_#{instruction}", 2
            value = inst.class.value

            inst.operands.must_equal [2]
            inst.encode.must_equal ["#{value} #{instruction.upcase}", 2]
          end
        end
      end
    end

    describe 'instructions with no operands' do
      %w(noop pushself pushlobby pushtrue pushfalse pushnil ret dump).each do |instruction|
        describe instruction do
          it "#{instruction} is encoded correctly" do
            inst = @g.__send__ :"_#{instruction}"
            value = inst.class.value

            inst.operands.must_equal []
            inst.encode.must_equal ["#{value} #{instruction.upcase}"]
          end
        end
      end
    end
  end
end
