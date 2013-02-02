require 'terror/generator'
require 'securerandom'

module Terror
  class Visitor
    attr_reader :generator

    attr_accessor :linenum

    def initialize(parent=nil)
      @generator = Generator.new(parent)
      @linenum = 0
      @slots = {}
      @fns = []
    end
    alias_method :g, :generator

    def name
      @name ||= "#{linenum}_block_#{SecureRandom.hex}"
    end

    def fixnum_literal(node, parent)
      setline(node)
      g.push node.value
    end

    def float_literal(node, parent)
      setline(node)
      g.push node.value
    end

    def local_variable_assignment(node, parent)
      setline(node)
      node.value.lazy_visit self, node
      g.setlocal node.name
    end

    def local_variable_access(node, parent)
      setline(node)
      g.pushlocal node.name
    end

    def block(node, parent)
      setline(node)
      node.array.each do |expression|
        expression.lazy_visit self, node
      end
    end

    def nil_literal(node, parent)
      setline(node)
      g.pushnil
    end

    def true_literal(node, parent)
      setline(node)
      g.pushtrue
    end

    def false_literal(node, parent)
      setline(node)
      g.pushfalse
    end

    def string_literal(node, parent)
      setline(node)
      g.push node.string
    end

    def send(node, parent)
      setline(node)
      if node.receiver.respond_to?(:name) && @slots[node.receiver.name] && @slots[node.receiver.name].include?(node.name)
        return slot_retrieval(node, parent)
      end

      if node.name == :lambda && node.block
        return defn(node.block)
      end

      node.receiver.lazy_visit self, node
      g.send_message node.name, 0
    end

    def defn(block)
      setline(block)
      visitor = Visitor.new(g)
      visitor.linenum = block.line
      block.arguments.names.each do |arg|
        # vivify arguments as locals
        visitor.g.scope.new_local(arg)
      end
      block.body.lazy_visit(visitor)
      @fns << visitor
      g.defn visitor.name
    end

    def send_with_arguments(node, parent)
      setline(node)
      node.receiver.lazy_visit self, node
      argc = node.arguments.array.count
      node.arguments.lazy_visit self

      g.send_message node.name, argc
    end

    def actual_arguments(node, parent)
      setline(node)
      node.array.each do |argument|
        argument.lazy_visit(self, parent)
      end
    end

    def self(node, parent)
      setline(node)
      g.pushself
    end

    def if(node, parent)
      setline(node)
      node.condition.lazy_visit(self, parent)

      body_val = else_val = nil

      done = g.new_label
      else_label = g.new_label

      g.jif else_label

      node.body.lazy_visit(self, parent)
      g.jmp done

      else_label.set!

      node.else.lazy_visit(self, parent)

      done.set!
    end

    def slot_retrieval(node, parent)
      setline(node)
      node.receiver.lazy_visit self
      g.getslot node.name
    end

    def attribute_assignment(node, parent)
      setline(node)
      receiver_name = if node.receiver.is_a?(Rubinius::AST::Self)
                        :self
                      else
                        node.receiver.name
                      end
      attribute_name = node.name[0..-2].to_sym
      @slots[receiver_name] ||= []
      @slots[receiver_name] << attribute_name

      node.receiver.lazy_visit self
      node.arguments.array.first.lazy_visit self
      g.setslot attribute_name
    end

    def constant_assignment(node, parent)
      setline(node)
      receiver_name = :lobby
      attribute_name = node.constant.name.to_sym
      @slots[receiver_name] ||= []
      @slots[receiver_name] << attribute_name

      g.pushlobby
      node.value.lazy_visit self
      g.setslot attribute_name
    end

    def element_assignment(node, parent)
      setline(node)
      receiver_name = if node.receiver.is_a?(Rubinius::AST::Self)
                        :self
                      else
                        node.receiver.name
                      end
      attr = node.arguments.array.first
      attribute_name = attr.respond_to?(:value) ? attr.value : attr.variable
      @slots[receiver_name] ||= []
      @slots[receiver_name] << attribute_name

      node.receiver.lazy_visit self
      node.arguments.array[1].lazy_visit self
      g.setslot attribute_name
    end

    def array_literal(node, parent)
      setline(node)
      node.body.reverse.each do |element|
        element.lazy_visit self
      end
      g.makevec node.body.count
    end

    def empty_array(node, parent)
      setline(node)
      g.makevec 0
    end

    def hash_literal(node, parent)
      setline(node)
      node.array.reverse.each do |element|
        element.lazy_visit self
      end
      g.makevec node.array.count

      g.send_message :to_map, 0
    end

    def symbol_literal(node, parent)
      setline(node)
      g.push node.value
    end

    def constant_access(node, parent)
      setline(node)
      g.pushlobby
      g.getslot node.name
    end

    def or(node, parent)
      setline(node)
      node.left.lazy_visit self
      node.right.lazy_visit self
      g.send_message :or, 1
    end

    def finalize(name)
      g.clear_stack
      out = g.encode(name)
      fns = @fns.map { |fn| fn.finalize(fn.name) }
      [out, fns].flatten.join("\n")
    end

    private

    def setline(node)
      g.setline(node.line)
    end
  end
end

