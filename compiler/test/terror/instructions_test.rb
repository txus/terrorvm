$: << 'lib'
require 'terror/instructions'
require 'minitest/autorun'

module Terror
  describe Instructions do
    before do
      @g = Class.new { include Instructions }.new
    end

    describe 'instructions with 2 operands' do
      %w(send).each do |instruction|
        describe instruction do
          it "#{instruction} is encoded correctly" do
            inst = @g.__send__ :"_#{instruction}", 2, 3
            value = inst.class.value

            inst.operands.must_equal [2, 3]
            inst.encode.must_equal [value, 2, 3]
          end
        end
      end
    end

    describe 'instructions with 1 operand' do
      %w(push pushlocal setlocal jmp jif jit getslot setslot).each do |instruction|
        describe instruction do
          it "#{instruction} is encoded correctly" do
            inst = @g.__send__ "_#{instruction}", 2
            value = inst.class.value

            inst.operands.must_equal [2]
            inst.encode.must_equal [value, 2]
          end
        end
      end
    end

    describe 'instructions with no operands' do
      %w(noop pushself pushtrue pushfalse pushnil add pop ret dump).each do |instruction|
        describe instruction do
          it "#{instruction} is encoded correctly" do
            inst = @g.__send__ :"_#{instruction}"
            value = inst.class.value

            inst.operands.must_equal []
            inst.encode.must_equal [value]
          end
        end
      end
    end
  end
end
