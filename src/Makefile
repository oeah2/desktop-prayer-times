release: Release

debug: Debug

Release: city.o cJSON.o config.o file.o geolocation.o gui.o lang.o main.o prayer_times.o prayer_times_calc.o prayer_times_diyanet.o Salah_times_calc.o socket.o update.o
	gcc -Wall -std=c11 -O3 -o ./bin/Release/Prayer_Times.exe ./obj/Release/city.o ./obj/Release/cJSON.o ./obj/Release/config.o ./obj/Release/file.o ./obj/Release/geolocation.o ./obj/Release/gui.o ./obj/Release/lang.o ./obj/Release/main.o ./obj/Release/prayer_times.o ./obj/Release/prayer_times_calc.o ./obj/Release/prayer_times_diyanet.o ./obj/Release/Salah_times_calc.o ./obj/Release/socket.o ./obj/Release/update.o `pkg-config gtk+-3.0 --libs` -lws2_32 -lssl -lcrypto
 
Debug: ./obj/Debug/city.o ./obj/Debug/cJSON.o ./obj/Debug/config.o ./obj/Debug/file.o ./obj/Debug/geolocation.o ./obj/Debug/gui.o ./obj/Debug/lang.o ./obj/Debug/main.o ./obj/Debug/prayer_times.o ./obj/Debug/prayer_times_calc.o ./obj/Debug/prayer_times_diyanet.o ./obj/Debug/Salah_times_calc.o ./obj/Debug/socket.o ./obj/Debug/update.o
	gcc -Wall -std=c11 -g -o ./bin/Debug/Prayer_Times.exe ./obj/Debug/city.o ./obj/Debug/cJSON.o ./obj/Debug/config.o ./obj/Debug/file.o ./obj/Debug/geolocation.o ./obj/Debug/gui.o ./obj/Debug/lang.o ./obj/Debug/main.o ./obj/Debug/prayer_times.o ./obj/Debug/prayer_times_calc.o ./obj/Debug/prayer_times_diyanet.o ./obj/Debug/Salah_times_calc.o ./obj/Debug/socket.o ./obj/Debug/update.o `pkg-config gtk+-3.0 --libs` -lws2_32 -lssl -lcrypto
 	
./obj/Debug/city.o:
	gcc -Wall -std=c11 -g -c city.c -O0 -o ./obj/Debug/city.o

./obj/Debug/cJSON.o:
	gcc -Wall -std=c11 -g -c cJSON.c -O0 -o ./obj/Debug/cJSON.o

./obj/Debug/config.o:
	gcc -Wall -std=c11 -g -c config.c -O0 -o ./obj/Debug/config.o

./obj/Debug/file.o:
	gcc -Wall -std=c11 -g -c file.c -O0 -o ./obj/Debug/file.o

./obj/Debug/geolocation.o:
	gcc -Wall -std=c11 -g -c geolocation.c -O0 -o ./obj/Debug/geolocation.o

./obj/Debug/lang.o:
	gcc -Wall -std=c11 -g -c lang.c -O0 -o ./obj/Debug/lang.o

./obj/Debug/prayer_times.o:
	gcc -Wall -std=c11 -g -c prayer_times.c -O0 -o ./obj/Debug/prayer_times.o

./obj/Debug/prayer_times_calc.o:
	gcc -Wall -std=c11 -g -c prayer_times_calc.c -O0 -o ./obj/Debug/prayer_times_calc.o

./obj/Debug/prayer_times_diyanet.o:
	gcc -Wall -std=c11 -g -c prayer_times_diyanet.c -O0 -o ./obj/Debug/prayer_times_diyanet.o

./obj/Debug/Salah_times_calc.o:
	gcc -Wall -std=c11 -g -c Salah_times_calc.c -O0 -o ./obj/Debug/Salah_times_calc.o

./obj/Debug/socket.o:
	gcc -Wall -std=c11 -g -c socket.c -O0 -o ./obj/Debug/socket.o

./obj/Debug/update.o:
	gcc -Wall -std=c11 -g -c update.c -O0 -o ./obj/Debug/update.o
	
./obj/Debug/gui.o:
	gcc `pkg-config gtk+-3.0 --cflags` -Wall -std=c11 -g -c gui.c -O0 -o ./obj/Debug/gui.o
	
./obj/Debug/main.o:
	gcc `pkg-config gtk+-3.0 --cflags` -Wall -std=c11 -g -c main.c -O0 -o ./obj/Debug/main.o
	
city.o:
	gcc -Wall -std=c11 -O3 -c city.c -o ./obj/Release/city.o

cJSON.o:
	gcc -Wall -std=c11 -O3 -c cJSON.c -o ./obj/Release/cJSON.o

config.o:
	gcc -Wall -std=c11 -O3 -c config.c -o ./obj/Release/config.o

file.o:
	gcc -Wall -std=c11 -O3 -c file.c -o ./obj/Release/file.o

geolocation.o:
	gcc -Wall -std=c11 -O3 -c geolocation.c -o ./obj/Release/geolocation.o

lang.o:
	gcc -Wall -std=c11 -O3 -c lang.c -o ./obj/Release/lang.o

prayer_times.o:
	gcc -Wall -std=c11 -O3 -c prayer_times.c -o ./obj/Release/prayer_times.o

prayer_times_calc.o:
	gcc -Wall -std=c11 -O3 -c prayer_times_calc.c -o ./obj/Release/prayer_times_calc.o

prayer_times_diyanet.o:
	gcc -Wall -std=c11 -O3 -c prayer_times_diyanet.c -o ./obj/Release/prayer_times_diyanet.o

Salah_times_calc.o:
	gcc -Wall -std=c11 -O3 -c Salah_times_calc.c -o ./obj/Release/Salah_times_calc.o

socket.o:
	gcc -Wall -std=c11 -O3 -c socket.c -o ./obj/Release/socket.o

update.o:
	gcc -Wall -std=c11 -O3 -c update.c -o ./obj/Release/update.o
	
gui.o:
	gcc `pkg-config gtk+-3.0 --cflags` -Wall -std=c11 -O3 -c gui.c -o ./obj/Release/gui.o
	
main.o:
	gcc `pkg-config gtk+-3.0 --cflags` -Wall -std=c11 -O3 -c main.c -o ./obj/Release/main.o	
	
cleanDebug:
	rm ./obj/Debug/city.o ./obj/Debug/cJSON.o ./obj/Debug/config.o ./obj/Debug/file.o ./obj/Debug/geolocation.o ./obj/Debug/gui.o ./obj/Debug/lang.o ./obj/Debug/main.o ./obj/Debug/prayer_times.o ./obj/Debug/prayer_times_calc.o ./obj/Debug/prayer_times_diyanet.o ./obj/Debug/Salah_times_calc.o ./obj/Debug/socket.o ./obj/Debug/update.o
	
cleanRelease:
	rm ./obj/Release/city.o ./obj/Release/cJSON.o ./obj/Release/config.o ./obj/Release/file.o ./obj/Release/geolocation.o ./obj/Release/gui.o ./obj/Release/lang.o ./obj/Release/main.o ./obj/Release/prayer_times.o ./obj/Release/prayer_times_calc.o ./obj/Release/prayer_times_diyanet.o ./obj/Release/Salah_times_calc.o ./obj/Release/socket.o ./obj/Release/update.o
