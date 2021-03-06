// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <ctime>
#include <fstream>
#include <string>

#include "types.h"

// Interface for syscall generators.
class Generator {
public:
    virtual bool header(std::ofstream& os) const;
    virtual bool syscall(std::ofstream& os, const Syscall& sc) const = 0;
    virtual bool footer(std::ofstream& os) const;

protected:
    virtual ~Generator() {}
};

// Interface for vDSO wrappers.
class CallWrapper {
public:
    virtual bool applies(const Syscall& sc) const = 0;
    virtual void preCall(std::ofstream& os, const Syscall& sc) const {}
    virtual void postCall(std::ofstream& os, const Syscall& sc, std::string return_var) const {}

protected:
    virtual ~CallWrapper() {}
};

// Generate the x86_64 userspace functions.
class X86AssemblyGenerator : public Generator {
public:
    X86AssemblyGenerator(const std::string& syscall_macro, const std::string& name_prefix,
                         std::vector<CallWrapper*> call_wrappers)
        : syscall_macro_(syscall_macro),
          name_prefix_(name_prefix),
          wrappers_(call_wrappers) {}

    bool syscall(std::ofstream& os, const Syscall& sc) const override;

private:
    const std::string syscall_macro_;
    const std::string name_prefix_;
    const std::vector<CallWrapper*> wrappers_;
};

// Generate the arm64 userspace functions.
class Arm64AssemblyGenerator : public Generator {
public:
    Arm64AssemblyGenerator(const std::string& syscall_macro, const std::string& name_prefix,
                           std::vector<CallWrapper*> call_wrappers)
        : syscall_macro_(syscall_macro),
          name_prefix_(name_prefix),
          wrappers_(call_wrappers) {}

    bool syscall(std::ofstream& os, const Syscall& sc) const override;

private:
    const std::string syscall_macro_;
    const std::string name_prefix_;
    const std::vector<CallWrapper*> wrappers_;
};

// Generate the syscall number definitions.
class SyscallNumbersGenerator : public Generator {
public:
    SyscallNumbersGenerator(const std::string& define_prefix)
        : define_prefix_(define_prefix) {}

    bool syscall(std::ofstream& os, const Syscall& sc) const override;

private:
    const std::string define_prefix_;
};

// Generate debug trace info.
class TraceInfoGenerator : public Generator {
public:
    bool syscall(std::ofstream& os, const Syscall& sc) const override;
};

// Writes the signature of a syscall, up to the end of the args list.
//
// Can wrap pointers with user_ptr.
// Can specify a type to substitute for no args.
// Doesn't write ';', '{}' or attributes.
void write_syscall_signature_line(std::ofstream& os, const Syscall& sc, std::string name_prefix,
                                  std::string before_args, std::string inter_arg,
                                  bool wrap_pointers_with_user_ptr, std::string no_args_type);

// Writes the return variable declaration for a syscall.
//
// Returns the name of the variable (or an empty string if the call was void).
std::string write_syscall_return_var(std::ofstream& os, const Syscall& sc);

// Writes an invocation of a syscall.
//
// Uses the argument names specified in the type description
// Performs no casting or pointer wrapping.
void write_syscall_invocation(std::ofstream& os, const Syscall& sc,
                              const std::string& return_var, const std::string& name_prefix);
