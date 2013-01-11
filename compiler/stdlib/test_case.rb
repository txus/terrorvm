TestCase = {
  assert_equal: -> exp, act, msg {
    unless msg
      msg = "Expected actual to equal expected."
    end
    if exp == act
      true
    else
      self.failures.push msg
      false
    end
  },
  setup: -> {},
  teardown: -> {},
  run: -> {
    self.tests    = []
    self.failures = []
    that = self;
    self.each(-> k, v {
      unless (v.prototype === Closure)
        puts 'executing!'
        tests.push k

        that.setup()
        result = v.apply()
        that.teardown()

        if result
          print "\e[32m.\e[0m"
        else
          failures.push k
          print "\e[31mF\e[0m"
        end
      end
    })

    puts ""
    if failures.length > 0
      failures.each(-> failure {
        puts "* " + failure
      })
      puts ""
    end
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