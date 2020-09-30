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

# Normal Optimiztion Flags
OPTFLAGS := -g

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
	rm -rf $(CELEDIR)
	git clone --depth=1  https://github.com/DigitalInBlue/Celero.git ./$(CELEDIR)

$(CELEDIR_B)/libcelero.so: $(CELEDIR)/CMakeLists.txt
	@echo "========================================================="
	@echo "==================== BUILDING CELERO ===================="
	@echo "========================================================="
	cmake -S $(CELEDIR) -B $(CELEDIR_B)
	$(MAKE) -C $(CELEDIR_B)
	@echo "========================================================="
	@echo "==================== DONE CELERO ===================="
	@echo "========================================================="

.PHONY: clean-celero
clean-celero:
	rm -rf $(CELEDIR) $(CELEDIR_B)

################################################
######### Collect flags for readability ########
################################################
B_FLAGS := $(CPPFLAGS) $(BENCH_FLAGS) $(LDFLAGS) $(BENCH_LDFLAGS)
B_LIBS := $(LIBS) $(BENCH_LIBS)
FLAGS := $(CFLAGS)

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
	@$(CC) $(OPTFLAGS) $(FLAGS) -fverbose-asm -g -S -o $@ $< $(LIBS)

$(ADIR)/%.s: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(@D)/.
	@echo "---- Assembling: $@"
	@$(CC) $(OPTFLAGS) $(B_FLAGS) -S -o $@ $< $(B_LIBS)

## Building Objects
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(CC) $(OPTFLAGS) $(CFLAGS) -c -o $@ $<

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(CC) $(OPTFLAGS) $(B_FLAGS) -c -o $@ $<

## Linking main
$(BDIR)/$(EXE).a: $(OBJS) $(MAIN) | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@"
	@$(CC) $(OPTFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(MAIN) $(LIBS)

$(BDIR)/B_$(EXE).a: $(OBJS) $(B_MAIN) | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@ $(*F) "
	@$(CC) $(OPTFLAGS) $(CPPFLAGS) $(B_FLAGS) -o $@ $^ $(B_LIBS)


.PHONY: _start _basic _end
base: _start $(BDIR)/B_$(EXE).a $(BDIR)/$(EXE).a $(A_FILES) _end

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

$(OADIR)/$(1)/%.s: $(SDIR)/%.c $(DEPS) | $$(FOLDER)
	@echo "---- Assembling: $$@"
	@$(CC) $$($(1)FLAGS) $$(FLAGS) $$(ASM_FLAGS) -S -o $$@ $$< $$(LIBS)

$(OADIR)/$(1)/%.s: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(FOLDER)
	@echo "---- Assembling: $$@ $$(@D)/."
	@$(CC) $($(1)FLAGS) $(B_FLAGS) -S -o $$@ $$< $(B_LIBS)

## Building Objects
$(OODIR)/$(1)/%.o: $$(SDIR)/%.c $$(DEPS) | $$(FOLDER)
	@echo "---- Building $$@" 
	@$(CC) $$($(1)FLAGS) $(CFLAGS) -c -o $$@ $$<

$(OODIR)/$(1)/%.o: $$(SDIR)/%.cpp $$(DEPS) $$(BENCH_DEPS)  | $$(FOLDER)
	@echo "---- Building $$@"
	@$(CC) $$($(1)FLAGS) $$(B_FLAGS) -c -o $$@ $$<

## Linking bechmark
$(BDIR)/$(1)/$(EXE).a: $$($(1)OBJS) $$($(1)MAIN) | $$(RDIR)/. $$(FOLDER).
	@echo "++ Linking $$@ "
	@$(CC) $$($(1)FLAGS) $$(CFLAGS) $$(LDFLAGS) -o $$@ $$^ $$(LIBS)

$(BDIR)/$(1)/B_$(EXE).a: $$($(1)OBJS) $$($(1)B_MAIN) | $$(RDIR)/. $$(FOLDER)
	@echo "++ Linking $$@"
	@$(CC) $$($(1)FLAGS) $$(CPPFLAGS) $$(B_FLAGS) -o $$@ $$^ $(B_LIBS)

_all_opt_bench += B$(1)
_all_opt_run += $(1)
_all_opt_asm += $(1)asm
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
base: _basic project
project: o0 o2 o2n
all: base opt-all

.PHONY: opt-all oall on oNum
opt-all: $(_all_opt_bench) $(_all_opt_run) $(_all_opt_asm)
oall: o0 o1 o2 o3 o0n o1n o2n o3n og ogn os osn
oNum: o0 o1 o2 o3
onNum: o0n o1n o2n o3n


###############################################
######## General clean-up Rules ###############
###############################################

.PHONY: clean full-clean clean-results clean-celero clean-folders
clean:
	rm -f $(ODIR)/*.o $(ODIR)/*/*.o $(OODIR)/*.o $(OODIR)/*/*.o $(BDIR)/*.a $(BDIR)/*/*.a $(ADIR)/*.s $(ADIR)/*/*.s

clean-results:
	rm -fr $(RDIR)/*

clean-folders:
	rm -fr $(ADIR) $(OADIR) $(BDIR) $(ODIR) $(OODIR) $(RDIR)

full-clean: clean clean-results clean-folders clean-celero
