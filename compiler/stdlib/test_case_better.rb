TestCase = {
  setup: -> {},
  teardown: -> {},
  assert_equal: -> exp, act, msg {
    m = msg
    unless m
      m = "Expected actual to equal expected."
    end
    # self.assertions = self.assertions + 1
    if exp == act
      print "\e[32m.\e[0m"
    else
      puts "FAILED:" + m.to_s
      self.failures.push m
      print "\e[31mF\e[0m"
    end
  },
  run: -> {
    self.assertions = 0
    self.tests = 0
    self.failures = []
    self.each(-> k, v {
      # self.tests = self.tests + 1
      if v.prototype === Closure
        v.apply
      end
    })
    puts ""
    puts self.tests.to_s + " tests, " + self.assertions.to_s + " assertions, " + self.failures.length.to_s + " failures"
    self.failures.each(-> failure {
      puts "\t* " + failure
    })
  }
}

mycase = TestCase.clone
mycase.test_foo = -> {
  assert_equal 2, 2
}
mycase.test_bar = -> {
  assert_equal 2, 3
  34
}
mycase.test_baz = -> {
  assert_equal 2, 3, "Failed man!!!"
}
mycase.run

