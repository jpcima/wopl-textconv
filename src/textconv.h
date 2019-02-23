//          Copyright Jean Pierre Cimalando 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "wopx_file.h"
#include <getopt.h>
#include <stdio.h>
#include <memory>

enum { filesize_limit = 32 * 1024 * 1024 };

static void display_help();

template <WOPx_Format F>
static void textconv_file(const WOPxBankFile<F> &file);

template <WOPx_Format F>
static void textconv_global(const WOPxBankFile<F> &file);

template <WOPx_Format F>
static void textconv_inst(const WOPxInstrument<F> &inst);

template <WOPx_Format F>
static void textconv_operator(unsigned no, const WOPxOperator<F> &op);

static std::unique_ptr<uint8_t[]> file_contents(const char *path, size_t &size, size_t limit);

//==============================================================================
struct FILE_deleter {
    void operator()(FILE *x) const noexcept { fclose(x); }
};
typedef std::unique_ptr<FILE, FILE_deleter> FILE_u;
