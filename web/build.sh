#!/usr/bin/env bash
# Compile le programme C++ en WebAssembly et prépare le site dans dist/.
# Prérequis : Emscripten (emcc) dans le PATH.
set -euo pipefail
cd "$(dirname "$0")/.."

mkdir -p dist
emcc main.cpp biblioWAV.cpp web/piano_web.cpp \
  -O2 -fexceptions \
  -sALLOW_MEMORY_GROWTH=1 \
  -sEXPORTED_FUNCTIONS=_piano_generer,_piano_donnees,_piano_nombre_notes,_piano_frequence,_piano_duree \
  -sEXPORTED_RUNTIME_METHODS=ccall,HEAPU8 \
  -o dist/piano.js
cp web/index.html dist/index.html
echo "Site prêt dans dist/ (piano.js, piano.wasm, index.html)"
