#!/bin/bash
cat <(echo -n "// cppPosit ") <(git rev-parse HEAD) <(echo "") include/bithippop.hpp include/fixedtraits.hpp include/floattraits.hpp include/typehelpers.hpp include/unpacked.h include/posit.h | grep -vEh '#include "[^\"]+"' | grep -vh "#pragma once" - > posit_one.h
cat posit_one.h | pbcopy
echo "copied in Clipboard if macOS"