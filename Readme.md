# OPUS demo
OPUS is a audio format which is featuring high quality of audio or voice codec.

Connection:
This program is using PWM for sound output. I don't have any I2S devices which is using in MaixPy.
```
22 -- L ch
23 -- R ch
```
you may use 2.2kΩ register and coupling capacitor to drive your head phone.

Demo music: [PeriTune Sakuya3](https://www.youtube.com/watch?v=sDyNzjrA9dg)

You may use other music by using opus-demo for decoding and convert your outputfile using xxd command. (Not ogg container; sorry)
```bash
$ ./opus_demo -e voip 48000 2 64000 -framesize 20 test.raw test.opus && ./opus_demo -d 48000 2 test.opus test2.raw && aplay test2.raw -r 44100 -c2 -f S16_LE
```
