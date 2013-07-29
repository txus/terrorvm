$: << File.expand_path(File.dirname(__FILE__))
require "terror/version"
require "terror/core_ext/node"
require "terror/visitor"
require 'pathname'

module Terror
  def self.parse_file(file, vm, verbose=false)
    ast = Rubinius::Melbourne19.parse_file(file)
    visitor = Terror::Visitor.new
    ast.lazy_visit(visitor, ast)
    puts visitor.generator.disassemble if verbose
    # Always return nil at the end of a script
    visitor.generator.pushnil
    puts path(file, vm)
    puts visitor.finalize('0_main')
  end

  private

  def self.path(file, vm)
    vm_path = Pathname.new(vm)
    path = Pathname.new(File.expand_path(file))
    path.relative_path_from(vm_path)
  end
end
