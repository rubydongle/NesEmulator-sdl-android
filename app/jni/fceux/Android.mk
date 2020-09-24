LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/drivers

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

SDL_PATH := ../SDL

LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(SDL_PATH)/include

LOCAL_SRC_FILES := \
	${LOCAL_PATH}/asm.cpp \
  	${LOCAL_PATH}/cart.cpp \
  	${LOCAL_PATH}/cheat.cpp \
  	${LOCAL_PATH}/conddebug.cpp \
  	${LOCAL_PATH}/config.cpp \
  	${LOCAL_PATH}/debug.cpp \
  	${LOCAL_PATH}/drawing.cpp \
  	${LOCAL_PATH}/fceu.cpp \
  	${LOCAL_PATH}/fds.cpp \
  	${LOCAL_PATH}/file.cpp \
  	${LOCAL_PATH}/emufile.cpp \
  	${LOCAL_PATH}/filter.cpp \
  	${LOCAL_PATH}/ines.cpp \
  	${LOCAL_PATH}/input.cpp \
  	${LOCAL_PATH}/movie.cpp \
  	${LOCAL_PATH}/netplay.cpp \
  	${LOCAL_PATH}/nsf.cpp \
  	${LOCAL_PATH}/oldmovie.cpp \
  	${LOCAL_PATH}/palette.cpp \
  	${LOCAL_PATH}/ppu.cpp \
  	${LOCAL_PATH}/sound.cpp \
  	${LOCAL_PATH}/state.cpp \
  	${LOCAL_PATH}/unif.cpp \
  	${LOCAL_PATH}/video.cpp \
  	${LOCAL_PATH}/vsuni.cpp \
  	${LOCAL_PATH}/wave.cpp \
  	${LOCAL_PATH}/x6502.cpp \
  	${LOCAL_PATH}/boards/01-222.cpp \
  	${LOCAL_PATH}/boards/09-034a.cpp \
  	${LOCAL_PATH}/boards/103.cpp \
  	${LOCAL_PATH}/boards/106.cpp \
  	${LOCAL_PATH}/boards/108.cpp \
  	${LOCAL_PATH}/boards/112.cpp \
  	${LOCAL_PATH}/boards/116.cpp \
  	${LOCAL_PATH}/boards/117.cpp \
  	${LOCAL_PATH}/boards/120.cpp \
  	${LOCAL_PATH}/boards/121.cpp \
  	${LOCAL_PATH}/boards/12in1.cpp \
  	${LOCAL_PATH}/boards/151.cpp \
  	${LOCAL_PATH}/boards/156.cpp \
  	${LOCAL_PATH}/boards/158B.cpp \
  	${LOCAL_PATH}/boards/15.cpp \
  	${LOCAL_PATH}/boards/164.cpp \
  	${LOCAL_PATH}/boards/168.cpp \
  	${LOCAL_PATH}/boards/170.cpp \
  	${LOCAL_PATH}/boards/175.cpp \
  	${LOCAL_PATH}/boards/176.cpp \
  	${LOCAL_PATH}/boards/177.cpp \
  	${LOCAL_PATH}/boards/178.cpp \
  	${LOCAL_PATH}/boards/183.cpp \
  	${LOCAL_PATH}/boards/185.cpp \
  	${LOCAL_PATH}/boards/186.cpp \
  	${LOCAL_PATH}/boards/187.cpp \
  	${LOCAL_PATH}/boards/189.cpp \
  	${LOCAL_PATH}/boards/18.cpp \
  	${LOCAL_PATH}/boards/190.cpp \
  	${LOCAL_PATH}/boards/193.cpp \
  	${LOCAL_PATH}/boards/199.cpp \
  	${LOCAL_PATH}/boards/206.cpp \
  	${LOCAL_PATH}/boards/208.cpp \
  	${LOCAL_PATH}/boards/222.cpp \
  	${LOCAL_PATH}/boards/225.cpp \
  	${LOCAL_PATH}/boards/228.cpp \
  	${LOCAL_PATH}/boards/230.cpp \
  	${LOCAL_PATH}/boards/232.cpp \
  	${LOCAL_PATH}/boards/234.cpp \
  	${LOCAL_PATH}/boards/235.cpp \
  	${LOCAL_PATH}/boards/244.cpp \
  	${LOCAL_PATH}/boards/246.cpp \
  	${LOCAL_PATH}/boards/252.cpp \
  	${LOCAL_PATH}/boards/253.cpp \
  	${LOCAL_PATH}/boards/28.cpp \
  	${LOCAL_PATH}/boards/32.cpp \
  	${LOCAL_PATH}/boards/33.cpp \
  	${LOCAL_PATH}/boards/34.cpp \
  	${LOCAL_PATH}/boards/36.cpp \
  	${LOCAL_PATH}/boards/3d-block.cpp \
  	${LOCAL_PATH}/boards/40.cpp \
  	${LOCAL_PATH}/boards/411120-c.cpp \
  	${LOCAL_PATH}/boards/41.cpp \
  	${LOCAL_PATH}/boards/42.cpp \
  	${LOCAL_PATH}/boards/43.cpp \
  	${LOCAL_PATH}/boards/46.cpp \
  	${LOCAL_PATH}/boards/50.cpp \
  	${LOCAL_PATH}/boards/51.cpp \
  	${LOCAL_PATH}/boards/57.cpp \
  	${LOCAL_PATH}/boards/603-5052.cpp \
  	${LOCAL_PATH}/boards/62.cpp \
  	${LOCAL_PATH}/boards/65.cpp \
  	${LOCAL_PATH}/boards/67.cpp \
  	${LOCAL_PATH}/boards/68.cpp \
  	${LOCAL_PATH}/boards/69.cpp \
  	${LOCAL_PATH}/boards/71.cpp \
  	${LOCAL_PATH}/boards/72.cpp \
  	${LOCAL_PATH}/boards/77.cpp \
  	${LOCAL_PATH}/boards/79.cpp \
  	${LOCAL_PATH}/boards/80013-B.cpp \
  	${LOCAL_PATH}/boards/80.cpp \
  	${LOCAL_PATH}/boards/8157.cpp \
  	${LOCAL_PATH}/boards/8237.cpp \
  	${LOCAL_PATH}/boards/82.cpp \
  	${LOCAL_PATH}/boards/830118C.cpp \
  	${LOCAL_PATH}/boards/88.cpp \
  	${LOCAL_PATH}/boards/8in1.cpp \
  	${LOCAL_PATH}/boards/90.cpp \
  	${LOCAL_PATH}/boards/91.cpp \
  	${LOCAL_PATH}/boards/96.cpp \
  	${LOCAL_PATH}/boards/99.cpp \
  	${LOCAL_PATH}/boards/a9746.cpp \
  	${LOCAL_PATH}/boards/ac-08.cpp \
  	${LOCAL_PATH}/boards/addrlatch.cpp \
  	${LOCAL_PATH}/boards/ax5705.cpp \
  	${LOCAL_PATH}/boards/bandai.cpp \
  	${LOCAL_PATH}/boards/bb.cpp \
  	${LOCAL_PATH}/boards/bmc13in1jy110.cpp \
  	${LOCAL_PATH}/boards/bmc42in1r.cpp \
  	${LOCAL_PATH}/boards/bmc64in1nr.cpp \
  	${LOCAL_PATH}/boards/bmc70in1.cpp \
  	${LOCAL_PATH}/boards/BMW8544.cpp \
  	${LOCAL_PATH}/boards/bonza.cpp \
  	${LOCAL_PATH}/boards/bs-5.cpp \
  	${LOCAL_PATH}/boards/cheapocabra.cpp \
  	${LOCAL_PATH}/boards/cityfighter.cpp \
  	${LOCAL_PATH}/boards/coolboy.cpp \
  	${LOCAL_PATH}/boards/dance2000.cpp \
  	${LOCAL_PATH}/boards/datalatch.cpp \
  	${LOCAL_PATH}/boards/dream.cpp \
  	${LOCAL_PATH}/boards/__dummy_mapper.cpp \
  	${LOCAL_PATH}/boards/edu2000.cpp \
  	${LOCAL_PATH}/boards/eh8813a.cpp \
  	${LOCAL_PATH}/boards/emu2413.c \
  	${LOCAL_PATH}/boards/et-100.cpp \
  	${LOCAL_PATH}/boards/et-4320.cpp \
  	${LOCAL_PATH}/boards/F-15.cpp \
  	${LOCAL_PATH}/boards/famicombox.cpp \
  	${LOCAL_PATH}/boards/ffe.cpp \
  	${LOCAL_PATH}/boards/fk23c.cpp \
  	${LOCAL_PATH}/boards/fns.cpp \
  	${LOCAL_PATH}/boards/ghostbusters63in1.cpp \
  	${LOCAL_PATH}/boards/gs-2004.cpp \
  	${LOCAL_PATH}/boards/gs-2013.cpp \
  	${LOCAL_PATH}/boards/h2288.cpp \
  	${LOCAL_PATH}/boards/hp10xx_hp20xx.cpp \
  	${LOCAL_PATH}/boards/hp898f.cpp \
  	${LOCAL_PATH}/boards/inlnsf.cpp \
  	${LOCAL_PATH}/boards/karaoke.cpp \
  	${LOCAL_PATH}/boards/kof97.cpp \
  	${LOCAL_PATH}/boards/ks7010.cpp \
  	${LOCAL_PATH}/boards/ks7012.cpp \
  	${LOCAL_PATH}/boards/ks7013.cpp \
  	${LOCAL_PATH}/boards/ks7016.cpp \
  	${LOCAL_PATH}/boards/ks7017.cpp \
  	${LOCAL_PATH}/boards/ks7030.cpp \
  	${LOCAL_PATH}/boards/ks7031.cpp \
  	${LOCAL_PATH}/boards/ks7032.cpp \
  	${LOCAL_PATH}/boards/ks7037.cpp \
  	${LOCAL_PATH}/boards/ks7057.cpp \
  	${LOCAL_PATH}/boards/le05.cpp \
  	${LOCAL_PATH}/boards/lh32.cpp \
  	${LOCAL_PATH}/boards/lh53.cpp \
  	${LOCAL_PATH}/boards/malee.cpp \
  	${LOCAL_PATH}/boards/mihunche.cpp \
  	${LOCAL_PATH}/boards/mmc1.cpp \
  	${LOCAL_PATH}/boards/mmc2and4.cpp \
  	${LOCAL_PATH}/boards/mmc3.cpp \
  	${LOCAL_PATH}/boards/mmc5.cpp \
  	${LOCAL_PATH}/boards/n106.cpp \
  	${LOCAL_PATH}/boards/n625092.cpp \
  	${LOCAL_PATH}/boards/novel.cpp \
  	${LOCAL_PATH}/boards/onebus.cpp \
  	${LOCAL_PATH}/boards/pec-586.cpp \
  	${LOCAL_PATH}/boards/rt-01.cpp \
  	${LOCAL_PATH}/boards/sa-9602b.cpp \
  	${LOCAL_PATH}/boards/sachen.cpp \
  	${LOCAL_PATH}/boards/sb-2000.cpp \
  	${LOCAL_PATH}/boards/sc-127.cpp \
  	${LOCAL_PATH}/boards/sheroes.cpp \
  	${LOCAL_PATH}/boards/sl1632.cpp \
  	${LOCAL_PATH}/boards/subor.cpp \
  	${LOCAL_PATH}/boards/super24.cpp \
  	${LOCAL_PATH}/boards/supervision.cpp \
  	${LOCAL_PATH}/boards/t-227-1.cpp \
  	${LOCAL_PATH}/boards/t-262.cpp \
  	${LOCAL_PATH}/boards/tengen.cpp \
  	${LOCAL_PATH}/boards/tf-1201.cpp \
  	${LOCAL_PATH}/boards/transformer.cpp \
  	${LOCAL_PATH}/boards/unrom512.cpp \
  	${LOCAL_PATH}/boards/vrc1.cpp \
  	${LOCAL_PATH}/boards/vrc2and4.cpp \
  	${LOCAL_PATH}/boards/vrc3.cpp \
  	${LOCAL_PATH}/boards/vrc5.cpp \
  	${LOCAL_PATH}/boards/vrc6.cpp \
  	${LOCAL_PATH}/boards/vrc7.cpp \
  	${LOCAL_PATH}/boards/vrc7p.cpp \
  	${LOCAL_PATH}/boards/yoko.cpp \
  	${LOCAL_PATH}/input/arkanoid.cpp \
  	${LOCAL_PATH}/input/bworld.cpp \
  	${LOCAL_PATH}/input/cursor.cpp \
  	${LOCAL_PATH}/input/fkb.cpp \
  	${LOCAL_PATH}/input/fns.cpp \
  	${LOCAL_PATH}/input/ftrainer.cpp \
  	${LOCAL_PATH}/input/hypershot.cpp \
  	${LOCAL_PATH}/input/mahjong.cpp \
  	${LOCAL_PATH}/input/mouse.cpp \
  	${LOCAL_PATH}/input/oekakids.cpp \
  	${LOCAL_PATH}/input/pec586kb.cpp \
  	${LOCAL_PATH}/input/powerpad.cpp \
  	${LOCAL_PATH}/input/quiz.cpp \
  	${LOCAL_PATH}/input/shadow.cpp \
  	${LOCAL_PATH}/input/snesmouse.cpp \
  	${LOCAL_PATH}/input/suborkb.cpp \
  	${LOCAL_PATH}/input/toprider.cpp \
  	${LOCAL_PATH}/input/virtualboy.cpp \
  	${LOCAL_PATH}/input/zapper.cpp \
  	${LOCAL_PATH}/utils/backward.cpp \
  	${LOCAL_PATH}/utils/ConvertUTF.c \
  	${LOCAL_PATH}/utils/xstring.cpp \
  	${LOCAL_PATH}/utils/crc32.cpp \
  	${LOCAL_PATH}/utils/endian.cpp \
  	${LOCAL_PATH}/utils/general.cpp \
  	${LOCAL_PATH}/utils/guid.cpp \
  	${LOCAL_PATH}/utils/md5.cpp \
  	${LOCAL_PATH}/utils/memory.cpp \
	${LOCAL_PATH}/drivers/common/args.cpp \
	${LOCAL_PATH}/drivers/common/cheat.cpp \
	${LOCAL_PATH}/drivers/common/config.cpp \
	${LOCAL_PATH}/drivers/common/configSys.cpp \
	${LOCAL_PATH}/drivers/common/hq2x.cpp \
	${LOCAL_PATH}/drivers/common/hq3x.cpp \
	${LOCAL_PATH}/drivers/common/scale2x.cpp \
	${LOCAL_PATH}/drivers/common/scale3x.cpp \
	${LOCAL_PATH}/drivers/common/scalebit.cpp \
	${LOCAL_PATH}/drivers/common/vidblit.cpp \
	${LOCAL_PATH}/drivers/common/os_utils.cpp \
	${LOCAL_PATH}/drivers/common/nes_ntsc.c \
	${LOCAL_PATH}/drivers/sdl-android/config.cpp \
	${LOCAL_PATH}/drivers/sdl-android/input.cpp  \
	${LOCAL_PATH}/drivers/sdl-android/sdl-joystick.cpp \
	${LOCAL_PATH}/drivers/sdl-android/sdl-throttle.cpp \
	${LOCAL_PATH}/drivers/sdl-android/unix-netplay.cpp \
    ${LOCAL_PATH}/drivers/sdl-android/main.cpp \
	${LOCAL_PATH}/drivers/sdl-android/sdl-sound.cpp   \
	${LOCAL_PATH}/drivers/sdl-android/sdl-video.cpp \
	${LOCAL_PATH}/drivers/sdl-android/Emulator.cpp \
#	${LOCAL_PATH}/drivers/videolog/nesvideos-piece.cpp \
#	${LOCAL_PATH}/drivers/videolog/rgbtorgb.cpp \
#	${LOCAL_PATH}/drivers/sdl-android/sdl-netplay.cpp  \
#	${LOCAL_PATH}/drivers/sdl-android/sdl-opengl.cpp \

LOCAL_SHARED_LIBRARIES := SDL2 zlib

LOCAL_SHORT_COMMANDS := true
LOCAL_CFLAGS += -DPSS_STYLE=1 -DHAVE_ASPRINTF
LOCAL_CFLAGS += \
    -frtti \
    -Wall  -Wno-write-strings  \
    -Wno-sign-compare  \
    -Wno-parentheses  \
    -Wno-unused-local-typedefs  \
    -fPIC \
    -Wno-c++11-narrowing \
    -Wno-unused-variable \

LOCAL_CPPFLAGS += -fexceptions

LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -landroid

# -lz -lm
#-lrt
#-lpthread

#ifeq ($(NDK_DEBUG),1)
#    cmd-strip :=
#endif

#LOCAL_STATIC_LIBRARIES := cpufeatures

include $(BUILD_SHARED_LIBRARY)

###########################
#
# SDL static library
#
###########################

#LOCAL_MODULE := SDL2_static

#LOCAL_MODULE_FILENAME := libSDL2

#LOCAL_LDLIBS :=
#LOCAL_EXPORT_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog -landroid

#include $(BUILD_STATIC_LIBRARY)

###########################
#
# SDL main static library
#
###########################

#include $(CLEAR_VARS)

#LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

#LOCAL_MODULE := SDL2_main

#LOCAL_MODULE_FILENAME := libSDL2main

#include $(BUILD_STATIC_LIBRARY)

###########################
#
# hidapi library
#
###########################

#include $(CLEAR_VARS)

#LOCAL_CPPFLAGS += -std=c++11

#LOCAL_SRC_FILES := src/hidapi/android/hid.cpp

#LOCAL_MODULE := libhidapi
#LOCAL_LDLIBS := -llog

#include $(BUILD_SHARED_LIBRARY)

#$(call import-module,android/cpufeatures)

