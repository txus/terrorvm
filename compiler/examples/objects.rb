parent = Object.new
parent.age = 50
parent.money = 1000
child = parent.clone()
child.age = 25

print "Parent age:"
puts parent.age
print "Parent money:"
puts parent.money

print "Child age:"
puts child.age
print "Child money:"
puts child.money
