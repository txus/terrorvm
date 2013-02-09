require 'terror/instructions'
require 'terror/scope'
require 'terror/branching'

module Terror
  class Generator
    include Instructions
    include Branching
    attr_reader :literals, :scope, :ip, :parent, :stack_size
    attr_accessor :children

    def initialize(parent=nil)
      @current_line = -1
      @children = []
      @parent    = parent

      if parent
        parent.children << self
        @scope = Scope.new(parent.scope)
      else
        @scope = Scope.new
      end

      @stack_size = 0
      @literals   = []
      @ip         = 0
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

    def setline(line)
      if @current_line != line
        @ip += 2
        _setline line
        @current_line = line
      end
    end

    def push(value)
      inc
      @ip += 2
      _push literal(value)
    end

    def pop(count)
      @ip += 2
      _pop count
    end

    def clear(count)
      @ip += 2
      @stack_size -= count
      _clear count
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
      dec
      @ip += 2
      label.start!
      _jif label
    end

    def jit(label)
      dec
      @ip += 2
      label.start!
      _jit label
    end

    def goto(label)
      @ip += 2
      label.start!
      _goto label.then - 1
    end

    def pushself
      inc
      @ip += 1
      _pushself
    end

    def pushlobby
      inc
      @ip += 1
      _pushlobby
    end

    def pushlocal(name)
      inc
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
      dec
      @ip += 2
      _setslot literal(name)
    end

    def send_message(msg, argc)
      argc.times { dec }
      @ip += 3
      _send literal(msg), argc
    end

    def defn(name)
      inc
      @ip += 2
      _defn literal(name)
    end

    def makevec(count)
      (count-1).times { dec }
      @ip += 2
      _makevec count
    end

    def ret
      @ip += 1
      _ret
    end

    def clear_stack
      extra = @stack_size - 1
      if extra > 0
        @stack_size = 1
        clear extra
      end
    end

    private

    def inc
      @stack_size += 1
    end

    def dec
      @stack_size -= 1
    end

    def local name
      @scope.search_local(name)
    end

    def literal value
      inspect = value.is_a?(Symbol) ? value.to_s.inspect : value.inspect
      val = value.is_a?(Numeric) ? value : "\"#{value}"

      idx = @literals.index(val) || begin
        @literals.push val
        @literals.index(val)
      end

      "#{idx} #{inspect}"
    end
  end
end
