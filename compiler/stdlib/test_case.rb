TestCase = {
  assert_equal: -> exp, act, msg {
    m = msg
    unless m
      m = "Expected actual to equal expected."
    end
    if exp == act
      true
    else
      m
    end
  },
  setup: -> {},
  teardown: -> {},
  run: -> {
    self.tests    = []
    self.failures = []
    that = self
    self.each(-> k, v {
      if v.prototype === Closure
        tests.push k

        that.setup()
        result = v.apply()
        that.teardown()

        if result == true
          print "\e[32m.\e[0m"
        else
          str = k + ": " + result
          failures.push str
          print "\e[31mF\e[0m"
        end
      end
    })

    puts ""
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
}
mycase.test_baz = -> {
  assert_equal 2, 3, "Failed man!!!"
}
mycase.run