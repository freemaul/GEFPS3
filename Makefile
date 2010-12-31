#
#    This file is part of GEFPS3.
#
#    GEFPS3 is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    GEFPS3 is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with GEFPS3.  If not, see <http://www.gnu.org/licenses/>.
#

#
# This file was made by a copie of Makefile from PSL1GHT
# https://github.com/HACKERCHANNEL/PSL1GHT
#

.SUFFIXES:

include $(PSL1GHT)/Makefile.base

TARGET		:=	libGEFPS3
BUILD		:=	build
SOURCE		:=	source
INCLUDE		:=	include
DATA		:=	data
LIBS		:=	

LD		:=	ppu-ld

CFLAGS		:=	-g -O2 -Wall

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCE),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export BUILDDIR	:=	$(CURDIR)/$(BUILD)
export DEPSDIR	:=	$(BUILDDIR)

CFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.c)))
CXXFILES	:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:= $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.bin)))

export OFILES	:=	$(CFILES:.c=.o) \
					$(CXXFILES:.cpp=.o) \
					$(SFILES:.S=.o)

export BINFILES	:=	$(BINFILES:.bin=.bin.h)

export INCLUDES	:=	$(foreach dir,$(INCLUDE),-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD) \
					-I$(PS3DEV)/ppu/include/freetype2\
					-I$(PS3DEV)/ppu/include

.PHONY: $(BUILD) clean

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo Clean...
	@rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).a
	@(cd sample && make clean)

install:
	@cp libGEFPS3.a $(PS3DEV)/ppu/lib/
	@cp -R include/GEFPS3 $(PS3DEV)/ppu/include/

uninstall:
	@rm -f $(PS3DEV)/ppu/lib/libGEFPS3.a
	@rm -dir -f $(PS3DEV)/ppu/include/GEFPS3

else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT).a: $(OFILES)


-include $(DEPENDS)

endif
