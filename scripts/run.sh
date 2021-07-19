#!/usr/bin/env bash

set -eu

flags=(
    "-ferror-limit=1"
    "-fno-autolink"
    "-fno-exceptions"
    "-fno-math-errno"
    "-fno-rtti"
    "-fno-unwind-tables"
    "-fshort-enums"
    "-march=native"
    "-nostdlib++"
    "-O1"
    "-std=c++11"
    "-Werror"
    "-Weverything"
    "-Wno-c++98-compat"
    "-Wno-disabled-macro-expansion"
    "-Wno-extra-semi-stmt"
    "-Wno-padded"
    "-Wno-reserved-id-macro"
    "-Wno-unused-function"
)
libs=(
    "-lglfw"
    "-lGL"
)

now () {
    date +%s.%N
}

(
    start=$(now)
    if [ ! -f "$WD/src/scene_assets_codegen.hpp" ]; then
        clang++ "-I$WD/stb" "${flags[@]}" -o "$WD/bin/codegen" \
            "$WD/src/codegen.cpp"
        "$WD/bin/codegen" "$WD/assets/charmap-futuristic_black.png" \
            > "$WD/src/scene_assets_codegen.hpp"
    fi
    "$WD/scripts/codegen.py" > "$WD/src/init_assets_codegen.hpp"
    clang-format -i -verbose "$WD/src"/*
    clang++ "${libs[@]}" "${flags[@]}" -o "$WD/bin/main" "$WD/src/main.cpp"
    end=$(now)
    python3 -c "print(\"Compiled! ({:.3f}s)\n\".format($end - $start))"
)

"$WD/bin/main"
