module Rubinius
  module AST
    class Node
      # Public: Works like #visit, but it doesn't visit the children just yet;
      # instead, lets the visitor decide when and how to do it.
      #
      # visitor - The visitor object. It must respond to methods named after the
      #           node names.
      #
      # Returns nothing.
      def lazy_visit(visitor, parent=nil, indent=false)
        args = [self.node_name, self, parent]
        args.push true if indent

        visitor.__send__ *args
      end
    end
  end
end
