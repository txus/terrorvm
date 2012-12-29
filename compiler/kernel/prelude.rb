VM.types[:object].clone = VM.primitives[:clone]
VM.types[:object].print = VM.primitives[:print]
VM.types[:object].puts  = VM.primitives[:puts]

VM.types[:integer][:+] = VM.primitives[:'integer_+']
VM.types[:integer][:-] = VM.primitives[:'integer_-']
VM.types[:integer][:/] = VM.primitives[:'integer_/']
VM.types[:integer][:*] = VM.primitives[:'integer_*']

VM.types[:vector][:[]] = VM.primitives[:'vector_[]']
VM.types[:vector][:to_map] = VM.primitives[:vector_to_map]
