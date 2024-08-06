v 20210731 2
C 40000 40000 0 0 0 title-B.sym
C 45900 48300 1 0 0 generic-power.sym
{
T 46100 48550 5 10 1 1 0 3 1
net=Vcc:1
}
C 46500 42800 1 0 0 gnd-1.sym
C 46900 45800 1 0 0 connector5-2.sym
{
T 47600 48300 5 10 1 1 0 6 1
refdes=J1
T 47200 48250 5 10 0 0 0 0 1
device=CONNECTOR_5
T 47200 48450 5 10 0 0 0 0 1
footprint=SIP5N
}
C 46900 43000 1 0 0 connector5-2.sym
{
T 47500 45500 5 10 1 1 0 6 1
refdes=J2
T 47200 45450 5 10 0 0 0 0 1
device=CONNECTOR_5
T 47200 45650 5 10 0 0 0 0 1
footprint=SIP5N
}
N 46100 44600 46100 48300 4
N 46100 44600 46900 44600 4
N 46600 43100 46600 47400 4
N 46600 47400 46900 47400 4
T 43900 44300 9 10 1 0 0 0 1
Pin 5 Yellow Clk
N 43900 46600 46900 46600 4
N 46900 47800 46100 47800 4
N 46900 45000 46600 45000 4
T 43900 43500 9 10 1 0 0 0 1
Pin 19 Wht MISO
T 43900 43900 9 10 1 0 0 0 1
Pin 23 Green CS
N 43900 46200 46900 46200 4
N 43900 47000 46900 47000 4
N 43900 44200 46900 44200 4
N 43900 43800 46900 43800 4
N 43900 43400 46900 43400 4
C 43100 46900 1 0 0 input-1.sym
{
T 43100 47200 5 10 0 0 0 0 1
device=INPUT
T 43100 46900 5 10 1 1 0 0 1
pinnumber=15
}
C 43100 46500 1 0 0 input-1.sym
{
T 43100 46800 5 10 0 0 0 0 1
device=INPUT
T 43100 46500 5 10 1 1 0 0 1
pinnumber=2
}
C 43100 46100 1 0 0 input-1.sym
{
T 43100 46400 5 10 0 0 0 0 1
device=INPUT
T 43100 46100 5 10 1 1 0 0 1
pinnumber=18
}
C 43100 44100 1 0 0 input-1.sym
{
T 43100 44400 5 10 0 0 0 0 1
device=INPUT
T 43100 44100 5 10 1 1 0 0 1
pinnumber=5
}
C 43100 43700 1 0 0 input-1.sym
{
T 43100 44000 5 10 0 0 0 0 1
device=INPUT
T 43100 43700 5 10 1 1 0 0 1
pinnumber=23
}
C 43100 43300 1 0 0 input-1.sym
{
T 43100 43600 5 10 0 0 0 0 1
device=INPUT
T 43100 43300 5 10 1 1 0 0 1
pinnumber=19
}
B 47800 43000 400 2400 3 10 1 0 -1 -1 0 -1 -1 -1 -1 -1
{
T 47900 45500 5 10 1 1 0 0 1
refdes=P2
T 48000 45000 5 10 1 1 0 0 1
pinnumber=1
T 48000 44200 5 10 1 1 0 0 1
pinnumber=3
T 48000 43800 5 10 1 1 0 0 1
pinnumber=4
T 48000 43400 5 10 1 1 0 0 1
pinnumber=5
T 48000 44600 5 10 1 1 0 0 1
pinnumber=2
}
T 40000 40000 8 10 1 1 0 0 1
pinnumber=3
B 47800 45800 400 2400 3 10 1 0 -1 -1 0 -1 -1 -1 -1 -1
{
T 47900 48300 5 10 1 1 0 0 1
refdes=P1
T 48000 47800 5 10 1 1 0 0 1
pinnumber=1
T 48000 47000 5 10 1 1 0 0 1
pinnumber=3
T 48000 46600 5 10 1 1 0 0 1
pinnumber=4
T 48000 46200 5 10 1 1 0 0 1
pinnumber=5
T 48000 47400 5 10 1 1 0 0 1
pinnumber=2
}
C 48200 45000 1 0 0 out-1.sym
{
T 48200 45300 5 10 0 0 0 0 1
device=OUTPUT
T 48200 45300 5 10 1 1 0 0 1
refdes=Blue
}
C 48200 44600 1 0 0 out-1.sym
{
T 48200 44900 5 10 0 0 0 0 1
device=OUTPUT
T 48200 44900 5 10 1 1 0 0 1
refdes=Purple
}
C 48200 44200 1 0 0 out-1.sym
{
T 48200 44500 5 10 0 0 0 0 1
device=OUTPUT
T 48400 44500 5 10 1 1 0 0 1
refdes=Grey
}
C 48200 43800 1 0 0 out-1.sym
{
T 48200 44100 5 10 0 0 0 0 1
device=OUTPUT
T 48200 44100 5 10 1 1 0 0 1
refdes=White
}
C 48200 43400 1 0 0 out-1.sym
{
T 48200 43700 5 10 0 0 0 0 1
device=OUTPUT
T 48200 43700 5 10 1 1 0 0 1
refdes=Blk
}
T 49900 44400 9 10 1 0 0 0 1
To Thermocouple
T 48400 47100 9 10 1 0 0 0 1
To Page 2
T 43900 47100 9 10 1 0 0 0 1
Servo Motor Org
T 43900 46700 9 10 1 0 0 0 1
Mixer Power Green
T 43900 46300 9 10 1 0 0 0 1
Mixer Dir
T 50100 41000 9 10 1 0 0 0 1
ESP32 I/O Pins