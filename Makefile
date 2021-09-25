OUT_RELEASE	= ./bin/Release/Prayer_times.exe
OUT_DEBUG = ./bin/Debug/Prayer_Times.exe
OBJ_DEBUG_PATH = ./obj/Debug
OBJ_RELEASE_PATH = ./obj/Release
SRC_PATH = ./src
CFLAGS_DEBUG = -Wall -std=c11 -g -O0
CLAGS_RELEASE = -Wall -std=c11 -O3

release: Release

debug: Debug

Release: $(OBJ_RELEASE_PATH)/city.o $(OBJ_RELEASE_PATH)/cJSON.o $(OBJ_RELEASE_PATH)/config.o $(OBJ_RELEASE_PATH)/file.o $(OBJ_RELEASE_PATH)/geolocation.o $(OBJ_RELEASE_PATH)/gui.o $(OBJ_RELEASE_PATH)/lang.o $(OBJ_RELEASE_PATH)/main.o $(OBJ_RELEASE_PATH)/prayer_times.o $(OBJ_RELEASE_PATH)/prayer_times_calc.o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o $(OBJ_RELEASE_PATH)/Salah_times_calc.o $(OBJ_RELEASE_PATH)/socket.o $(OBJ_RELEASE_PATH)/update.o ICON
	gcc $(CLAGS_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE_PATH)/city.o $(OBJ_RELEASE_PATH)/cJSON.o $(OBJ_RELEASE_PATH)/config.o $(OBJ_RELEASE_PATH)/file.o $(OBJ_RELEASE_PATH)/geolocation.o $(OBJ_RELEASE_PATH)/gui.o $(OBJ_RELEASE_PATH)/lang.o $(OBJ_RELEASE_PATH)/main.o $(OBJ_RELEASE_PATH)/prayer_times.o $(OBJ_RELEASE_PATH)/prayer_times_calc.o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o $(OBJ_RELEASE_PATH)/Salah_times_calc.o $(OBJ_RELEASE_PATH)/socket.o $(OBJ_RELEASE_PATH)/update.o icon.res `pkg-config gtk+-3.0 --libs` -lws2_32 -lssl -lcrypto
 
Debug: $(OBJ_DEBUG_PATH)/city.o $(OBJ_DEBUG_PATH)/cJSON.o $(OBJ_DEBUG_PATH)/config.o $(OBJ_DEBUG_PATH)/file.o $(OBJ_DEBUG_PATH)/geolocation.o $(OBJ_DEBUG_PATH)/gui.o $(OBJ_DEBUG_PATH)/lang.o $(OBJ_DEBUG_PATH)/main.o $(OBJ_DEBUG_PATH)/prayer_times.o $(OBJ_DEBUG_PATH)/prayer_times_calc.o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o $(OBJ_DEBUG_PATH)/Salah_times_calc.o $(OBJ_DEBUG_PATH)/socket.o $(OBJ_DEBUG_PATH)/update.o ICON
	gcc $(CFLAGS_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG_PATH)/city.o $(OBJ_DEBUG_PATH)/cJSON.o $(OBJ_DEBUG_PATH)/config.o $(OBJ_DEBUG_PATH)/file.o $(OBJ_DEBUG_PATH)/geolocation.o $(OBJ_DEBUG_PATH)/gui.o $(OBJ_DEBUG_PATH)/lang.o $(OBJ_DEBUG_PATH)/main.o $(OBJ_DEBUG_PATH)/prayer_times.o $(OBJ_DEBUG_PATH)/prayer_times_calc.o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o $(OBJ_DEBUG_PATH)/Salah_times_calc.o $(OBJ_DEBUG_PATH)/socket.o $(OBJ_DEBUG_PATH)/update.o icon.res `pkg-config gtk+-3.0 --libs` -lws2_32 -lssl -lcrypto

ICON: icon.rc
	windres icon.rc -O coff -o icon.res
 
$(OBJ_DEBUG_PATH)/city.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/city.c -o $(OBJ_DEBUG_PATH)/city.o

$(OBJ_DEBUG_PATH)/cJSON.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/cJSON.c -o $(OBJ_DEBUG_PATH)/cJSON.o

$(OBJ_DEBUG_PATH)/config.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/config.c -o $(OBJ_DEBUG_PATH)/config.o

$(OBJ_DEBUG_PATH)/file.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/file.c -o $(OBJ_DEBUG_PATH)/file.o

$(OBJ_DEBUG_PATH)/geolocation.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/geolocation.c -o $(OBJ_DEBUG_PATH)/geolocation.o

$(OBJ_DEBUG_PATH)/lang.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/lang.c -o $(OBJ_DEBUG_PATH)/lang.o

$(OBJ_DEBUG_PATH)/prayer_times.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/prayer_times.c -o $(OBJ_DEBUG_PATH)/prayer_times.o

$(OBJ_DEBUG_PATH)/prayer_times_calc.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/prayer_times_calc.c -o $(OBJ_DEBUG_PATH)/prayer_times_calc.o

$(OBJ_DEBUG_PATH)/prayer_times_diyanet.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/prayer_times_diyanet.c -o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o

$(OBJ_DEBUG_PATH)/Salah_times_calc.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/Salah_times_calc.c -o $(OBJ_DEBUG_PATH)/Salah_times_calc.o

$(OBJ_DEBUG_PATH)/socket.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/socket.c -o $(OBJ_DEBUG_PATH)/socket.o

$(OBJ_DEBUG_PATH)/update.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/update.c -o $(OBJ_DEBUG_PATH)/update.o
	
$(OBJ_DEBUG_PATH)/gui.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/gui.c -o $(OBJ_DEBUG_PATH)/gui.o
	
$(OBJ_DEBUG_PATH)/main.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/main.c -o $(OBJ_DEBUG_PATH)/main.o
	
$(OBJ_RELEASE_PATH)/city.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/city.c -o $(OBJ_RELEASE_PATH)/city.o

$(OBJ_RELEASE_PATH)/cJSON.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/cJSON.c -o $(OBJ_RELEASE_PATH)/cJSON.o

$(OBJ_RELEASE_PATH)/config.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/config.c -o $(OBJ_RELEASE_PATH)/config.o

$(OBJ_RELEASE_PATH)/file.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/file.c -o $(OBJ_RELEASE_PATH)/file.o

$(OBJ_RELEASE_PATH)/geolocation.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/geolocation.c -o $(OBJ_RELEASE_PATH)/geolocation.o

$(OBJ_RELEASE_PATH)/lang.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/lang.c -o $(OBJ_RELEASE_PATH)/lang.o

$(OBJ_RELEASE_PATH)/prayer_times.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/prayer_times.c -o $(OBJ_RELEASE_PATH)/prayer_times.o

$(OBJ_RELEASE_PATH)/prayer_times_calc.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/prayer_times_calc.c -o $(OBJ_RELEASE_PATH)/prayer_times_calc.o

$(OBJ_RELEASE_PATH)/prayer_times_diyanet.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/prayer_times_diyanet.c -o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o

$(OBJ_RELEASE_PATH)/Salah_times_calc.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/Salah_times_calc.c -o $(OBJ_RELEASE_PATH)/Salah_times_calc.o

$(OBJ_RELEASE_PATH)/socket.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/socket.c -o $(OBJ_RELEASE_PATH)/socket.o

$(OBJ_RELEASE_PATH)/update.o:
	gcc $(CLAGS_RELEASE) -c $(SRC_PATH)/update.c -o $(OBJ_RELEASE_PATH)/update.o
	
$(OBJ_RELEASE_PATH)/gui.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CLAGS_RELEASE) -c $(SRC_PATH)/gui.c -o $(OBJ_RELEASE_PATH)/gui.o
	
$(OBJ_RELEASE_PATH)/main.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CLAGS_RELEASE) -c $(SRC_PATH)/main.c -o $(OBJ_RELEASE_PATH)/main.o	
	
cleanDebug:
	rm $(OBJ_DEBUG_PATH)/city.o $(OBJ_DEBUG_PATH)/cJSON.o $(OBJ_DEBUG_PATH)/config.o $(OBJ_DEBUG_PATH)/file.o $(OBJ_DEBUG_PATH)/geolocation.o $(OBJ_DEBUG_PATH)/gui.o $(OBJ_DEBUG_PATH)/lang.o $(OBJ_DEBUG_PATH)/main.o $(OBJ_DEBUG_PATH)/prayer_times.o $(OBJ_DEBUG_PATH)/prayer_times_calc.o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o $(OBJ_DEBUG_PATH)/Salah_times_calc.o $(OBJ_DEBUG_PATH)/socket.o $(OBJ_DEBUG_PATH)/update.o
	
cleanRelease:
	rm $(OBJ_RELEASE_PATH)/city.o $(OBJ_RELEASE_PATH)/cJSON.o $(OBJ_RELEASE_PATH)/config.o $(OBJ_RELEASE_PATH)/file.o $(OBJ_RELEASE_PATH)/geolocation.o $(OBJ_RELEASE_PATH)/gui.o $(OBJ_RELEASE_PATH)/lang.o $(OBJ_RELEASE_PATH)/main.o $(OBJ_RELEASE_PATH)/prayer_times.o $(OBJ_RELEASE_PATH)/prayer_times_calc.o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o $(OBJ_RELEASE_PATH)/Salah_times_calc.o $(OBJ_RELEASE_PATH)/socket.o $(OBJ_RELEASE_PATH)/update.o

run: $(OUT_RELEASE)
	$(OUT_RELEASE)

debug: $(OUT_DEBUG)
	$(OUT_DEBUG)

valgrind: $(OUT_DEBUG)
	valgrind $(OUT_DEBUG)

valgrind_leakcheck: $(OUT_DEBUG)
	valgrind --leak-check=full $(OUT_DEBUG)

valgrind_extreme: $(OUT_DEBUG)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes $(OUT_DEBUG)