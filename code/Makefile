#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
CFILES		:=
CPPFILES	:=
TARGET		:=	freespace2
BUILD		:=	build
FSSOURCES	:=	ai anim autopilot bmpman camera cfile cmdline cmeasure controlconfig \
        ddsutils debris decals demo exceptionhandler fireball gamehelp gamesequence \
        gamesnd hud iff_defs irc jumpnode lab lighting localization math menuui \
        mission missionui model nebula object observer palman parse particle pcxutils \
        physics playerman popup radar render ship sound species_defs starfield stats \
        tgautils ui vcodec weapon globalincs debugconsole  osapi io \
        graphics windows_stub asteroid wii_port freespace2 \
		network fs2netd inetfile jpgutils  cutscene sound/ogg
LIBSOURCES	:= ../libjpeg ../lua
SOURCES		:= $(FSSOURCES) $(LIBSOURCES)
DATA		:=	data  
INCLUDES	:=  . ../lua ../libjpeg
INCLUDE_DIR := /i/setup/SDL-Port/include /i/setup/libogg-1.1.3/include /i/setup/libtheora-1.0beta3/include /i/setup/libvorbis-1.2.0/include /i/setup/openal-soft-1.5.304/include /i/netport/include /i/gnulib/include /i/setup/gl2gx/include /i/profile_tools/wiitrace/tracer/include/

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------


#CFLAGS	= -Wall -g -finline-functions -save-temps -Os $(MACHDEP) $(INCLUDE) -DSCP_WII -DUSE_OPENAL -DSCP_UNIX -D_POSIX_SOURCE -DNO_DIRECT3D \
#		-D_POSIX_TIMERS -fstack-protector-all -fbounds-check -fstack-limit-symbol=__stack_end -Wstack-protector -finstrument-functions -fsigned-char

CFLAGS	= -Wall -g -finline-functions -save-temps -Os $(MACHDEP) $(INCLUDE) -DSCP_WII -DUSE_OPENAL -DSCP_UNIX -D_POSIX_SOURCE -DNO_DIRECT3D \
		-D_POSIX_TIMERS -DNDEBUG -DDEBUG_MALLOC -fsigned-char -DHAVE_MALLOC_H
		
CXXFLAGS	=	$(CFLAGS)

LDFLAGS	= -g $(MACHDEP) -Wl,-Map,$(notdir $@).map -fstack-protector-all -fbounds-check -fstack-limit-symbol=__stack_end -finstrument-functions

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
#LIBS	:=	-logc -nodefaultlibs -lstdc++ -lsupc++ -lc -lgcc
LIBS	:=	-ltheora -lvorbis -logg -lopenal -lgl2gx -lmemtrace -ltrace -lz -lgnu -lnetport -lSDL -lfat -lwiiuse -lbte -logc -lm 

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= /i/setup/gl2gx/libogc/lib/wii /i/setup/SDL-Port/lib /i/libogc/lib/wii /i/gnulib/lib /i/setup/libogg-1.1.3/lib /i/setup/libtheora-1.0beta3/lib /i/setup/libvorbis-1.2.0/lib /i/setup/openal-soft-1.5.304/lib /i/netport/lib /i/profile_tools/wiitrace/tracer/lib /i/setup/memtracer/lib /i/setup/libfat/libogc/lib/wii

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
HFILES		:=  $(foreach dir,$(FSSOURCES),$(notdir $(wildcard $(dir)/*.h)))
CFILES		:=	$(CFILES) $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(CPPFILES) $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export GCHFILES := 	$(addsuffix .gch, $(HFILES))

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	-I$(BUILD) $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					-I$(BUILD) $(foreach dir,$(INCLUDE_DIR),-I$(dir)) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)) \
					-L$(LIBOGC_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol $(GCHFILES)
#---------------------------------------------------------------------------------
run:
	wiiload $(OUTPUT).dol

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

all: $(GCHFILES) $(OUTPUT).dol
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	:	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)
%.h.gch: %.h
	@echo $(notdir $<)
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -D PRECOMP_HEADER -c $< -o $@

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
