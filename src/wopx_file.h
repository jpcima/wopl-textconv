//          Copyright Jean Pierre Cimalando 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "wopl/wopl_file.h"
#include "wopn/wopn_file.h"
#include <memory>

struct WOPLFile_Deleter {
    void operator()(WOPLFile *x) const noexcept { WOPL_Free(x); }
};
struct WOPNFile_Deleter {
    void operator()(WOPNFile *x) const noexcept { WOPN_Free(x); }
};
typedef std::unique_ptr<WOPLFile, WOPLFile_Deleter> WOPLFile_Ptr;
typedef std::unique_ptr<WOPNFile, WOPNFile_Deleter> WOPNFile_Ptr;

//==============================================================================
enum WOPx_Format { WOPL, WOPN };
template <WOPx_Format> struct WOPx_T;

template <>
struct WOPx_T<WOPL> {

enum {
    Ins_IsBlank = WOPL_Ins_IsBlank,
};

typedef WOPLInstrument Instrument;
typedef WOPLBank Bank;
typedef WOPIFile InstrumentFile;
typedef WOPLFile BankFile;
typedef WOPLOperator Operator;

static constexpr auto &Free = WOPL_Free;
static constexpr auto &LoadBankFromMem = WOPL_LoadBankFromMem;
static constexpr auto &LoadInstFromMem = WOPL_LoadInstFromMem;
static constexpr auto &CalculateBankFileSize = WOPL_CalculateBankFileSize;
static constexpr auto &CalculateInstFileSize = WOPL_CalculateInstFileSize;
static constexpr auto &SaveBankToMem = WOPL_SaveBankToMem;
static constexpr auto &SaveInstToMem = WOPL_SaveInstToMem;

typedef WOPLFile_Deleter BankFile_Deleter;
typedef WOPLFile_Ptr BankFile_Ptr;

};

template <>
struct WOPx_T<WOPN> {

enum {
    Ins_IsBlank = WOPN_Ins_IsBlank,
};

typedef WOPNInstrument Instrument;
typedef WOPNBank Bank;
typedef OPNIFile InstrumentFile;
typedef WOPNFile BankFile;
typedef WOPNOperator Operator;

static constexpr auto &Free = WOPN_Free;
static constexpr auto &LoadBankFromMem = WOPN_LoadBankFromMem;
static constexpr auto &LoadInstFromMem = WOPN_LoadInstFromMem;
static constexpr auto &CalculateBankFileSize = WOPN_CalculateBankFileSize;
static constexpr auto &CalculateInstFileSize = WOPN_CalculateInstFileSize;
static constexpr auto &SaveBankToMem = WOPN_SaveBankToMem;
static constexpr auto &SaveInstToMem = WOPN_SaveInstToMem;

typedef WOPNFile_Deleter BankFile_Deleter;
typedef WOPNFile_Ptr BankFile_Ptr;

};

template <WOPx_Format F> using WOPxInstrument = typename WOPx_T<F>::Instrument;
template <WOPx_Format F> using WOPxBank = typename WOPx_T<F>::Bank;
template <WOPx_Format F> using WOPxInstrumentFile = typename WOPx_T<F>::InstrumentFile;
template <WOPx_Format F> using WOPxBankFile = typename WOPx_T<F>::BankFile;
template <WOPx_Format F> using WOPxOperator = typename WOPx_T<F>::Operator;
