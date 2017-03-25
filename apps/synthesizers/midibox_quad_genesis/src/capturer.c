/*
 * MIDIbox Quad Genesis: Capturer dialog
 *
 * ==========================================================================
 *
 *  Copyright (C) 2016 Sauraen (sauraen@gmail.com)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#include <mios32.h>
#include "capturer.h"

#include <genesis.h>

#include "cmdeditor.h"
#include "frontpanel.h"
#include "interface.h"
#include "mode_chan.h"
#include "syeng.h"

static u8 cvoice;

static void DrawMenu(){
    FrontPanel_GenesisLEDSet((selchan>>4), 0, 1, 1);
    FrontPanel_GenesisLEDSet(((selchan>>2)&3), (selchan&3)+8, 1, 1);
    MIOS32_LCD_Clear();
    MIOS32_LCD_CursorSet(0,0);
    MIOS32_LCD_PrintFormattedString("Capture voice to port %d chn %d", (selchan>>4)+1, (selchan&0xF)+1);
    u8 v = cvoice & 0x0F;
    if(v == 0 || v == 7 || v > 11){
        MIOS32_LCD_CursorSet(0,1);
        MIOS32_LCD_PrintString("Warning: nothing to capture");
    }
}

void Capturer_Start(u8 origvoice){
    subscreen = SUBSCREEN_CAPTURER;
    cvoice = origvoice;
    //Clear voice LEDs
    u8 g, v;
    for(g=0; g<GENESIS_COUNT; ++g){
        for(v=0; v<12; ++v){
            FrontPanel_GenesisLEDSet(g, v, 0, 0);
            FrontPanel_GenesisLEDSet(g, v, 1, 0);
        }
    }
    DrawMenu();
}

void Capturer_BtnSystem(u8 button, u8 state){
    if(!state) return;
    if(button == FP_B_MENU){
        subscreen = 0;
        return;
    }else if(button == FP_B_CAPTURE || button == FP_B_ENTER){
        //Compute usage
        VgmUsageBits usage = (VgmUsageBits){.all = 0};
        u8 g = cvoice >> 4;
        u8 v = cvoice & 0x0F;
        u8 usinglfo = 0;
        if(v >= 1 && v <= 6){
            usage.all |= (1 << (v-1));
            if((genesis[g].opn2.chan[v-1].lfofreqd || genesis[g].opn2.chan[v-1].lfoampd)
                    && genesis[g].opn2.lfo_enabled){
                usage.all |= (1 << (v+5));
                usinglfo = 1;
            }
        }else if(v >= 8 && v <= 10){
            usage.all |= (1 << (16+v));
        }else if(v == 11){
            usage.noise = 1;
            if(genesis[g].psg.noise.rate == 3){
                usage.sq3 = 1;
                usage.noisefreqsq3 = 1;
            }
        }else{
            //Nothing to capture
            subscreen = 0;
            return;
        }
        //Delete old program
        channels[selchan].trackermode = 0;
        if(channels[selchan].program != NULL) SyEng_DeleteProgram(selchan);
        //Create program
        synprogram_t* prog = vgmh2_malloc(sizeof(synprogram_t));
        channels[selchan].program = prog;
        prog->usage.all = usage.all;
        prog->initsource = NULL;
        prog->noteonsource = NULL;
        prog->noteoffsource = NULL;
        prog->rootnote = 60;
        sprintf(prog->name, "Captured");
        //Create init VGM
        if(v >= 1 && v <= 6){
            //Capture OPN2 parameters
            VgmSource* vs = VGM_SourceRAM_Create();
            prog->initsource = vs;
            u32 a = 0;
            u8 addrhicmd = (v >= 4) ? 0x53 : 0x52;
            u8 addrlocmd = (v-1) % 3;
            //OPN2 parameters, if applicable
            if(usinglfo){
                VGM_SourceRAM_InsertCmd(vs, a++, (VgmChipWriteCmd){ .cmd=0x52,
                    .addr = 0x22, .data = genesis[g].opn2.lforeg, .data2=0});
            }
            //Operator parameters
            u8 op, reg;
            for(reg=0; reg<7; ++reg){
                for(op=0; op<4; ++op){
                    VGM_SourceRAM_InsertCmd(vs, a++, (VgmChipWriteCmd){ .cmd=addrhicmd,
                            .addr = (0x30 + (reg << 4)) | addrlocmd | ((op & 1) << 3) | ((op & 2) << 1),
                            .data = genesis[g].opn2.chan[v-1].op[op].ALL[reg], .data2=0});
                }
            }
            //Channel parameters
            VGM_SourceRAM_InsertCmd(vs, a++, (VgmChipWriteCmd){ .cmd=addrhicmd,
                    .addr = 0xB0 | addrlocmd, .data = genesis[g].opn2.chan[v-1].algfbreg, .data2=0});
            VGM_SourceRAM_InsertCmd(vs, a++, (VgmChipWriteCmd){ .cmd=addrhicmd,
                    .addr = 0xB4 | addrlocmd, .data = genesis[g].opn2.chan[v-1].lfooutreg, .data2=0});
            VGM_Source_UpdateUsage(vs);
        }else if(v == 11){
            //Capture noise parameters
            VgmSource* vs = VGM_SourceRAM_Create();
            prog->initsource = vs;
            VGM_SourceRAM_InsertCmd(vs, 0, (VgmChipWriteCmd){
                    .cmd=0x50, .addr=0x00, .data = 0b11100000 | genesis[g].psg.noise.rate 
                    | (genesis[g].psg.noise.type << 2), .data2=0});
            VGM_Source_UpdateUsage(vs);
        }
        //Create note-on VGM
        prog->noteonsource = CreateNewVGM(1, usage);
        //Create note-off VGM
        prog->noteoffsource = CreateNewVGM(2, usage);
        //Update usage
        SyEng_RecalcSourceAndProgramUsage(prog, NULL);
        subscreen = 0;
        return;
    }
}

void Capturer_BtnGVoice(u8 gvoice, u8 state){
    //Draw old channel LED
    FrontPanel_GenesisLEDSet((selchan>>4), 0, 1, 0);
    FrontPanel_GenesisLEDSet(((selchan>>2)&3), (selchan&3)+8, 1, 0);
    //Change channels
    if((gvoice & 0xF) == 0){
        selchan = (selchan & 0xF) | (gvoice & 0xF0);
    }else if((gvoice & 0xF) >= 8){
        selchan = (selchan & 0xF0) | ((gvoice >> 2) & 0xC) | ((gvoice - 8) & 3);
    }
    DrawMenu();
}