//          Copyright Jean Pierre Cimalando 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "textconv.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        display_help();
        return 0;
    }

    for (int c; (c = getopt(argc, argv, "")) != -1;) {
        switch (c) {
        default:
            display_help();
            return 1;
        }
    }

    if (argc - optind != 1) {
        display_help();
        return 1;
    }

    const char *filepath = argv[optind];
    size_t filesize;
    std::unique_ptr<uint8_t[]> filedata = file_contents(filepath, filesize, filesize_limit);
    if (!filedata)
        return 1;

    WOPLFile_Ptr wopl;
    wopl.reset(WOPL_LoadBankFromMem(filedata.get(), filesize, nullptr));
    if (wopl) {
        textconv_file<WOPL>(*wopl);
        return 0;
    }

    WOPNFile_Ptr wopn;
    wopn.reset(WOPN_LoadBankFromMem(filedata.get(), filesize, nullptr));
    if (wopn) {
        textconv_file<WOPN>(*wopn);
        return 0;
    }

    fprintf(stderr, "could not load the bank file.\n");
    return 1;
}

static void display_help()
{
    fprintf(stderr, "Usage: wopl-textconv <wopl-file>\n");
}

template <>
void textconv_global<WOPL>(const WOPLFile &file)
{
    const char *vm = "unknown";
    switch (file.volume_model) {
    case WOPL_VM_Generic: vm = "generic"; break;
    case WOPL_VM_Native: vm = "native"; break;
    case WOPL_VM_DMX: vm = "dmx"; break;
    case WOPL_VM_Apogee: vm = "apogee"; break;
    case WOPL_VM_Win9x: vm = "win9x"; break;
    }

    printf("trem+:%u\n" "vib+:%u\n" "vm:%s\n",
           (file.opl_flags & WOPL_FLAG_DEEP_TREMOLO) != 0,
           (file.opl_flags & WOPL_FLAG_DEEP_VIBRATO) != 0,
           vm);
}

template <>
void textconv_global<WOPN>(const WOPNFile &file)
{
    const char *vm = "unknown";
    switch (file.volume_model) {
    case WOPN_VM_Generic: vm = "generic"; break;
    }

    const char *ch = "unknown";
    switch (file.chip_type) {
    case WOPN_Chip_OPN2: ch = "opn2"; break;
    case WOPN_Chip_OPNA: ch = "opna"; break;
    }

    printf("lfoen:%u\n" "lfofr:%u\n" "vm:%s\n" "chip:%s\n",
           (file.lfo_freq & 8) != 0, file.lfo_freq & 7,
           vm, ch);
}

template <WOPx_Format F>
static void textconv_file(const WOPxBankFile<F> &file)
{
    printf("version: %u\n", file.version);

    printf("\n");
    textconv_global<F>(file);

    unsigned nm = file.banks_count_melodic;
    unsigned np = file.banks_count_percussion;

    for (unsigned i = 0; i < nm + np; ++i) {
        const WOPxBank<F> &bank = (i < nm) ?
            file.banks_melodic[i] : file.banks_percussive[i - nm];

        printf("\nbank %c%u:%u \"%s\"\n",
               "MP"[i >= nm],
               bank.bank_midi_msb, bank.bank_midi_lsb, bank.bank_name);

        for (unsigned j = 0; j < 128; ++j) {
            const WOPxInstrument<F> &inst = bank.ins[j];
            if ((inst.inst_flags & WOPx_T<F>::Ins_IsBlank) == 0) {
                printf("\n  instrument %c%u:%u:%u \"%s\"\n",
                       "MP"[i >= nm],
                       bank.bank_midi_msb, bank.bank_midi_lsb, j,
                       inst.inst_name);
                textconv_inst<F>(inst);
                printf("  //instrument %c%u:%u:%u \"%s\"\n",
                       "MP"[i >= nm],
                       bank.bank_midi_msb, bank.bank_midi_lsb, j,
                       inst.inst_name);
            }
        }

        printf("\n//bank %c%u:%u \"%s\"\n",
               "MP"[i >= nm],
               bank.bank_midi_msb, bank.bank_midi_lsb, bank.bank_name);
    }
}

template <>
void textconv_operator<WOPL>(unsigned no, const WOPLOperator &op)
{
    printf(
        "    OP%u  ar:%u dr:%u sl:%u rr:%u tl:%u ksl:%u mul:%u am:%u vib:%u egt:%u ksr:%u\n",
        1 + no,
        op.atdec_60 >> 4, op.atdec_60 & 15,
        op.susrel_80 >> 4, op.susrel_80 & 15,
        op.ksl_l_40 >> 6, op.ksl_l_40 & 3,
        op.avekf_20 & 15, op.avekf_20 & 128, op.avekf_20 & 64, op.avekf_20 & 32, op.avekf_20 & 16);
}

template <>
void textconv_operator<WOPN>(unsigned no, const WOPNOperator &op)
{
    printf(
        "    OP%u  ar:%u d1r:%u d2r:%u d1l:%u rr:%u tl:%u rs:%u mul:%u dt:%u am:%u ssg:%u\n",
        1 + no,
        op.rsatk_50 & 31, op.amdecay1_60 & 31, op.decay2_70 & 31, op.susrel_80 >> 4, op.susrel_80 & 15,
        op.level_40 & 127,
        op.rsatk_50 >> 6,
        op.dtfm_30 & 15, (op.dtfm_30 >> 4) & 7,
        (op.amdecay1_60 & 128) != 0,
        op.ssgeg_90 & 15);
}

template <>
void textconv_inst<WOPL>(const WOPLInstrument &inst)
{
    unsigned flg = inst.inst_flags;
    const char *rhy = "unknown";

    switch (flg & WOPL_RhythmModeMask) {
    case WOPL_RM_BassDrum: rhy = "bd"; break;
    case WOPL_RM_Snare: rhy = "sd"; break;
    case WOPL_RM_TomTom: rhy = "tt"; break;
    case WOPL_RM_Cymbal: rhy = "cy"; break;
    case WOPL_RM_HiHat: rhy = "hh"; break;
    case 0: rhy = "none"; break;
    }

    printf("    4op:%d ps4op:%d rhy:%s con1:%u fb1:%u",
           (flg & WOPL_Ins_4op) != 0, (flg & WOPL_Ins_Pseudo4op) != 0,
           rhy, inst.fb_conn1_C0 & 1, (inst.fb_conn1_C0 >> 1) & 7);
    if (flg & WOPL_Ins_4op)
        printf(" con2:%u fb2:%u",
               inst.fb_conn2_C0 & 1, (inst.fb_conn2_C0 >> 1) & 7);
    printf("\n");

    textconv_operator<WOPL>(0, inst.operators[WOPL_OP_MODULATOR1]);
    textconv_operator<WOPL>(1, inst.operators[WOPL_OP_CARRIER1]);
    if (flg & WOPL_Ins_4op) {
        textconv_operator<WOPL>(2, inst.operators[WOPL_OP_MODULATOR2]);
        textconv_operator<WOPL>(3, inst.operators[WOPL_OP_CARRIER2]);
    }

    printf("    note1:%+d", inst.note_offset1);
    if ((flg & (WOPL_Ins_4op|WOPL_Ins_Pseudo4op)) == (WOPL_Ins_4op|WOPL_Ins_Pseudo4op))
        printf(" note2:%+d", inst.note_offset2);
    printf(" vel:%+d dt2:%+d pk:%u kon:%u koff:%u",
           inst.midi_velocity_offset, inst.second_voice_detune,
           inst.percussion_key_number, inst.delay_on_ms, inst.delay_off_ms);
    printf("\n");
}

template <>
void textconv_inst<WOPN>(const WOPNInstrument &inst)
{
    printf("    alg:%u fb:%u ams:%u fms:%u\n",
           inst.fbalg & 7, (inst.fbalg >> 3) & 7,
           (inst.lfosens >> 4) & 3, inst.lfosens & 7);

    for (unsigned i = 0; i < 4; ++i)
        textconv_operator<WOPN>(i, inst.operators[i]);

    printf("    note:%+d vel:%+d pk:%u kon:%u koff:%u\n",
           inst.note_offset, inst.midi_velocity_offset,
           inst.percussion_key_number, inst.delay_on_ms, inst.delay_off_ms);
}

static std::unique_ptr<uint8_t[]> file_contents(const char *path, size_t &size, size_t limit)
{
    FILE_u file(fopen(path, "rb"));

    if (!file) {
        fprintf(stderr, "cannot open the file.\n");
        return nullptr;
    }

    size_t filesize;
    if (fseek(file.get(), 0, SEEK_END) != 0) {
        fprintf(stderr, "cannot determine the size of the file.\n");
        return nullptr;
    }
    filesize = (size_t)ftell(file.get());

    if (filesize > filesize_limit) {
        fprintf(stderr, "the file is too large to open.\n");
        return nullptr;
    }
    rewind(file.get());

    std::unique_ptr<uint8_t[]> data(new uint8_t[filesize]);
    if (fread(data.get(), 1, filesize, file.get()) != filesize) {
        fprintf(stderr, "cannot read the file.\n");
        return nullptr;
    }

    size = filesize;
    return std::move(data);
}
