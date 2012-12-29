require 'terror/generator'

module Terror
  class Visitor
    attr_reader :generator

    def initialize(g=Generator.new)
      @generator = g
      @slots = {}
      @fns = []
    end
    alias_method :g, :generator

    def name
      "block_#{hash}"
    end

    def fixnum_literal(node, parent)
      g.push node.value
    end

    def local_variable_assignment(node, parent)
      node.value.lazy_visit self, node
      g.setlocal node.name
    end

    def local_variable_access(node, parent)
      g.pushlocal node.name
    end

    def block(node, parent)
      node.array.each do |expression|
        expression.lazy_visit self, node
      end
    end

    def nil_literal(node, parent)
      g.pushnil
    end

    def true_literal(node, parent)
      g.pushtrue
    end

    def false_literal(node, parent)
      g.pushfalse
    end

    def string_literal(node, parent)
      g.push node.string
    end

    def send(node, parent)
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
      visitor = Visitor.new
      block.body.lazy_visit(visitor)
      @fns << visitor
      g.defn visitor.name
    end

    def send_with_arguments(node, parent)
      node.receiver.lazy_visit self, node
      argc = node.arguments.array.count
      node.arguments.lazy_visit self

      g.send_message node.name, argc
    end

    def actual_arguments(node, parent)
      node.array.each do |argument|
        argument.lazy_visit(self, parent)
      end
    end

    def self(node, parent)
      g.pushself
    end

    def if(node, parent)
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
      node.receiver.lazy_visit self
      g.getslot node.name
    end

    def attribute_assignment(node, parent)
      receiver_name = node.receiver.name
      attribute_name = node.name[0..-2].to_sym
      @slots[receiver_name] ||= []
      @slots[receiver_name] << attribute_name

      node.receiver.lazy_visit self
      node.arguments.array.first.lazy_visit self
      g.setslot attribute_name
    end

    def array_literal(node, parent)
      node.body.reverse.each do |element|
        element.lazy_visit self
      end
      g.makevec node.body.count
    end

    def empty_array(node, parent)
      g.makevec 0
    end

    def hash_literal(node, parent)
      node.array.reverse.each do |element|
        element.lazy_visit self
      end
      g.makevec node.array.count

      g.send_message :to_map, 0
    end

    def symbol_literal(node, parent)
      g.push node.value
    end

    def constant_access(node, parent)
      g.pushself
      g.getslot node.name
    end

    def finalize(name)
      out = g.encode(name)
      fns = @fns.map { |fn| fn.finalize(fn.name) }.join("\n")
      [out, fns].join("\n")
    end
  end
end
