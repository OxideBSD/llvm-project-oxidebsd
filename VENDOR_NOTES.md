# OxideBSD vendoring notes

This repo is a sparse, trimmed snapshot of upstream `llvm/llvm-project` at tag
`llvmorg-23.1.2`, not a GitHub "Fork" relationship — llvm-project's full history is
multi-gigabyte and impractical to carry here. Included: `llvm/`, `clang/`, `lld/`,
`compiler-rt/`, `cmake/`, `third-party/`, `libcxx/`, `libcxxabi/`, `libunwind/` (needed
for a statically-linked, C++-runtime-capable on-target Clang). Removed from each: `test/`,
`docs/`, `unittests/`, `examples/`, `bindings/`, `www/`, `benchmarks/` (not needed to build
the compiler itself). `third-party/unittest` and `third-party/benchmark` also removed for
the same reason.

Base import: upstream `llvmorg-23.1.1`, as a single squashed commit on `main`.

Updating to a newer upstream tag: there is no shared history to rebase onto, so apply
`git diff <old-tag> <new-tag>` restricted to files that exist in this tree (every skipped file
should be in a trimmed directory or an unvendored subproject) as one "Update to <tag>" commit on
`oxidebsd`. Done for `llvmorg-23.1.2` (all 21 vendored files applied cleanly; the 30 skipped were
tests, docs, clang-tidy and lldb).
OxideBSD-specific patches (real `Triple::OxideBSD`, a Clang toolchain driver, etc.) live on
the `oxidebsd` branch, following the same pin/update convention as `external/mit/musl` in the
main OxideBSD repo (this submodule itself lives at `external/apache2/llvm` there, not
`third_party/llvm-project` -- the whole tree was reorganized into a real BSD-style layout).
