#!/bin/sh

assert() {
  expected="$1"
  input="$2"
  
  # 如果运行不成功，则会执行exit；否则会短路exit
  ./rvcc $input > tmp.s || exit

  riscv64-unknown-elf-gcc -static -o tmp tmp.s

  # 运行
  qemu-riscv64 -L $RISCV/sysroot ./tmp

  # 程序返回值
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 0
assert 34 34
