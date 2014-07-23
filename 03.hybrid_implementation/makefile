mpicc -c matrix.c
mpicc -c warray.c
mpicc -c -fopenmp algorithm.c
mpicc -fopenmp driver.c algorithm.o matrix.o warray.o -o driver
mpirun -np 5 ./driver 4039 12 dat1_4039.txt -host 172.18.4.120 172.18.4.118 172.18.4.127 172.18.4.43 172.18.4.107 > out1.txt
