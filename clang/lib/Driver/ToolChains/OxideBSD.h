//===--- OxideBSD.h - OxideBSD ToolChain Implementations -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// OxideBSD (https://github.com/Pomsky2011/OxideBSD) is a from-scratch, x86_64-
// only Rust kernel with a real, patched musl libc port and its own native
// syscall ABI. Its OS identity is genuine (Triple::OxideBSD), not a borrowed
// Linux one, but its on-disk ELF/CRT/dynamic-linker conventions are musl/
// Linux-shaped (its musl fork is patched only under arch/x86_64, unchanged
// elsewhere) - so this ToolChain reuses the generic GNU-style Assembler/
// Linker/StaticLibTool (tools::gnutools::*, see Gnu.h) exactly like Linux
// does, rather than writing FreeBSD/NetBSD-style bespoke tools for a real
// BSD's own distinct ABI conventions, which OxideBSD does not share.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_OXIDEBSD_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_OXIDEBSD_H

#include "Gnu.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY OxideBSD : public Generic_ELF {
public:
  OxideBSD(const Driver &D, const llvm::Triple &Triple,
           const llvm::opt::ArgList &Args);

  bool HasNativeLLVMSupport() const override { return true; }
  bool IsMathErrnoDefault() const override { return false; }

  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override;

  RuntimeLibType GetDefaultRuntimeLibType() const override {
    return ToolChain::RLT_CompilerRT;
  }
  CXXStdlibType GetDefaultCXXStdlibType() const override {
    return ToolChain::CST_Libcxx;
  }
  UnwindLibType GetDefaultUnwindLibType() const override {
    return ToolChain::UNW_None;
  }

  std::string getDynamicLinker(const llvm::opt::ArgList &Args) const override;
  const char *getDefaultLinker() const override { return "ld.lld"; }

  // No on-target dynamic-linking-at-runtime story mature enough yet (see
  // OxideBSD's own CLAUDE.md "Dynamic linking: milestone 1" section) - every
  // binary this ToolChain produces should default to static unless a caller
  // explicitly asks for PIE/shared.
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override {
    return false;
  }

protected:
  Tool *buildAssembler() const override;
  Tool *buildLinker() const override;
  Tool *buildStaticLibTool() const override;
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_OXIDEBSD_H
