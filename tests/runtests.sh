echo

if test -f tests/tests.log
then
  rm tests/tests.log
fi

for i in tests/*_tests
do
  if test -f $i
  then
    if $VALGRIND ./$i 2>> tests/tests.log
    then
      echo $i PASS
    else
      echo "$i FAIL:\n"
      cat tests/tests.log
      exit 1
    fi
    echo
  fi
done

echo ""