.SECONDEXPANSION:
.PHONY: _base 
_base: base





#Compiler command
CC := gcc

#####################
### Directories #####
#####################
# Dynamic directories can be changed without issue static cannot

# Source Directory (Static)
SDIR :=./src
# Include Directory (Static)
IDIR :=./include
# Libary Directory (Static)
LDIR :=./lib
# Object Directory (Dynamic)
ODIR :=./obj
# Optimization Object Directory (Dynamic)
OODIR :=./obj_o
# Binary Directory (Dynamic)
BDIR :=./bin
# Asembler output Directory (Dynamic)
ADIR :=./asm
# Assmebelr Optimization output directory (Dynamic)
OADIR :=./asm_o

RDIR := ./results

###########################
####### Normal Libaries ###
###########################

#Libaries
LIBS := 
#Benchamrking Libaries
BENCH_LIBS = -lstdc++


#####################
####### FLAGS ######
####################

# C Specific flags
CFLAGS :=-I$(IDIR) 
# CPP Specific Flags
CPPFLAGS := $(CFLAGS)  

#DEBUGGNING FLAGS (Turned off by default)
.PHONY: _debug
_debug:
	@echo "!!!!!!! Building for debugging !!!!!!!!!!!"
	$(eval DEBUG_FLAGS = -g)

# PROFILING FLAGS (Turned off by defualt)
.PHONY: _prof
_prof:
	@echo "!!!!!!! Building for profiling !!!!!!!!!!!"
	$(eval PERF_FLAGS = -g -pg)


# Normal Optimiztion Flags
OPTFLAGS :=

# Linker Flags
LDFLAGS :=
# Benchmarking Specific Linker flags
BENCH_LDFLAGS :=

# Asmbler output flags
ASM_FLAGS := -fverbose-asm -g


###########################
#### Files in use #########
###########################

#Dependancies files
_DEPS :=  
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))
BENCH_DEPS :=

#Exceutable file name
EXE := run

# Objects to build
_OBJS := f/t.o hg.o
OBJS := $(patsubst %.o,$(ODIR)/%.o,$(_OBJS))


# Main file to build
_MAIN := main.o
MAIN := $(patsubst %.o,$(ODIR)/%.o,$(_MAIN))

# Benchmark main to build
_B_MAIN := benchmark.o
B_MAIN := $(patsubst %.o,$(ODIR)/%.o,$(_B_MAIN))

#Basic Assembler Files
A_FILES := $(patsubst %.o,$(ADIR)/%.s,$(_OBJS))
A_FILES += $(patsubst %.o,$(ADIR)/%.s,$(_B_MAIN))
A_FILES += $(patsubst %.o,$(ADIR)/%.s,$(_MAIN))

###############################################
##### Build Celero for benchmarking (CPP) #####
###############################################
CELEDIR := lib/Celero
CELEDIR_B := lib/CeleroBuild

BENCH_FLAGS += -I./$(CELEDIR)/include
BENCH_LIBS += -L$(CELEDIR_B) -lcelero
BENCH_LDFLAGS = -Wl,-rpath -Wl,$(CELEDIR_B)
BENCH_DEPS += $(CELEDIR_B)/libcelero.so

$(CELEDIR)/CMakeLists.txt:
	@echo "---- Cloning Celero"
	@mkdir -p $(LDIR)
	@rm -rf $(CELEDIR)
	@git clone --depth=1  https://github.com/DigitalInBlue/Celero.git ./$(CELEDIR)

$(CELEDIR_B)/libcelero.so: $(CELEDIR)/CMakeLists.txt
	@echo "========================================================="
	@echo "==================== BUILDING CELERO ===================="
	@echo "========================================================="
	@cmake -S $(CELEDIR) -B $(CELEDIR_B)
	@$(MAKE) -C $(CELEDIR_B)
	@echo "========================================================="
	@echo "==================== DONE CELERO ===================="
	@echo "========================================================="

.PHONY: clean-celero
clean-celero:
	@echo "---- Cleaning Celero"
	@rm -rf $(CELEDIR) $(CELEDIR_B)

################################################
######### Collect flags for readability ########
################################################
B_FLAGS = $(CPPFLAGS) $(DEBUG_FLAGS) $(PERF_FLAGS) $(BENCH_FLAGS) $(LDFLAGS) $(BENCH_LDFLAGS)
B_LIBS := $(LIBS) $(BENCH_LIBS)
FLAGS = $(CFLAGS) $(DEBUG_FLAGS) $(PROF_FLAGS)

##################################################################################################
########################## FOLDER RULES ##########################################################
##################################################################################################

.PRECIOUS: $(BDIR)/. $(BDIR)%/. $(ADIR)/. $(ADIR)%/. 
.PRECIOUS: $(ODIR)/. $(ODIR)%/. $(OODIR)/. $(OODIR)%/.
.PRECIOUS: $(OADIR)/. $(OADIR)%/. $(OADIR)/. $(OADIR)/.

$(ODIR)/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(ODIR)%/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(OODIR)/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(OODIR)%/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(BDIR)/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(BDIR)%/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(ADIR)/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(ADIR)%/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(OADIR)/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(OADIR)%/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(RDIR)/.:
	@echo "--== Creeating $@"
	@mkdir -p $@

#############################################
##########  Build standard files ############
#############################################

# Assembly files
$(ADIR)/%.s: $(SDIR)/%.c $(DEPS) | $$(@D)/.
	@echo "---- Assembling: $@"
	@$(strip $(CC) $(OPTFLAGS) $(FLAGS) -fverbose-asm -g -S -o $@ $< $(LIBS))

$(ADIR)/%.s: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(@D)/.
	@echo "---- Assembling: $@"
	@$(strip $(CC) $(OPTFLAGS) $(B_FLAGS) -S -o $@ $< $(B_LIBS))

## Building Objects
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(FLAGS) -c -o $@ $<)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(B_FLAGS) -c -o $@ $<)

## Linking main
$(BDIR)/$(EXE).a: $(OBJS) $(MAIN) | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@"
	@$(strip $(CC) $(OPTFLAGS) $(FLAGS) $(LDFLAGS) -o $@ $(OBJS) $(MAIN) $(LIBS))

$(BDIR)/B_$(EXE).a: $(OBJS) $(B_MAIN) | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@ $(*F) "
	@$(strip $(CC) $(OPTFLAGS) $(B_FLAGS) -o $@ $^ $(B_LIBS))


.PHONY: _start _basic _end
_basic: _start $(BDIR)/B_$(EXE).a $(BDIR)/$(EXE).a $(A_FILES) _end

_start:
	@echo "========= Building Base ========="

_end:
	@echo "========= Finsihed Base ========="
	@echo ""


##################################################################################################
########################## BUILD RULES for different flags #######################################
##################################################################################################

FOLDER = $$(@D)/.
define FLAG_SET
$(1)FLAGS := $(2)
$(1)OBJS := $$(patsubst %.o,$$(OODIR)/$(1)/%.o,$$(_OBJS))
$(1)ASM := $$(patsubst %.o,$$(OADIR)/$(1)/%.s,$$(_OBJS))
$(1)MAIN := $$(patsubst %.o,$$(OODIR)/$(1)/%.o,$$(_MAIN))
$(1)B_MAIN :=  $$(patsubst %.o,$$(OODIR)/$(1)/%.o,$$(_B_MAIN))

.PHONY: $(1) B$(1) _$(1)START _$(1)asm _$(1)END
$(1): _$(1)START $$(BDIR)/$(1)/B_$$(EXE).a $$(BDIR)/$(1)/$$(EXE).a $(1)asm _$(1)END
$(1)asm: $$($(1)ASM)

_$(1)START:
	@echo "========= Building $(1) ========="

_$(1)END:
	@echo "========= Finsihed $(1) ========="
	@echo ""

$(OADIR)/$(1)/%.s: $(SDIR)/%.c $$(DEPS) | $$(FOLDER)
	@echo "---- Assembling: $$@"
	@$$(strip $(CC) $$($(1)FLAGS) $$(FLAGS) $$(ASM_FLAGS) -S -o $$@ $$< $$(LIBS))

$(OADIR)/$(1)/%.s: $(SDIR)/%.cpp $$(DEPS) $(BENCH_DEPS) | $$(FOLDER)
	@echo "---- Assembling: $$@ $$(@D)/."
	@$$(strip $(CC) $($(1)FLAGS) $$(B_FLAGS) -S -o $$@ $$< $(B_LIBS))

## Building Objects
$(OODIR)/$(1)/%.o: $$(SDIR)/%.c $$(DEPS) | $$(FOLDER)
	@echo "---- Building $$@" 
	@$$(strip $(CC) $$($(1)FLAGS) $$(FLAGS) -c -o $$@ $$<)

$(OODIR)/$(1)/%.o: $$(SDIR)/%.cpp $$(DEPS) $$(BENCH_DEPS)  | $$(FOLDER)
	@echo "---- Building $$@"
	@$$(strip $(CC) $$($(1)FLAGS) $$(B_FLAGS) -c -o $$@ $$<)

## Linking bechmark
$(BDIR)/$(1)/$(EXE).a: $$($(1)OBJS) $$($(1)MAIN) | $$(RDIR)/. $$(FOLDER).
	@echo "++ Linking $$@ "
	@$$(strip $(CC) $$($(1)FLAGS) $$(FLAGS) $$(LDFLAGS) -o $$@ $$^ $$(LIBS))

$(BDIR)/$(1)/B_$(EXE).a: $$($(1)OBJS) $$($(1)B_MAIN) | $$(RDIR)/. $$(FOLDER)
	@echo "++ Linking $$@"
	@$$(strip $(CC) $$($(1)FLAGS) $$(B_FLAGS) -o $$@ $$^ $(B_LIBS))

endef


$(eval $(call FLAG_SET,o0,-O1))
$(eval $(call FLAG_SET,o1,-O2))
$(eval $(call FLAG_SET,o2,-O2))
$(eval $(call FLAG_SET,o3,-O3))
$(eval $(call FLAG_SET,og,-Og))
$(eval $(call FLAG_SET,os,-Os))

$(eval $(call FLAG_SET,o0n,-O0 -march=native))
$(eval $(call FLAG_SET,o1n,-O1 -march=native))
$(eval $(call FLAG_SET,o2n,-O2 -march=native))
$(eval $(call FLAG_SET,o3n,-O3 -march=native))
$(eval $(call FLAG_SET,osn,-Os -march=native))
$(eval $(call FLAG_SET,ogn,-Og -march=native))


###############################
######## general rules ########
###############################


.PHONY: base project all benchmark
base: _basic
project: o0 o2 o2n
all: base oall

.PHONY: opt-all oall on oNum
oall: o0 o1 o2 o3 o0n o1n o2n o3n og ogn os osn
oNum: o0 o1 o2 o3
oNum-n: o0n o1n o2n o3n

.PHONY: _touch_src _touch_src2
_touch_src:
	@echo "!!!!!!!!!!! touching $(SDIR)"
	@touch $(SDIR)/*

_touch_src2:
	@echo "!!!!!!!!!!! touching $(SDIR)"
	@touch $(SDIR)/*

debug-base:    _debug _touch_src base    _touch_src2
debug-project: _debug _touch_src project _touch_src2
debug-all:     _debug _touch_src all     _touch_src2
debug-oall:    _debug _touch_src oall    _touch_src2
debug-oNum:    _debug _touch_src oNum    _touch_src2
debug-oNum-n:  _debug _touch_src oNum-n  _touch_src2

prof-base:    _prof _touch_src base    _touch_src2
prof-project: _prof _touch_src project _touch_src2
prof-all:     _prof _touch_src all     _touch_src2
prof-oall:    _prof _touch_src oall    _touch_src2
prof-oNum:    _prof _touch_src oNum    _touch_src2
prof-oNum-n:  _prof _touch_src oNum-n  _touch_src2
###############################################
######## General clean-up Rules ###############
###############################################

.PHONY: clean clean_obj clean_asm clean_bin clean-results clean-all
clean:
	rm -f $(BDIR)/*.a $(BDIR)/*/*.a 
clean_obj:
	@echo "---- Cleaning Objects"
	@rm -f $(ODIR)/*.o $(ODIR)/*/*.o
	@echo "---- Cleaning Optimization Objects"
	@rm -f $(OODIR)/*.o $(OODIR)/*/*.o 

clean_asm:
	@echo "---- Cleaning Assembler code"
	@rm -f $(ADIR)/*.s $(ADIR)/*/*.s
	@echo "---- Cleaning Optimization Assembler code"
	@rm -f $(OADIR)/*.s $(OADIR)/*/*.s

clean_bin:
	@echo "---- Cleaning bin"
	@rm -f $(BDIR)/*.a $(BDIR)/*/*.a 

clean-results:
	@echo "---- Cleaning Results"
	@rm -fr $(RDIR)/*
	

clean-folders:
	@echo "---- Removing Folders"
	@rm -fr $(ADIR) $(OADIR) $(BDIR) $(ODIR) $(OODIR) $(RDIR)

clean-all: clean clean-results clean-folders clean-celero
