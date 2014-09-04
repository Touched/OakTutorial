#!/usr/bin/python3

import os

def linker_script(offset):
    out = """OUTPUT_ARCH(arm)
MEMORY {{

        rom     : ORIGIN = 0x{:08X}, LENGTH = 32M
        ewram   : ORIGIN = 0x02000000, LENGTH = 4M - 4k
}}

SECTIONS {{
        .text : {{

                FILL (0xABCD)

                __text_start = . ;
                *(.init)
                *(.text)
                *(.ctors)
                *(.dtors)
                *(.rodata)
                *(.fini)
                *(COMMON)
                __text_end  = . ;

                __bss_start__ = . ;
                *(.bss)
                __bss_end__ = . ;
        _end = __bss_end__ ;
        __end__ = __bss_end__ ;
        }} >rom = 0xff
}}
    """.format(offset)

    return out

def target(path):
    base, ext = os.path.splitext(path.strip())

    # TODO: Ensure target name is unique
    target_name = base



    if ext in ['.s', '.asm']:
        target_name += '.o'
        output = '{}: {}\n\t'.format(target_name, path.strip())
        output += '$(AS) $(AFLAGS) -c {} -o {}'.format(path.strip(), target_name)
    elif ext in ['.c']:
        target_name += '.o'
        output = '{}: {}\n\t'.format(target_name, path.strip())
        output += '$(CC) $(CFLAGS) -c {} -o {}'.format(path, target_name)
    elif ext in ['.cc', '.cpp', '.cxx']:
        target_name += '.o'
        output = '{}: {}\n\t'.format(target_name, path.strip())
        output += '$(CXX) $(CXXFLAGS) -c {}'.format(path)
    elif ext in ['.csv']:
        # String
        target_name += '.h'
        output = '{}: {}\n\t'.format(target_name, path.strip())
        output += 'python3 poke2c.py {}'.format(path)
    else:
        raise ValueError(path, ext)

    output += '\n'

    return target_name, output

def makefile(toolchain, rom_path, offset=0x800000):
    with open('targets') as file:
        lines = [line.strip() for line in file]
        targets = dict(target(line) for line in lines)
        order = [target(line)[0] for line in lines]

    with open('linker.lsc', 'w') as file:
        file.write(linker_script(0x8000000 + offset))

    with open(rom_path, 'rb') as file:
        file.seek(0xAC)
        gamecode = file.read(4).decode()
        file.seek(0xBC)
        version = int(file.read(1)[0])

    target_strings = ' '.join(item for item in order if os.path.splitext(item)[1] == '.o')
    target_strings_all = ' '.join(order)

    output = """
all: {targets_all}
\t$(LD) $(LDFLAGS) -o a.o -T {linker_script} -T {symbols} {targets}
\t$(OBJCOPY) -O binary a.o a.bin

""".format(targets=target_strings, targets_all=target_strings_all, linker_script='linker.lsc', symbols=gamecode + '.sym')

    defines = """CC={toolchain[cc]}
CXX={toolchain[cxx]}
AS={toolchain[as]}
LD={toolchain[ld]}
OBJCOPY={toolchain[objcopy]}
OBJDUMP={toolchain[objdump]}
XXD=xxd
IMG=todo
COMPRESS=toolchain/gbalzss
PADBIN=toolchain/padbin

OPTS=-fauto-inc-dec -fcompare-elim -fcprop-registers -fdce -fdefer-pop -fdse -fguess-branch-probability -fif-conversion2 -fif-conversion -fipa-pure-const -fipa-profile -fipa-reference -fmerge-constants -fsplit-wide-types -ftree-bit-ccp -ftree-builtin-call-dce -ftree-ccp -ftree-ch -ftree-copyrename -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre -ftree-phiprop -ftree-sra -ftree-pta -ftree-ter -funit-at-a-time -fomit-frame-pointer -fthread-jumps -falign-functions -falign-jumps -falign-loops  -falign-labels -fcaller-saves -fcrossjumping -fcse-follow-jumps  -fcse-skip-blocks -fdelete-null-pointer-checks -fdevirtualize -fexpensive-optimizations -fgcse -fgcse-lm -finline-small-functions -findirect-inlining -fipa-sra -foptimize-sibling-calls -fpartial-inlining -fpeephole2 -fregmove -freorder-blocks -freorder-functions -frerun-cse-after-loop -fsched-interblock -fsched-spec -fschedule-insns -fschedule-insns2 -fstrict-aliasing -fstrict-overflow -ftree-switch-conversion -ftree-tail-merge -ftree-pre -ftree-vrp -finline-functions -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-slp-vectorize -fvect-cost-model -fipa-cp-clone -ffast-math -fforward-propagate -finline-functions-called-once -fmerge-all-constants -fmodulo-sched -fmodulo-sched-allow-regmoves -fgcse-sm -fgcse-las -funsafe-loop-optimizations -fconserve-stack
DEFINES=-D{gamecode} -DSOFTWARE_VERSION={version}
CFLAGS=-mthumb -mthumb-interwork -mcpu=arm7tdmi $(OPTS) -mlong-calls -march=armv4t -Wall -O3 $(DEFINES)
CXXFLAGS=-mthumb -mthumb-interwork -mcpu=arm7tdmi $(OPTS) -mlong-calls -march=armv4t -Wall -O3 $(DEFINES)
ASFLAGS=-mthumb
LDFLAGS=
""".format(toolchain=toolchain, gamecode=gamecode, version=version)
    body = '\n'.join(targets.values())

    clean = """
clean:
\trm -rf {targets} a.o a.bin
""".format(targets=target_strings_all)

    insert = """
insert:
\tdd conv=notrunc of={rom} if={bin} bs=1 seek={offset}
""".format(offset=offset, rom=rom_path, bin='a.bin')

    with open('makefile', 'w') as file:
        print(defines + output + body + clean + insert, file=file)

toolchain = {
             'cc': 'arm-linux-gnueabi-gcc-4.7',
             'cxx': 'arm-linux-gnueabi-gcc-4.7',
             'as': 'arm-linux-gnueabi-as',
             'ld': 'arm-linux-gnueabi-ld',
             'objcopy': 'arm-linux-gnueabi-objcopy',
             'objdump': 'arm-linux-gnueabi-objcopy'
             }
makefile(toolchain, 'test.gba')

