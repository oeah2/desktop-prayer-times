OUT_RELEASE	= ./bin/Release/Prayer_times.exe
OUT_RELEASE_LINUX = ./bin/Release/Prayer_times
OUT_DEBUG = ./bin/Debug/Prayer_Times.exe
OUT_DEBUG_LINUX = ./bin/Debug/Prayer_Times.a
OBJ_DEBUG_PATH = ./obj/Debug
OBJ_RELEASE_PATH = ./obj/Release
SRC_PATH = ./src
CFLAGS_DEBUG = -Wall -std=c11 -g -O0
#CFLAGS_DEBUG += -fsanitize=address
CFLAGS_RELEASE = -Wall -std=c11 -O3 

all: DebugLinux

release: Release

$(OUT_RELEASE): Release

$(OUT_DEBUG): Debug

Release: $(OBJ_RELEASE_PATH)/city.o $(OBJ_RELEASE_PATH)/cJSON.o $(OBJ_RELEASE_PATH)/config.o $(OBJ_RELEASE_PATH)/file.o $(OBJ_RELEASE_PATH)/geolocation.o $(OBJ_RELEASE_PATH)/gui.o $(OBJ_RELEASE_PATH)/gui_assistant.o $(OBJ_RELEASE_PATH)/gui_dlg_settings.o $(OBJ_RELEASE_PATH)/gui_general.o $(OBJ_RELEASE_PATH)/gui_listbox.o $(OBJ_RELEASE_PATH)/gui_menuitm.o $(OBJ_RELEASE_PATH)/lang.o $(OBJ_RELEASE_PATH)/main.o $(OBJ_RELEASE_PATH)/prayer_times.o $(OBJ_RELEASE_PATH)/prayer_times_calc.o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o $(OBJ_RELEASE_PATH)/Salah_times_calc.o $(OBJ_RELEASE_PATH)/socket.o $(OBJ_RELEASE_PATH)/update.o $(OBJ_RELEASE_PATH)/error.o $(OBJ_RELEASE_PATH)/hadith.o $(OBJ_RELEASE_PATH)/hadith_key.o ./Graphics/icon.res
	gcc $(CFLAGS_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE_PATH)/city.o $(OBJ_RELEASE_PATH)/cJSON.o $(OBJ_RELEASE_PATH)/config.o $(OBJ_RELEASE_PATH)/file.o $(OBJ_RELEASE_PATH)/geolocation.o $(OBJ_RELEASE_PATH)/gui.o $(OBJ_RELEASE_PATH)/gui_assistant.o $(OBJ_RELEASE_PATH)/gui_dlg_settings.o $(OBJ_RELEASE_PATH)/gui_general.o $(OBJ_RELEASE_PATH)/gui_listbox.o $(OBJ_RELEASE_PATH)/gui_menuitm.o $(OBJ_RELEASE_PATH)/lang.o $(OBJ_RELEASE_PATH)/main.o $(OBJ_RELEASE_PATH)/prayer_times.o $(OBJ_RELEASE_PATH)/prayer_times_calc.o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o $(OBJ_RELEASE_PATH)/Salah_times_calc.o $(OBJ_RELEASE_PATH)/socket.o $(OBJ_RELEASE_PATH)/update.o $(OBJ_RELEASE_PATH)/error.o $(OBJ_RELEASE_PATH)/hadith.o $(OBJ_RELEASE_PATH)/hadith_key.o ./Graphics/icon.res `pkg-config gtk+-3.0 --libs` -mwindows -lws2_32 -lssl -lcrypto -lm
	
ReleaseLinux: $(OBJ_RELEASE_PATH)/city.o $(OBJ_RELEASE_PATH)/cJSON.o $(OBJ_RELEASE_PATH)/config.o $(OBJ_RELEASE_PATH)/file.o $(OBJ_RELEASE_PATH)/geolocation.o $(OBJ_RELEASE_PATH)/gui.o $(OBJ_RELEASE_PATH)/gui_assistant.o $(OBJ_RELEASE_PATH)/gui_dlg_settings.o $(OBJ_RELEASE_PATH)/gui_general.o $(OBJ_RELEASE_PATH)/gui_listbox.o $(OBJ_RELEASE_PATH)/gui_menuitm.o $(OBJ_RELEASE_PATH)/lang.o $(OBJ_RELEASE_PATH)/main.o $(OBJ_RELEASE_PATH)/prayer_times.o $(OBJ_RELEASE_PATH)/prayer_times_calc.o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o $(OBJ_RELEASE_PATH)/Salah_times_calc.o $(OBJ_RELEASE_PATH)/socket.o $(OBJ_RELEASE_PATH)/update.o $(OBJ_RELEASE_PATH)/error.o $(OBJ_RELEASE_PATH)/hadith.o $(OBJ_RELEASE_PATH)/hadith_key.o
	gcc $(CFLAGS_RELEASE) -o $(OUT_RELEASE_LINUX) $(OBJ_RELEASE_PATH)/city.o $(OBJ_RELEASE_PATH)/cJSON.o $(OBJ_RELEASE_PATH)/config.o $(OBJ_RELEASE_PATH)/file.o $(OBJ_RELEASE_PATH)/geolocation.o $(OBJ_RELEASE_PATH)/gui.o $(OBJ_RELEASE_PATH)/gui_assistant.o $(OBJ_RELEASE_PATH)/gui_dlg_settings.o $(OBJ_RELEASE_PATH)/gui_general.o $(OBJ_RELEASE_PATH)/gui_listbox.o $(OBJ_RELEASE_PATH)/gui_menuitm.o $(OBJ_RELEASE_PATH)/lang.o $(OBJ_RELEASE_PATH)/main.o $(OBJ_RELEASE_PATH)/prayer_times.o $(OBJ_RELEASE_PATH)/prayer_times_calc.o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o $(OBJ_RELEASE_PATH)/Salah_times_calc.o $(OBJ_RELEASE_PATH)/socket.o $(OBJ_RELEASE_PATH)/update.o $(OBJ_RELEASE_PATH)/error.o $(OBJ_RELEASE_PATH)/hadith.o $(OBJ_RELEASE_PATH)/hadith_key.o `pkg-config gtk+-3.0 --libs` -lssl -lcrypto -lm
 
Debug: $(OBJ_DEBUG_PATH)/city.o $(OBJ_DEBUG_PATH)/cJSON.o $(OBJ_DEBUG_PATH)/config.o $(OBJ_DEBUG_PATH)/file.o $(OBJ_DEBUG_PATH)/geolocation.o $(OBJ_DEBUG_PATH)/gui.o $(OBJ_DEBUG_PATH)/gui_assistant.o $(OBJ_DEBUG_PATH)/gui_dlg_settings.o $(OBJ_DEBUG_PATH)/gui_general.o $(OBJ_DEBUG_PATH)/gui_listbox.o $(OBJ_DEBUG_PATH)/gui_menuitm.o $(OBJ_DEBUG_PATH)/lang.o $(OBJ_DEBUG_PATH)/main.o $(OBJ_DEBUG_PATH)/prayer_times.o $(OBJ_DEBUG_PATH)/prayer_times_calc.o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o $(OBJ_DEBUG_PATH)/Salah_times_calc.o $(OBJ_DEBUG_PATH)/socket.o $(OBJ_DEBUG_PATH)/update.o $(OBJ_DEBUG_PATH)/error.o $(OBJ_DEBUG_PATH)/hadith.o $(OBJ_DEBUG_PATH)/hadith_key.o ./Graphics/icon.res
	gcc $(CFLAGS_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG_PATH)/city.o $(OBJ_DEBUG_PATH)/cJSON.o $(OBJ_DEBUG_PATH)/config.o $(OBJ_DEBUG_PATH)/file.o $(OBJ_DEBUG_PATH)/geolocation.o $(OBJ_DEBUG_PATH)/gui.o $(OBJ_DEBUG_PATH)/gui_assistant.o $(OBJ_DEBUG_PATH)/gui_dlg_settings.o $(OBJ_DEBUG_PATH)/gui_general.o $(OBJ_DEBUG_PATH)/gui_listbox.o $(OBJ_DEBUG_PATH)/gui_menuitm.o $(OBJ_DEBUG_PATH)/lang.o $(OBJ_DEBUG_PATH)/main.o $(OBJ_DEBUG_PATH)/prayer_times.o $(OBJ_DEBUG_PATH)/prayer_times_calc.o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o $(OBJ_DEBUG_PATH)/Salah_times_calc.o $(OBJ_DEBUG_PATH)/socket.o $(OBJ_DEBUG_PATH)/update.o $(OBJ_DEBUG_PATH)/error.o $(OBJ_DEBUG_PATH)/hadith.o $(OBJ_DEBUG_PATH)/hadith_key.o ./Graphics/icon.res `pkg-config gtk+-3.0 --libs` -lws2_32 -lssl -lcrypto -lm

DebugLinux: $(OBJ_DEBUG_PATH)/city.o $(OBJ_DEBUG_PATH)/cJSON.o $(OBJ_DEBUG_PATH)/config.o $(OBJ_DEBUG_PATH)/file.o $(OBJ_DEBUG_PATH)/geolocation.o $(OBJ_DEBUG_PATH)/gui.o $(OBJ_DEBUG_PATH)/gui_assistant.o $(OBJ_DEBUG_PATH)/gui_dlg_settings.o $(OBJ_DEBUG_PATH)/gui_general.o $(OBJ_DEBUG_PATH)/gui_listbox.o $(OBJ_DEBUG_PATH)/gui_menuitm.o $(OBJ_DEBUG_PATH)/lang.o $(OBJ_DEBUG_PATH)/main.o $(OBJ_DEBUG_PATH)/prayer_times.o $(OBJ_DEBUG_PATH)/prayer_times_calc.o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o $(OBJ_DEBUG_PATH)/Salah_times_calc.o $(OBJ_DEBUG_PATH)/socket.o $(OBJ_DEBUG_PATH)/update.o $(OBJ_DEBUG_PATH)/error.o $(OBJ_DEBUG_PATH)/hadith.o $(OBJ_DEBUG_PATH)/hadith_key.o
	gcc $(CFLAGS_DEBUG) -o $(OUT_DEBUG_LINUX) $(OBJ_DEBUG_PATH)/city.o $(OBJ_DEBUG_PATH)/cJSON.o $(OBJ_DEBUG_PATH)/config.o $(OBJ_DEBUG_PATH)/file.o $(OBJ_DEBUG_PATH)/geolocation.o $(OBJ_DEBUG_PATH)/gui.o $(OBJ_DEBUG_PATH)/gui_assistant.o $(OBJ_DEBUG_PATH)/gui_dlg_settings.o $(OBJ_DEBUG_PATH)/gui_general.o $(OBJ_DEBUG_PATH)/gui_listbox.o $(OBJ_DEBUG_PATH)/gui_menuitm.o $(OBJ_DEBUG_PATH)/lang.o $(OBJ_DEBUG_PATH)/main.o $(OBJ_DEBUG_PATH)/prayer_times.o $(OBJ_DEBUG_PATH)/prayer_times_calc.o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o $(OBJ_DEBUG_PATH)/Salah_times_calc.o $(OBJ_DEBUG_PATH)/socket.o $(OBJ_DEBUG_PATH)/update.o $(OBJ_DEBUG_PATH)/error.o $(OBJ_DEBUG_PATH)/hadith.o $(OBJ_DEBUG_PATH)/hadith_key.o `pkg-config gtk+-3.0 --libs` -lssl -lcrypto -lm -static-libasan


./Graphics/icon.res: ./Graphics/icon.rc
	windres ./Graphics/icon.rc -O coff -o ./Graphics/icon.res
 
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
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/Salah_times_calc.c -o $(OBJ_DEBUG_PATH)/Salah_times_calc.o -lm

$(OBJ_DEBUG_PATH)/socket.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/socket.c -o $(OBJ_DEBUG_PATH)/socket.o

$(OBJ_DEBUG_PATH)/update.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/update.c -o $(OBJ_DEBUG_PATH)/update.o
	
$(OBJ_DEBUG_PATH)/error.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/error.c -o $(OBJ_DEBUG_PATH)/error.o
	
$(OBJ_DEBUG_PATH)/hadith.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/hadith.c -o $(OBJ_DEBUG_PATH)/hadith.o
	
$(OBJ_DEBUG_PATH)/hadith_key.o:
	gcc $(CFLAGS_DEBUG) -c $(SRC_PATH)/hadith_key.c -o $(OBJ_DEBUG_PATH)/hadith_key.o
	
$(OBJ_DEBUG_PATH)/gui.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/gui.c -o $(OBJ_DEBUG_PATH)/gui.o
	
$(OBJ_DEBUG_PATH)/gui_assistant.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/gui_assistant.c -o $(OBJ_DEBUG_PATH)/gui_assistant.o
	
$(OBJ_DEBUG_PATH)/gui_dlg_settings.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/gui_dlg_settings.c -o $(OBJ_DEBUG_PATH)/gui_dlg_settings.o
	
$(OBJ_DEBUG_PATH)/gui_general.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/gui_general.c -o $(OBJ_DEBUG_PATH)/gui_general.o
	
$(OBJ_DEBUG_PATH)/gui_listbox.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/gui_listbox.c -o $(OBJ_DEBUG_PATH)/gui_listbox.o
	
$(OBJ_DEBUG_PATH)/gui_menuitm.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/gui_menuitm.c -o $(OBJ_DEBUG_PATH)/gui_menuitm.o
	
$(OBJ_DEBUG_PATH)/main.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_DEBUG) -c $(SRC_PATH)/main.c -o $(OBJ_DEBUG_PATH)/main.o
	
$(OBJ_RELEASE_PATH)/city.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/city.c -o $(OBJ_RELEASE_PATH)/city.o

$(OBJ_RELEASE_PATH)/cJSON.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/cJSON.c -o $(OBJ_RELEASE_PATH)/cJSON.o

$(OBJ_RELEASE_PATH)/config.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/config.c -o $(OBJ_RELEASE_PATH)/config.o

$(OBJ_RELEASE_PATH)/file.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/file.c -o $(OBJ_RELEASE_PATH)/file.o

$(OBJ_RELEASE_PATH)/geolocation.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/geolocation.c -o $(OBJ_RELEASE_PATH)/geolocation.o

$(OBJ_RELEASE_PATH)/lang.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/lang.c -o $(OBJ_RELEASE_PATH)/lang.o

$(OBJ_RELEASE_PATH)/prayer_times.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/prayer_times.c -o $(OBJ_RELEASE_PATH)/prayer_times.o

$(OBJ_RELEASE_PATH)/prayer_times_calc.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/prayer_times_calc.c -o $(OBJ_RELEASE_PATH)/prayer_times_calc.o

$(OBJ_RELEASE_PATH)/prayer_times_diyanet.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/prayer_times_diyanet.c -o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o

$(OBJ_RELEASE_PATH)/Salah_times_calc.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/Salah_times_calc.c -o $(OBJ_RELEASE_PATH)/Salah_times_calc.o -lm

$(OBJ_RELEASE_PATH)/socket.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/socket.c -o $(OBJ_RELEASE_PATH)/socket.o

$(OBJ_RELEASE_PATH)/update.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/update.c -o $(OBJ_RELEASE_PATH)/update.o
	
$(OBJ_RELEASE_PATH)/error.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/error.c -o $(OBJ_RELEASE_PATH)/error.o
	
$(OBJ_RELEASE_PATH)/hadith.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/hadith.c -o $(OBJ_RELEASE_PATH)/hadith.o
	
$(OBJ_RELEASE_PATH)/hadith_key.o:
	gcc $(CFLAGS_RELEASE) -c $(SRC_PATH)/hadith_key.c -o $(OBJ_RELEASE_PATH)/hadith_key.o
	
$(OBJ_RELEASE_PATH)/gui.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_RELEASE) -c $(SRC_PATH)/gui.c -o $(OBJ_RELEASE_PATH)/gui.o
	
$(OBJ_RELEASE_PATH)/gui_assistant.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_RELEASE) -c $(SRC_PATH)/gui_assistant.c -o $(OBJ_RELEASE_PATH)/gui_assistant.o
	
$(OBJ_RELEASE_PATH)/gui_dlg_settings.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_RELEASE) -c $(SRC_PATH)/gui_dlg_settings.c -o $(OBJ_RELEASE_PATH)/gui_dlg_settings.o
	
$(OBJ_RELEASE_PATH)/gui_general.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_RELEASE) -c $(SRC_PATH)/gui_general.c -o $(OBJ_RELEASE_PATH)/gui_general.o
	
$(OBJ_RELEASE_PATH)/gui_listbox.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_RELEASE) -c $(SRC_PATH)/gui_listbox.c -o $(OBJ_RELEASE_PATH)/gui_listbox.o
	
$(OBJ_RELEASE_PATH)/gui_menuitm.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_RELEASE) -c $(SRC_PATH)/gui_menuitm.c -o $(OBJ_RELEASE_PATH)/gui_menuitm.o
	
$(OBJ_RELEASE_PATH)/main.o:
	gcc `pkg-config gtk+-3.0 --cflags` $(CFLAGS_RELEASE) -c $(SRC_PATH)/main.c -o $(OBJ_RELEASE_PATH)/main.o	
	
cleanDebug:
	rm $(OUT_DEBUG) $(OBJ_DEBUG_PATH)/city.o $(OBJ_DEBUG_PATH)/cJSON.o $(OBJ_DEBUG_PATH)/config.o $(OBJ_DEBUG_PATH)/file.o $(OBJ_DEBUG_PATH)/geolocation.o $(OBJ_DEBUG_PATH)/gui.o $(OBJ_DEBUG_PATH)/gui_assistant.o $(OBJ_DEBUG_PATH)/gui_dlg_settings.o $(OBJ_DEBUG_PATH)/gui_general.o $(OBJ_DEBUG_PATH)/gui_listbox.o $(OBJ_DEBUG_PATH)/gui_menuitm.o $(OBJ_DEBUG_PATH)/lang.o $(OBJ_DEBUG_PATH)/main.o $(OBJ_DEBUG_PATH)/prayer_times.o $(OBJ_DEBUG_PATH)/prayer_times_calc.o $(OBJ_DEBUG_PATH)/prayer_times_diyanet.o $(OBJ_DEBUG_PATH)/Salah_times_calc.o $(OBJ_DEBUG_PATH)/socket.o $(OBJ_DEBUG_PATH)/update.o $(OBJ_DEBUG_PATH)/error.o $(OBJ_DEBUG_PATH)/hadith.o
	
cleanRelease:
	rm $(OUT_RELEASE) $(OBJ_RELEASE_PATH)/city.o $(OBJ_RELEASE_PATH)/cJSON.o $(OBJ_RELEASE_PATH)/config.o $(OBJ_RELEASE_PATH)/file.o $(OBJ_RELEASE_PATH)/geolocation.o $(OBJ_RELEASE_PATH)/gui.o $(OBJ_RELEASE_PATH)/gui_assistant.o $(OBJ_RELEASE_PATH)/gui_dlg_settings.o $(OBJ_RELEASE_PATH)/gui_general.o $(OBJ_RELEASE_PATH)/gui_listbox.o $(OBJ_RELEASE_PATH)/gui_menuitm.o $(OBJ_RELEASE_PATH)/lang.o $(OBJ_RELEASE_PATH)/main.o $(OBJ_RELEASE_PATH)/prayer_times.o $(OBJ_RELEASE_PATH)/prayer_times_calc.o $(OBJ_RELEASE_PATH)/prayer_times_diyanet.o $(OBJ_RELEASE_PATH)/Salah_times_calc.o $(OBJ_RELEASE_PATH)/socket.o $(OBJ_RELEASE_PATH)/update.o $(OBJ_RELEASE_PATH)/error.o $(OBJ_RELEASE_PATH)/hadith.o

run: $(OUT_RELEASE)
	$(OUT_RELEASE)
	
runLinux: $(OUT_RELEASE_LINUX)
	$(OUT_RELEASE_LINUX)

debug: $(OUT_DEBUG)
	gdb $(OUT_DEBUG)
	
debugLinux: $(OUT_DEBUG_LINUX)
	gdb $(OUT_DEBUG_LINUX)
	

valgrind: $(OUT_DEBUG_LINUX)
	valgrind $(OUT_DEBUG_LINUX)

valgrind_leakcheck: $(OUT_DEBUG_LINUX)
	valgrind --leak-check=full $(OUT_DEBUG_LINUX)

valgrind_extreme: $(OUT_DEBUG_LINUX)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes $(OUT_DEBUG_LINUX)
