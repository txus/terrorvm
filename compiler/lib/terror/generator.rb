require 'terror/instructions'
require 'terror/scope'
require 'terror/branching'

module Terror
  class Generator
    include Instructions
    include Branching
    attr_reader :literals, :scope, :ip, :parent
    attr_accessor :children

    def initialize(parent=nil)
      @children = []
      @parent    = parent

      if parent
        parent.children << self
        @scope = Scope.new(parent.scope)
      else
        @scope = Scope.new
      end

      @literals  = []
      @ip        = 0
    end

    def disassemble
      instructions.map(&:to_s).join "\n"
    end

    def encode(name)
      ret unless instructions[-1].inspect == "RET"

      instrs = instructions.map(&:encode).flatten

      output = "_#{name}\n"
      output << ":%i:%i\n" % [@literals.size, @ip]

      output << [@literals, instrs].flatten.join("\n")
      output
    end

    def push(value)
      @ip += 2
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
      @ip += 2
      label.start!
      _jmp label
    end

    def jif(label)
      @ip += 2
      label.start!
      _jif label
    end

    def jit(label)
      @ip += 2
      label.start!
      _jit label
    end

    def pushself
      @ip += 1
      _pushself
    end

    def pushlobby
      @ip += 1
      _pushlobby
    end

    def pushlocal(name)
      l = local(name)
      if l.depth > 0
        @ip += 3
        _pushlocaldepth l.depth, l.index
      else
        @ip += 2
        _pushlocal l.index
      end
    end

    def setlocal(name)
      l = local(name)
      if l.depth > 0
        @ip += 3
        _setlocaldepth l.depth, l.index
      else
        @ip += 2
        _setlocal l.index
      end
    end

    def getslot(name)
      @ip += 2
      _getslot literal(name)
    end

    def setslot(name)
      @ip += 2
      _setslot literal(name)
    end

    def send_message(msg, argc)
      @ip += 3
      _send literal(msg), argc
    end

    def defn(name)
      @ip += 2
      _defn literal(name)
    end

    def makevec(count)
      @ip += 2
      _makevec count
    end

    def ret
      @ip += 1
      _ret
    end

    private

    def local name
      @scope.search_local(name)
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
