TestCase = {}

TestCase.setup = -> {}
TestCase.teardown = -> {}

TestCase.assert = {
  equal: -> exp, act, msg {
    if !msg
      msg = "Expected " + act.to_s + " to equal  " + exp.to_s
    end
    if exp == act
      true
    else
      self.failures.push msg
      false
    end
  }
}

TestCase.run = -> {
  tests = []
  failures = []
  self.each(-> k, v {
    unless (k == 'setup' || k == 'teardown')
      tests.push k

      setup()
      result = v.apply(assert)
      teardown()

      if result
        print "\e[32m.\e[0m"
      else
        failures.push k
        print "\e[31mF\e[0m"
      end
    end
  })
}

mycase = TestCase.clone
mycase.test_foo = -> assert {
  assert.equal 2 == 2
}
mycase.test_bar = -> assert {
  assert.equal 2 == 3
}
mycase.run