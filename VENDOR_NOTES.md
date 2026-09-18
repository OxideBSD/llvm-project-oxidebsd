# OxideBSD vendoring notes

This repo is a sparse, trimmed snapshot of upstream `llvm/llvm-project` at tag
`llvmorg-23.1.1`, not a GitHub "Fork" relationship — llvm-project's full history is
multi-gigabyte and impractical to carry here. Included: `llvm/`, `clang/`, `lld/`,
`compiler-rt/`, `cmake/`, `third-party/`, `libcxx/`, `libcxxabi/`, `libunwind/` (needed
for a statically-linked, C++-runtime-capable on-target Clang). Removed from each: `test/`,
`docs/`, `unittests/`, `examples/`, `bindings/`, `www/`, `benchmarks/` (not needed to build
the compiler itself). `third-party/unittest` and `third-party/benchmark` also removed for
the same reason.

Base import: upstream `llvmorg-23.1.1`, as a single squashed commit on `main`.
OxideBSD-specific patches (real `Triple::OxideBSD`, a Clang toolchain driver, etc.) live on
the `oxidebsd` branch, following the same pin/update convention as `third_party/musl` and
`third_party/tinycc` in the main OxideBSD repo.
