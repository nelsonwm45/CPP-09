#!/bin/bash

# Colors
GRN="$(printf '\033[1;32m')"
BLU="$(printf '\033[1;34m')"
RED="$(printf '\033[1;31m')"
RST="$(printf '\033[0m')"

inputs=(
  "8 1 +"                         # 9
  "9 5 -"                         # 4
  "7 6 *"                         # 42
  "9 3 /"                         # 3
  "2 3 + 4 +"                     # 9
  "8 2 / 3 *"                     # 12
  "5 9 1 - *"                     # 40
  "9 2 3 + /"                     # 1
  "4 2 / 2 /"                     # 1
  "3 3 * 2 - 4 /"                 # 1
  "6 2 3 * +"                     # 12
  "5 9 + 8 7 - *"                 # 14
  "8 5 2 * -"                     # -2
  "9 1 - 8 2 / +"                 # 12
  "7 3 - 2 2 + *"                 # 16
  "6 2 / 2 / 2 /"                 # 0
  "8 0 /"                         # Error — division by zero
  "4 a +"                         # Error — invalid token 'a' (only digits 0–9 and + - * / allowed)
  "1 +"                           # Error — insufficient operands for '+'
  "2 2 2 +"                       # Error — leftover operands (stack not reduced to a single value)
  "12 3 +"                        # Error — multi-digit number '12' not allowed by subject
  "(1 2 +)"                       # Error — parentheses/invalid tokens '(' and ')'
  "9 0 0 / +"                     # Error — division by zero (0 / 0 during '/')
  "0 7 7 - +"                     # 0
)


# Expected outputs aligned by index; use "Error" for error cases.
outputs=(
  "9"
  "4"
  "42"
  "3"
  "9"
  "12"
  "40"
  "1"
  "1"
  "1"
  "12"
  "14"
  "-2"
  "12"
  "16"
  "0"
  "Error"   # division by zero
  "Error"   # invalid token 'a'
  "Error"   # insufficient operands for '+'
  "Error"   # leftover operands (not a single result)
  "Error"   # multi-digit number not allowed
  "Error"   # parentheses/invalid tokens
  "Error"   # division by zero (0/0)
  "0"
)

# Optional: build if Makefile exists
if [ -f Makefile ]; then
  echo "Building…"
  if ! make -s; then
    echo "${RED}Build failed.${RST}"
    exit 2
  fi
fi

if [ ! -x ./RPN ]; then
  echo "${RED}Missing ./RPN executable.${RST}"
  exit 2
fi

# Run tests
for i in "${!inputs[@]}"; do
  expr="${inputs[$i]}"
  exp="${outputs[$i]}"

  # Run program and capture stdout/stderr separately
  tmp_err=".$$.rpn.err"
  out="$(./RPN "$expr" 2>"$tmp_err")"
  status=$?
  err="$(cat "$tmp_err")"
  rm -f "$tmp_err"

  # Decide what to display as actual result:
  # - If expected is "Error", show "Error" when status != 0 or stderr is non-empty.
  # - Otherwise show stdout (numeric).
  actual="$out"
  if [ "$exp" = "Error" ]; then
    if [ $status -ne 0 ] || [ -n "$err" ]; then
      actual="Error"
    fi
  else
    # For success cases, ignore any stderr and show numeric stdout
    # (your program should exit 0 and print the number)
    :
  fi

  echo "Input: $expr"
  echo "Expected Output : ${GRN}${exp}${RST}"
  echo "RPN: ${BLU}${actual}${RST}"
  echo "-----------------------"
done
