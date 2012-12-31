module Terror
  module Branching
    class Label
      def initialize(generator)
        @g    = generator
        @now  = 0
        @then = 0
      end

      def start!
        @now = @g.ip
        self
      end

      def set!
        @then = @g.ip
        self
      end

      def offset
        @then - @now
      end

      def to_i
        offset
      end

      def ==(other)
        to_i == other.to_i
      end

      def to_s
        to_i.to_s
      end
    end

    def new_label
      Label.new(self)
    end
  end
end
