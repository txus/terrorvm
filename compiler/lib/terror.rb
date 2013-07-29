$: << File.expand_path(File.dirname(__FILE__))
require "terror/version"
require "terror/core_ext/node"
require "terror/visitor"
require 'pathname'

module Terror
  def self.parse_file(file, verbose=false)
    ast = Rubinius::Melbourne19.parse_file(ARGV[0])
    visitor = Terror::Visitor.new
    ast.lazy_visit(visitor, ast)
    puts visitor.generator.disassemble if verbose
    # Always return nil at the end of a script
    visitor.generator.pushnil
    puts path(file)
    puts visitor.finalize('0_main')
  end

  private

  def self.path(file)
    Pathname.new(file).expand_path
  end
end
