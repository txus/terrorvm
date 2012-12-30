module Terror
  class Local
    attr_reader :name, :depth, :index
    def initialize(name, index=0, depth=0)
      @name  = name
      @depth = depth
      @index = index
    end
  end

  class Scope
    attr_reader :parent, :locals
    def initialize(parent=nil)
      @parent = parent
      @locals = []
    end

    def search_local(name)
      # Search in current scope
      if idx = @locals.index { |l| l == name }
        Local.new(name, idx, 0)
      # Search in parent scope
      elsif @parent and local = @parent.search_local(name)
        Local.new(local.name, local.index, local.depth + 1)
      else
        new_local(name)
      end
    end

    def new_local(name)
      @locals.push name
      Local.new(name, @locals.size - 1, 0)
    end
  end
end
