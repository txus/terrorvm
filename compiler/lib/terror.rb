require "terror/version"
require "terror/core_ext/node"
require "terror/visitor"

module Terror
  def self.parse_file(file, verbose=false)
    ast = Rubinius::Melbourne19.parse_file(ARGV[0])
    visitor = Terror::Visitor.new
    ast.lazy_visit(visitor, ast)
    puts visitor.generator.disassemble if verbose
    puts visitor.finalize
  end
end
