Release: city.o cJSON.o config.o file.o geolocation.o gui.o lang.o main.o prayer_times.o prayer_times_calc.o prayer_times_diyanet.o Salah_times_calc.o socket.o update.o
	gcc -Wall -std=c11 -o ./bin/Release/Prayer_Times.exe ./obj/Release/city.o ./obj/Release/cJSON.o ./obj/Release/config.o ./obj/Release/file.o ./obj/Release/geolocation.o ./obj/Release/gui.o ./obj/Release/lang.o ./obj/Release/main.o ./obj/Release/prayer_times.o ./obj/Release/prayer_times_calc.o ./obj/Release/prayer_times_diyanet.o ./obj/Release/Salah_times_calc.o ./obj/Release/socket.o ./obj/Release/update.o `pkg-config gtk+-3.0 --libs` -lws2_32 -lssl -lcrypto
 
Debug: city.d cJSON.d config.d file.d geolocation.d gui.d lang.d main.d prayer_times.d prayer_times_calc.d prayer_times_diyanet.d Salah_times_calc.d socket.d update.d
	gcc -Wall -std=c11 -g -o ./bin/Debug/Prayer_Times.exe ./obj/Debug/city.o ./obj/Debug/cJSON.o ./obj/Debug/config.o ./obj/Debug/file.o ./obj/Debug/geolocation.o ./obj/Debug/gui.o ./obj/Debug/lang.o ./obj/Debug/main.o ./obj/Debug/prayer_times.o ./obj/Debug/prayer_times_calc.o ./obj/Debug/prayer_times_diyanet.o ./obj/Debug/Salah_times_calc.o ./obj/Debug/socket.o ./obj/Debug/update.o `pkg-config gtk+-3.0 --libs` -lws2_32 -lssl -lcrypto
 	
city.d:
	gcc -Wall -std=c11 -c city.c -O0 -o ./obj/Debug/city.o

cJSON.d:
	gcc -Wall -std=c11 -c cJSON.c -O0 -o ./obj/Debug/cJSON.o

config.d:
	gcc -Wall -std=c11 -c config.c -O0 -o ./obj/Debug/config.o

file.d:
	gcc -Wall -std=c11 -c file.c -O0 -o ./obj/Debug/file.o

geolocation.d:
	gcc -Wall -std=c11 -c geolocation.c -O0 -o ./obj/Debug/geolocation.o

lang.d:
	gcc -Wall -std=c11 -c lang.c -O0 -o ./obj/Debug/lang.o

prayer_times.d:
	gcc -Wall -std=c11 -c prayer_times.c -O0 -o ./obj/Debug/prayer_times.o

prayer_times_calc.d:
	gcc -Wall -std=c11 -c prayer_times_calc.c -O0 -o ./obj/Debug/prayer_times_calc.o

prayer_times_diyanet.d:
	gcc -Wall -std=c11 -c prayer_times_diyanet.c -O0 -o ./obj/Debug/prayer_times_diyanet.o

Salah_times_calc.d:
	gcc -Wall -std=c11 -c Salah_times_calc.c -O0 -o ./obj/Debug/Salah_times_calc.o

socket.d:
	gcc -Wall -std=c11 -c socket.c -O0 -o ./obj/Debug/socket.o

update.d:
	gcc -Wall -std=c11 -c update.c -O0 -o ./obj/Debug/update.o
	
gui.d:
	gcc `pkg-config gtk+-3.0 --cflags` -Wall -std=c11 -c gui.c -O0 -o ./obj/Debug/gui.o
	
main.d:
	gcc `pkg-config gtk+-3.0 --cflags` -Wall -std=c11 -c main.c -O0 -o ./obj/Debug/main.o
	
city.o:
	gcc -Wall -std=c11 -O3 -c city.c -O0 -o ./obj/Release/city.o

cJSON.o:
	gcc -Wall -std=c11 -O3 -c cJSON.c -O0 -o ./obj/Release/cJSON.o

config.o:
	gcc -Wall -std=c11 -O3 -c config.c -O0 -o ./obj/Release/config.o

file.o:
	gcc -Wall -std=c11 -O3 -c file.c -O0 -o ./obj/Release/file.o

geolocation.o:
	gcc -Wall -std=c11 -O3 -c geolocation.c -O0 -o ./obj/Release/geolocation.o

lang.o:
	gcc -Wall -std=c11 -O3 -c lang.c -O0 -o ./obj/Release/lang.o

prayer_times.o:
	gcc -Wall -std=c11 -O3 -c prayer_times.c -O0 -o ./obj/Release/prayer_times.o

prayer_times_calc.o:
	gcc -Wall -std=c11 -O3 -c prayer_times_calc.c -O0 -o ./obj/Release/prayer_times_calc.o

prayer_times_diyanet.o:
	gcc -Wall -std=c11 -O3 -c prayer_times_diyanet.c -O0 -o ./obj/Release/prayer_times_diyanet.o

Salah_times_calc.o:
	gcc -Wall -std=c11 -O3 -c Salah_times_calc.c -O0 -o ./obj/Release/Salah_times_calc.o

socket.o:
	gcc -Wall -std=c11 -O3 -c socket.c -O0 -o ./obj/Release/socket.o

update.o:
	gcc -Wall -std=c11 -O3 -c update.c -O0 -o ./obj/Release/update.o
	
gui.o:
	gcc `pkg-config gtk+-3.0 --cflags` -Wall -std=c11 -O3 -c gui.c -O0 -o ./obj/Release/gui.o
	
main.o:
	gcc `pkg-config gtk+-3.0 --cflags` -Wall -std=c11 -O3 -c main.c -O0 -o ./obj/Release/main.o	
	
cleanDebug:
	rm ./obj/Debug/city.o ./obj/Debug/cJSON.o ./obj/Debug/config.o ./obj/Debug/file.o ./obj/Debug/geolocation.o ./obj/Debug/gui.o ./obj/Debug/lang.o ./obj/Debug/main.o ./obj/Debug/prayer_times.o ./obj/Debug/prayer_times_calc.o ./obj/Debug/prayer_times_diyanet.o ./obj/Debug/Salah_times_calc.o ./obj/Debug/socket.o ./obj/Debug/update.o
	
cleanRelease:
	rm ./obj/Release/city.o ./obj/Release/cJSON.o ./obj/Release/config.o ./obj/Release/file.o ./obj/Release/geolocation.o ./obj/Release/gui.o ./obj/Release/lang.o ./obj/Release/main.o ./obj/Release/prayer_times.o ./obj/Release/prayer_times_calc.o ./obj/Release/prayer_times_diyanet.o ./obj/Release/Salah_times_calc.o ./obj/Release/socket.o ./obj/Release/update.o
