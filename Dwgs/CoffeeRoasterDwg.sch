v 20210731 2
C 40000 40000 0 0 0 title-B.sym
C 49200 47800 1 0 0 voltage-3.sym
{
T 49400 48500 5 8 0 0 0 0 1
device=VOLTAGE_SOURCE
T 49500 48300 5 10 0 1 0 0 1
refdes=VCC
T 49800 48300 5 10 1 1 0 0 1
value=5V
T 49400 48700 5 10 0 0 0 0 1
symversion=0.1
}
C 42800 48200 1 0 0 generic-power.sym
{
T 43000 48450 5 10 1 1 0 3 1
net=Vcc:1
}
C 42600 42600 1 0 0 gnd-1.sym
C 43600 45700 1 0 0 connector5-2.sym
{
T 44300 48200 5 10 1 1 0 6 1
refdes=J1
T 43900 48150 5 10 0 0 0 0 1
device=CONNECTOR_5
T 43900 48350 5 10 0 0 0 0 1
footprint=SIP5N
}
C 43600 42900 1 0 0 connector5-2.sym
{
T 44300 45400 5 10 1 1 0 6 1
refdes=J2
T 43900 45350 5 10 0 0 0 0 1
device=CONNECTOR_5
T 43900 45550 5 10 0 0 0 0 1
footprint=SIP5N
}
N 43000 44500 43000 48200 4
N 43000 44500 43700 44500 4
N 42700 42900 42700 47300 4
N 42700 47300 43600 47300 4
T 40200 48000 9 10 1 0 0 0 1
Pin 0 Heater Pwr
T 41200 44200 9 10 1 0 0 0 1
Pin 5 Yellow Clk
N 41600 46500 43600 46500 4
N 43600 47700 43000 47700 4
N 43600 44900 42700 44900 4
T 41100 43400 9 10 1 0 0 0 1
Pin 19 Wht MISO
T 41100 43800 9 10 1 0 0 0 1
Pin 23 Grn MOSI
N 41600 46100 43600 46100 4
N 43600 46900 42100 46900 4
N 41100 44100 43600 44100 4
N 41100 43700 43600 43700 4
N 41100 43300 43600 43300 4
C 40800 46800 1 0 0 input-1.sym
{
T 40800 47100 5 10 0 0 0 0 1
device=INPUT
T 40800 46800 5 10 1 1 0 0 1
pinnumber=15
}
C 40800 46400 1 0 0 input-1.sym
{
T 40800 46700 5 10 0 0 0 0 1
device=INPUT
T 40800 46400 5 10 1 1 0 0 1
pinnumber=2
}
C 40800 46000 1 0 0 input-1.sym
{
T 40800 46300 5 10 0 0 0 0 1
device=INPUT
T 40800 46000 5 10 1 1 0 0 1
pinnumber=18
}
C 40300 44000 1 0 0 input-1.sym
{
T 40300 44300 5 10 0 0 0 0 1
device=INPUT
T 40300 44000 5 10 1 1 0 0 1
pinnumber=5
}
C 40300 43600 1 0 0 input-1.sym
{
T 40300 43900 5 10 0 0 0 0 1
device=INPUT
T 40300 43600 5 10 1 1 0 0 1
pinnumber=23
}
C 40300 43200 1 0 0 input-1.sym
{
T 40300 43500 5 10 0 0 0 0 1
device=INPUT
T 40300 43200 5 10 1 1 0 0 1
pinnumber=19
}
C 45100 45600 1 0 0 connector5-2.sym
{
T 45800 48100 5 10 1 1 0 6 1
refdes=J1
T 45400 48050 5 10 0 0 0 0 1
device=CONNECTOR_5
T 45400 48250 5 10 0 0 0 0 1
footprint=SIP5N
}
B 44400 42900 400 2400 3 10 1 0 -1 -1 0 -1 -1 -1 -1 -1
L 44800 44900 45100 44900 3 10 1 0 -1 -1
L 44800 44500 45100 44500 3 10 1 0 -1 -1
L 44800 44100 45100 44100 3 10 1 0 -1 -1
L 44800 43700 45100 43700 3 10 1 0 -1 -1
L 44800 43300 45100 43300 3 10 1 0 -1 -1
T 44600 44900 9 10 1 0 0 0 1
1
T 44600 44500 9 10 1 0 0 0 1
2
T 44600 44100 9 10 1 0 0 0 1
3
T 44600 43700 9 10 1 0 0 0 1
4