# OPUS demo
OPUS is a audio format which is featuring high quality of audio or voice codec.

## Connection
This program is using PWM for sound output. I don't have any I2S devices which is using in MaixPy.
```
22 -- L ch
23 -- R ch
```
you may use 2.2kΩ register and coupling capacitor to drive your head phone.

## Music from
Demo music: [PeriTune Sakuya3](https://www.youtube.com/watch?v=sDyNzjrA9dg)

You may use other music by using opus-demo for decoding and convert your outputfile using xxd command. (Not ogg container; sorry)

## opus codec demo format
you can use libopus demo program
```bash
$ ./opus_demo -e voip 48000 2 64000 -framesize 20 test.raw test.opus && ./opus_demo -d 48000 2 test.opus test2.raw && aplay test2.raw -r 44100 -c2 -f S16_LE
```

convert into .c file
```
xxd -i [filename] > [outputfile]
```

## make libopus.a
libopus: https://github.com/xiph/opus
(tested :ad8fe90db79b7d2a135e3dfd2ed6631b0c5662ab)

host is mips but this build is for any archtecture and overritten by CC=`riscv64-unknown-elf-gcc` 
```sh
$ ./configure --host=mips --enable-fixed-point --disable-extra-programs CC=riscv64-unknown-elf-gcc CFLAGS=" -mcmodel=medany -fno-common -ffunction-sections -fdata-sections -fno-exceptions -fstrict-volatile-bitfields -fno-zero-initialized-in-bss -Os"
$ make
```
you can see the archive in the `.lib/` directory.
