require 'terror/instructions'
require 'terror/branching'

module Terror
  class Generator
    include Instructions
    include Branching
    attr_reader :literals, :locals, :ip

    Local = Struct.new(:name)

    def initialize
      @locals    = []
      @literals  = []
      @ip        = 0
    end

    def disassemble
      instructions.map(&:to_s).join "\n"
    end

    def encode(name)
      ret

      instrs = instructions.map(&:encode).flatten

      output = "_#{name}\n"
      output << ":%i:%i\n" % [@literals.size, instrs.size]

      output << @literals.join("\n")
      output << "\n"
      # instructions.each do |i|
      #   p i
      # end
      output << instrs.join("\n")
      output
    end

    def push(value)
      @ip += 1
      _push literal(value)
    end

    def pop
      @ip += 1
      _pop
    end

    def pushtrue
      @ip += 1
      _pushtrue
    end

    def pushfalse
      @ip += 1
      _pushfalse
    end

    def pushnil
      @ip += 1
      _pushnil
    end

    def jmp(label)
      label.start!
      @ip += 1
      _jmp label
    end

    def jif(label)
      label.start!
      @ip += 1
      _jif label
    end

    def jit(label)
      label.start!
      @ip += 1
      _jit label
    end

    def pushself
      @ip += 1
      _pushself
    end

    def pushlocal(name)
      @ip += 1
      _pushlocal local(name)
    end

    def setlocal(name)
      @ip += 1
      idx = local(name)
      _setlocal idx
    end

    def getslot(name)
      @ip += 1
      _getslot literal(name)
    end

    def setslot(name)
      @ip += 1
      _setslot literal(name)
    end

    def send_message(msg, argc)
      @ip += 1
      _send literal(msg), argc
    end

    def defn(name)
      @ip += 1
      _defn literal(name)
    end

    def ret
      @ip += 1
      _ret
    end

    private

    def local name
      @locals.index { |l| l.name == name } or begin
        @locals.push Local.new name
        @locals.size - 1
      end
    end

    def literal value
      val = value.is_a?(Numeric) ? value : "\"#{value}"

      @literals.index(val) or begin
        @literals.push val
        @literals.index(val)
      end
    end
  end
end
