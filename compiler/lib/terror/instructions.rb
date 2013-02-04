module Terror
  module Instructions
    module Instruction
      # Internal: Add a :value accessor to the host class when included.
      def self.included(base)
        class << base; attr_accessor :value; end
      end

      # Public: encodes an Instruction into a compact 32-bit format.
      #
      # Returns a String encoded in hexadecimal format.
      def encode
        val = "#{self.class.value} #{name}"
        [val].tap do |ary|
          ary.push(a) if respond_to?(:a)
          ary.push(b) if respond_to?(:b)
        end
      end

      # Public: Disassembles the instruction to a readable format.
      #
      # Returns a readable String representing the instruction and its operands.
      def to_s
        output = [
          name,
          # self.class.value
        ] + operands

        output.join " "
      end

      def name
        self.class.name.split('::').last.upcase
      end

      def inspect
        to_s
      end

      # Public: Checks for equality between instructions.
      #
      # Returns whether or not the two instructions are equal.
      def ==(other)
        self.class.value == other.class.value && operands == other.operands
      end

      # Internal: Returns the operands of the instruction.
      def operands
        length = if respond_to?(:members)
                   members.length
                 else
                   0
                 end
        [:a, :b].take(length).map do |operand|
          __send__ operand
        end
      end
    end

    # Internal: Little DSL helper that generates a method for a given opcode,
    # value and arity.
    #
    # name  - the Symbol opcode name (:move, :loadi...)
    # value - the Fixnum opcode value (0x00, 0x01...)
    # arity - the Fixnum number of operands that the instruction accepts
    #
    # Example
    #
    #   op :move, 0x00, 2
    #
    #   # Generates:
    #   klass = Class.new Struct.new(:a, :b)
    #   klass.value # => 0x00
    #   define_method(:move) do |*args|
    #     Move.new(*args)
    #   end
    #
    def self.op(name, value, arity)
      operands = [:a, :b].take(arity)

      if arity > 0
        klass = Class.new(Struct.new(*operands)) { include Instruction }
      else
        klass = Class.new { include Instruction }
      end
      klass.value = value

      Instructions.const_set name.to_s.capitalize, klass

      define_method(:"_#{name}") do |*args|
        instruction = Instructions.const_get(name.capitalize)
        if arity > 0
          emit instruction.new(*args)
        else
          emit instruction.new
        end
      end
    end

    # Internal: Rmits an instruction.
    #
    # instruction - the Instruction to emit.
    #
    # Returns nothing.
    def emit(instruction)
      instructions << instruction
      instruction
    end

    # op :opcode, value, arity
    op :noop,      0x00, 0
    op :setline,   0x01, 1

    op :pushself,  0x10, 0
    op :push,      0x11, 1
    op :pushtrue,  0x12, 0
    op :pushfalse, 0x13, 0
    op :pushnil,   0x14, 0
    op :pushlobby, 0x15, 0

    op :pushlocal,      0x20, 1
    op :setlocal,       0x21, 1
    op :pushlocaldepth, 0x22, 2
    op :setlocaldepth,  0x23, 2

    op :jmp,       0x30, 1
    op :jif,       0x31, 1
    op :jit,       0x32, 1
    op :goto,      0x33, 1

    op :getslot,   0x40, 1
    op :setslot,   0x41, 1

    op :pop,       0x42, 1
    op :defn,      0x43, 1
    op :makevec,   0x44, 1
    op :clear,     0x45, 1

    op :send,      0x80, 2
    op :ret,       0x90, 0

    op :dump,      0x91, 0

    # Public: Returns the Array of emitted instructions.
    def instructions
      @instructions ||= []
    end
  end
end
