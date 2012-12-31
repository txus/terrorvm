# The alpha prelude wires up all the primitives to the types.

VM.types[:object].clone = VM.primitives[:clone]
VM.types[:object].print = VM.primitives[:print]
VM.types[:object].puts  = VM.primitives[:puts]

VM.types[:number][:+] = VM.primitives[:'number_+']
VM.types[:number][:-] = VM.primitives[:'number_-']
VM.types[:number][:/] = VM.primitives[:'number_/']
VM.types[:number][:*] = VM.primitives[:'number_*']

VM.types[:string][:+] = VM.primitives[:'string_+']

VM.types[:vector][:[]]   = VM.primitives[:'vector_[]']
VM.types[:vector].push   = VM.primitives[:vector_push]
VM.types[:vector].to_map = VM.primitives[:vector_to_map]
VM.types[:vector].each   = VM.primitives[:vector_each]
VM.types[:vector].each_with_index = VM.primitives[:vector_each_with_index]
