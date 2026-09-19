//===--- OxideBSD.cpp - OxideBSD ToolChain Implementations -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "OxideBSD.h"
#include "clang/Driver/CommonArgs.h"
#include "clang/Driver/Compilation.h"
#include "clang/Options/Options.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"

using namespace clang::driver;
using namespace clang::driver::toolchains;
using namespace clang;
using namespace llvm::opt;

// OxideBSD's musl-sysroot layout (see build.rs's build_musl_sysroot and
// target/musl-sysroot/ on the host doing the cross build) places headers and
// CRT objects/libraries directly under <sysroot>/include and <sysroot>/lib -
// no "usr/" prefix, unlike the real Linux/FreeBSD convention this ToolChain
// otherwise mirrors. (The "/usr/include", "/usr/lib" paths documented in
// OxideBSD's own CLAUDE.md are the *on-target* oxfs seed layout, a distinct,
// later concern from this host-side cross-compiler's sysroot.)
//
// That "later concern" arrived: found live via the real on-target Clang/LLVM
// port's own smoke test, once `ld.lld` itself could actually be launched (see
// this fork's own musl commit fixing `posix_spawn()`'s vfork-via-clone path).
// A `--sysroot=` is never passed for an on-target invocation (there's nothing
// to point it at -- oxfs's own root *is* the sysroot), so `D.SysRoot` is
// empty at runtime and `GetFilePath("crt1.o")` only ever checked `/lib`,
// which doesn't exist in oxfs's own seed layout -- crt1.o/crti.o/crtn.o/
// libc.a all live under `/usr/lib` there (see CLAUDE.md's TinyCC section).
// `GetFilePath` silently returns the bare, unresolved filename on a miss
// (real Clang behavior, not a bug in that function), which is exactly what
// left `gnutools::Linker` invoking `ld.lld` with a plain "crt1.o" it could
// never open. Registering both search dirs keeps this constructor correct
// for the host-side cross-compile sysroot layout (`/lib`) *and* the on-target
// oxfs layout (`/usr/lib`) at once, since exactly one of the two will
// actually exist and matter for any single invocation of this same binary.
OxideBSD::OxideBSD(const Driver &D, const llvm::Triple &Triple,
                    const ArgList &Args)
    : Generic_ELF(D, Triple, Args) {
  getFilePaths().push_back(concat(getDriver().SysRoot, "/lib"));
  getFilePaths().push_back(concat(getDriver().SysRoot, "/usr/lib"));
}

void OxideBSD::AddClangSystemIncludeArgs(const ArgList &DriverArgs,
                                         ArgStringList &CC1Args) const {
  const Driver &D = getDriver();

  if (DriverArgs.hasArg(options::OPT_nostdinc))
    return;

  if (!DriverArgs.hasArg(options::OPT_nobuiltininc)) {
    SmallString<128> ResourceDirInclude(D.ResourceDir);
    llvm::sys::path::append(ResourceDirInclude, "include");
    addSystemInclude(DriverArgs, CC1Args, ResourceDirInclude.str());
  }

  if (DriverArgs.hasArg(options::OPT_nostdlibinc))
    return;

  addExternCSystemInclude(DriverArgs, CC1Args,
                          concat(D.SysRoot, "/include"));
}

std::string OxideBSD::getDynamicLinker(const ArgList &Args) const {
  // OxideBSD has no dynamic-linking-at-runtime story mature enough for this
  // to matter yet in practice (every produced binary defaults to static -
  // see isPIEDefault() in OxideBSD.h) - but musl's own real convention is
  // preserved here for when milestone-2 dynamic linking (see OxideBSD's own
  // CLAUDE.md) is ready to use it.
  return "/lib/ld-musl-x86_64.so.1";
}

Tool *OxideBSD::buildAssembler() const {
  return new tools::gnutools::Assembler(*this);
}

Tool *OxideBSD::buildLinker() const {
  return new tools::gnutools::Linker(*this);
}

Tool *OxideBSD::buildStaticLibTool() const {
  return new tools::gnutools::StaticLibTool(*this);
}
