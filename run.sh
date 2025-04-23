#!/usr/bin/bash
execname=test
LEAK_CHECK=true

clear
clang -std=c99 -g -Wall -Wextra alist.c datatype.c main.c -o $execname
if [ $? -eq 0 ]; then
  if [ "$LEAK_CHECK" = true ]; then
    echo "Checking for memory leaks with valgrind..."
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$execname
  fi

  ./$execname > test.out
  diff test.out test.expect
else
  echo "Compilation failed."
fi
