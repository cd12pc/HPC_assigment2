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
BO_DIR :=./obj/bench
# Optimization Object Directory (Dynamic)
OODIR :=./obj_o
BOO_DIR :=./obj_o/bench
# Binary Directory (Dynamic)
BDIR :=./bin
BB_DIR :=./bin/bench
# Asembler output Directory (Dynamic)
ADIR :=./asm
# Assmebelr Optimization output directory (Dynamic)
OADIR :=./asm_o
# Test source directory
TS_DIR :=./src/test
TB_DIR :=./bin/test
TO_DIR :=./obj/test

RDIR := ./results

###########################
####### Normal Libaries ###
###########################

#Libaries
LIBS := 
#Benchamrking Libaries
BENCH_LIBS = -lstdc++ -lm


#####################
####### FLAGS ######
####################

# C Specific flags
CFLAGS :=-I$(IDIR) -fopenmp
# CPP Specific Flags
CPPFLAGS := $(CFLAGS)

#DEBUGGNING FLAGS (Turned off by default)
DEBUG_FLAGS = -g

# PROFILING FLAGS (Turned off by defualt)
PROF_FLAGS:= -g -pg


# Normal Optimiztion Flags
OPTFLAGS := -O2 -march=native

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
_DEPS :=  cell_distances.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))
BENCH_DEPS :=

#Exceutable file name
EXE := run

# Objects to build
_OBJS :=  cell_distances.o
OBJS := $(patsubst %.o,$(ODIR)/%.o,$(_OBJS))
D_OBJS := $(patsubst %.o,$(ODIR)/%.d.o,$(_OBJS))
P_OBJS := $(patsubst %.o,$(ODIR)/%.p.o,$(_OBJS))

# Main file to build
_MAIN := main.o
MAIN := $(patsubst %.o,$(ODIR)/%.o,$(_MAIN))
D_MAIN := $(patsubst %.o,$(ODIR)/%.d.o,$(_MAIN))
P_MAIN := $(patsubst %.o,$(ODIR)/%.p.o,$(_MAIN))


_BENCHMARK_FILES:= benchmark.o
B_R_FILES := $(patsubst %.o,$(BB_DIR)/%.a,$(notdir $(_BENCHMARK_FILES)))
DB_R_FILES := $(patsubst %.o,$(BB_DIR)/%.d.a,$(notdir $(_BENCHMARK_FILES)))
PB_R_FILES := $(patsubst %.o,$(BB_DIR)/%.p.a,$(notdir $(_BENCHMARK_FILES)))

# Testing main to build
_TEST_FILES := test.o
T_R_FILES := $(patsubst %.o,$(TB_DIR)/%.a,$(_TEST_FILES))
DT_R_FILES := $(patsubst %.o,$(TB_DIR)/%.d.a,$(_TEST_FILES))
PT_R_FILES := $(patsubst %.o,$(TB_DIR)/%.p.a,$(_TEST_FILES))

#Basic Assembler Files
A_FILES := $(patsubst %.o,$(ADIR)/%.s,$(_OBJS))
A_FILES += $(patsubst %.o,$(ADIR)/%.s,$(_B_MAIN))
A_FILES += $(patsubst %.o,$(ADIR)/test/%.s,$(_B_MAIN))
A_FILES += $(patsubst %.o,$(ADIR)/bench/%.s,$(_BENCHMARK_FILES))

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

#################################################
######### Build Unity for testing ##############
################################################
UNITYDIR := lib/Unity
UNITYDIR_B := lib/UnityBuild

TEST_FLAGS += -I./$(UNITYDIR)/src
TEST_LIBS += -L$(UNITYDIR_B) -lunity
TEST_LDFLAGS = -Wl,-rpath -Wl,$(UNITYDIR_B)
TEST_DEPS += $(UNITYDIR_B)/libunity.a

$(UNITYDIR)/CMakeLists.txt:
	@echo "---- Cloning Unity"
	@mkdir -p $(LDIR)
	@rm -rf $(UNITYDIR)
	@git clone --depth=1 https://github.com/ThrowTheSwitch/Unity.git ./$(UNITYDIR)

$(UNITYDIR_B)/libunity.a: $(UNITYDIR)/CMakeLists.txt
	@echo "========================================================="
	@echo "==================== BUILDING UNITY ===================="
	@echo "========================================================="
	@cmake -S $(UNITYDIR) -B $(UNITYDIR_B)
	@$(MAKE) -C $(UNITYDIR_B)
	@echo "========================================================="
	@echo "==================== DONE UNITY ===================="
	@echo "========================================================="

.PHONY: clean-unity
clean-unity:
	@echo "---- Cleaning Unity"
	@rm -rf $(UNITYDIR) $(UNITYDIR_B)
################################################
######### Collect flags for readability ########
################################################
B_FLAGS = $(CPPFLAGS) $(BENCH_FLAGS) $(LDFLAGS) $(BENCH_LDFLAGS)
FLAGS = $(CFLAGS) $(LDFLAGS)
T_FLAGS := $(CFLAGS) $(TEST_FLAGS) $(LDFLAGS) $(TEST_LDFLAGS)

B_LIBS := $(LIBS) $(BENCH_LIBS)
T_LIBS := $(LIBS) $(TEST_LIBS)
##################################################################################################
########################## FOLDER RULES ##########################################################
##################################################################################################

.PRECIOUS: $(BDIR)/. $(BDIR)%/. $(ADIR)/. $(ADIR)%/. 
.PRECIOUS: $(ODIR)/. $(ODIR)%/. $(OODIR)/. $(OODIR)%/.
.PRECIOUS: $(OADIR)/. $(OADIR)%/. $(OADIR)/. $(OADIR)/.
.PRECIOUS: $(TB_DIR)/. $(TB_DIR)%/. $(TO_DIR)/. $(TO_DIR)%/.

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

$(TB_DIR)/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(TB_DIR)%/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(TO_DIR)/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(TO_DIR)%/.:
	@echo "--== Creating $@"
	@mkdir -p $@

$(RDIR)/.:
	@echo "--== Creeating $@"
	@mkdir -p $@

########################################
####### Random Functions ###############
########################################
#F = $(1) $(1)
RBASE =$(patsubst ./$(2)/%,.%,$(1))


#############################################
########## Build assmbler filxes ##################
#############################################
$(ADIR)/%.s: $(SDIR)/%.c $(DEPS) | $$(@D)/.
	@echo "---- Assembling: $@"
	@$(strip $(CC) $(OPTFLAGS) $(FLAGS) $(ASM_FLAGS) -S -o $@ $< $(LIBS))

$(ADIR)/%.s: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(@D)/.
	@echo "---- Assembling: $@"
	@$(strip $(CC) $(OPTFLAGS) $(B_FLAGS) $(ASM_FLAGS) -S -o $@ $< $(B_LIBS))

#############################################
##########  Build standard files ############
#############################################

## Building object files
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(FLAGS) -c -o $@ $<)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(B_FLAGS) -c -o $@ $<)

## Linking main
$(BDIR)/$(EXE).a: $(OBJS) $(MAIN) | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@"
	@$(strip $(CC) $(OPTFLAGS) $(FLAGS) -o $@ $(OBJS) $(MAIN) $(LIBS))

$(B_R_FILES): $(OBJS) $(BO_DIR)/$$(basename $$(@F)).o | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@"
	$(strip $(CC) $(OPTFLAGS) $(B_FLAGS) -o $@ $^ $(B_LIBS))

.PHONY: _start _basic _end
_basic: _start $(BDIR)/$(EXE).a $(B_R_FILES) $(A_FILES) _t_basic _end
_start:
	@echo "========= Building Base ========="
_end:
	@echo "========= Finsihed Base ========="



#############################################
##########  Build debugging files ###########
#############################################

## Building Objects
$(ODIR)/%.d.o: $(SDIR)/%.c $(DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(DEBUG_FLAGS) $(FLAGS) -c -o $@ $<)

$(ODIR)/%.d.o: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(DEBUG_FLAGS) $(B_FLAGS) -c -o $@ $<)

## Linking main
$(BDIR)/$(EXE).d.a: $(D_OBJS) $(D_MAIN) | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@"
	@$(strip $(CC) $(OPTFLAGS) $(DEBUG_FLAGS) $(FLAGS) -o $@ $^ $(LIBS))

$(DB_R_FILES): $(D_OBJS) $(BO_DIR)/$$(basename $$(@F)).o | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@ $(*F) "
	@$(strip $(CC) $(OPTFLAGS) $(DEBUG_FLAGS) $(B_FLAGS)  -o $@ $^ $(B_LIBS))


.PHONY: _d_start _d_basic _d_end
_d_basic: _d_start $(A_FILES) $(DB_R_FILES) $(BDIR)/$(EXE).d.a _dt_basic _d_end

_d_start:
	@echo "Building========= Building Debug Base ========="

_d_end:
	@echo "========= Finsihed Debug Base ========="
	@echo ""

#############################################
##########  Build PROFILING Files ###########
#############################################

## Building Objects
$(ODIR)/%.p.o: $(SDIR)/%.c $(DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(PROF_FLAGS) $(FLAGS) -c -o $@ $<)

$(ODIR)/%.p.o: $(SDIR)/%.cpp $(DEPS) $(BENCH_DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(PROF_FLAGS) $(B_FLAGS) -c -o $@ $<)

## Linking main
$(BDIR)/$(EXE).p.a: $(P_OBJS) $(P_MAIN) | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@"
	@$(strip $(CC) $(OPTFLAGS) $(PROF_FLAGS) $(FLAGS) -o $@ $^ $(LIBS))

$(PB_R_FILES): $(P_OBJS) $(BO_DIR)/$$(basename $$(@F)).o | $(RDIR)/. $$(@D)/.
	@echo "++ Linking $@"
	@$(strip $(CC) $(OPTFLAGS) $(PROF_FLAGS) $(B_FLAGS) -o $@ $^ $(B_LIBS))

.PHONY: _p_start _p_basic _p_end
_p_basic: _p_start $(A_FILES) $(PB_R_FILES) $(BDIR)/$(EXE).p.a _pt_basic _p_end

_p_start:
	@echo "========= Building Debug Base ========="

_p_end:
	@echo "========= Finsihed Debug Base ========="
	@echo ""

#################################################################################
############## Testing Rules ####################################################
#################################################################################


## TESTING RULES
$(TO_DIR)/%.o: $(TS_DIR)/%.c $(DEPS) $(TEST_DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(T_FLAGS) -c -o $@ $<)

$(T_R_FILES): $(TO_DIR)/$$(basename $$(@F)).o $(OBJS) | $$(@D)/.
	@echo "++ Linking $@"
	@$(strip $(CC) $(OPTFLAGS) $(DEBUG_FLAGS) $(T_FLAGS) -o $@ $^ $(T_LIBS))

## Debuging test
$(TO_DIR)/%.d.o: $(TS_DIR)/%.c $(DEPS) $(TEST_DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(DEBUG_FLAGS) $(T_FLAGS) -c -o $@ $<)

$(DT_R_FILES): $(TO_DIR)/$$(basename $$(@F)).o $(D_OBJS) | $$(@D)/.
	@echo "++ Linking $@"
	@$(strip $(CC) $(OPTFLAGS) $(DEBUG_FLAGS) $(T_FLAGS) -o $@ $^ $(T_LIBS))

## Profiling test
$(TO_DIR)/%.p.o: $(TS_DIR)/%.c $(DEPS) $(TEST_DEPS) | $$(@D)/.
	@echo "---- Building $@"
	@$(strip $(CC) $(OPTFLAGS) $(PROF_FLAGS) $(T_FLAGS) -c -o $@ $<)

$(PT_R_FILES): $(TO_DIR)/$$(basename $$(@F)).o $(P_OBJS) | $$(@D)/.
	@echo "++ Linking $@"
	@$(strip $(CC) $(OPTFLAGS) $(PROF_FLAGS) $(T_FLAGS) -o $@ $^ $(T_LIBS))

_pt_basic: $(PT_R_FILES)
_dt_basic: $(DT_R_FILES)
_t_basic: $(T_R_FILES)


##################################################################################################
########################## BUILD RULES for different flags #######################################
##################################################################################################



BASENAME = $$(basename $$(@F))
FOLDER = $$(@D)/.
define FLAG_SET
$(1)BO_DIR = $(OODIR)/$(1)/bench

$(1)FLAGS := $(2)
$(1)ASM := $$(patsubst %.o,$$(OADIR)/$(1)/%.s,$$(_OBJS))

$(1)OBJS := $$(patsubst %.o,$$(OODIR)/$(1)/%.o,$$(_OBJS))
$(1)D_OBJS := $$(patsubst %.o,$$(OODIR)/$(1)/%.d.o,$$(_OBJS))
$(1)P_OBJS := $$(patsubst %.o,$$(OODIR)/$(1)/%.p.o,$$(_OBJS))

$(1)MAIN := $$(patsubst %.o,$$(OODIR)/$(1)/%.o,$$(_MAIN))
$(1)D_MAIN := $$(patsubst %.o,$$(OODIR)/$(1)/%.d.o,$$(_MAIN))
$(1)P_MAIN := $$(patsubst %.o,$$(OODIR)/$(1)/%.p.o,$$(_MAIN))

$(1)B_MAIN :=  $$(patsubst %.o,$$(OODIR)/$(1)/%.o,$$(_B_MAIN))
$(1)DB_MAIN :=  $$(patsubst %.o,$$(OODIR)/$(1)/%.d.o,$$(_B_MAIN))
$(1)PB_MAIN :=  $$(patsubst %.o,$$(OODIR)/$(1)/%.p.o,$$(_B_MAIN))

$(1)B_R_FILES := $$(patsubst %.o,$$(BB_DIR)/$(1)/%.a,$$(_BENCHMARK_FILES))
$(1)DB_R_FILES := $$(patsubst %.o,$$(BB_DIR)/$(1)/%.d.a,$$(_BENCHMARK_FILES))
$(1)PB_R_FILES := $$(patsubst %.o,$$(BB_DIR)/$(1)/%.p.a,$$(_BENCHMARK_FILES))

$(1)T_R_FILES := $$(patsubst %.o,$$(TB_DIR)/$(1)/%.a,$$(_TEST_FILES))
$(1)DT_R_FILES := $$(patsubst %.o,$$(TB_DIR)/$(1)/%.d.a,$$(_TEST_FILES))
$(1)PT_R_FILES := $$(patsubst %.o,$$(TB_DIR)/$(1)/%.p.a,$$(_TEST_FILES))

.PHONY: _$(1)START _D$(1)START _P$(1)START
_$(1)START:
	@echo "========= Building $(1) ========="
	@echo "$$($(1)OBJS)"
	@echo "$$($(1)B_R_FILES)"
	@echo "$$($(1)MAIN) $$($(1)OBJS) "
	@echo "$(BO_DIR)"
	@echo "$(OODIR)"
	@echo "$(ODIR)"
_D$(1)START:
	@echo "========= Building Debugging $(1) ========="

_P$(1)START:
	@echo "========= Building Profling $(1) ========="

.PHONY: _$(1)END _D$(1)END _P$(1)END
_$(1)END:
	@echo "========= Finsihed $(1) ========="
	@echo ""

_D$(1)END:
	@echo "========= Finsihed Debugging $(1) ========="
	@echo ""

_P$(1)END:
	@echo "========= Finsihed Profiling $(1) ========="
	@echo ""


### Assebler code generation
$(OADIR)/$(1)/%.s: $(SDIR)/%.c $$(DEPS) | $$(FOLDER)
	@echo "---- Assembling: $$@"
	@$$(strip $(CC) $$($(1)FLAGS) $$(FLAGS) $$(ASM_FLAGS) -S -o $$@ $$< $$(LIBS))

$(OADIR)/$(1)/%.s: $(SDIR)/%.cpp $$(DEPS) $(BENCH_DEPS) | $$(FOLDER)
	@echo "---- Assembling: $$@ $$(@D)/."
	@$$(strip $(CC) $($(1)FLAGS) $$(B_FLAGS) -S -o $$@ $$< $(B_LIBS))

############ STANDARD BUILD ##############
## Building Objects
$$(OODIR)/$(1)/%.o: $$(SDIR)/%.c $$(DEPS) | $$(FOLDER)
	@echo "---- Building $$@" 
	@$$(strip $(CC) $$($(1)FLAGS) $$(FLAGS) -c -o $$@ $$<)

$$(OODIR)/$(1)/%.o: $$(SDIR)/%.cpp $$(DEPS) $$(BENCH_DEPS)  | $$(FOLDER)
	@echo "---- Building $$@"
	@$$(strip $(CC) $$($(1)FLAGS) $$(B_FLAGS) -c -o $$@ $$<)

## Linking bechmark
$(BDIR)/$(1)/$(EXE).a: $$($(1)OBJS) $$($(1)MAIN) | $$(RDIR)/. $$(FOLDER)
	@echo "++ Linking $$@ "
	@$$(strip $(CC) $$($(1)FLAGS) $$(FLAGS) -o $$@ $$^ $$(LIBS))

$$($(1)B_R_FILES): $$($(1)OBJS) $$($(1)BO_DIR)/$$(BASENAME).o | $(RDIR)/. $$(FOLDER)
	@echo "++ Linking $$@"
	@$$(strip $$(CC) $$($(1)FLAGS) $$(B_FLAGS) -o $$@ $$^ $$(B_LIBS))



########### DEBUG BUILD #####################
## Building Objects
$(OODIR)/$(1)/%.d.o: $$(SDIR)/%.c $$(DEPS) | $$(FOLDER)
	@echo "---- Building $$@" 
	@$$(strip $(CC) $$($(1)FLAGS) $$(DEBUG_FLAGS) $$(FLAGS) -c -o $$@ $$<)

$(OODIR)/$(1)/%.d.o: $$(SDIR)/%.cpp $$(DEPS) $$(BENCH_DEPS)  | $$(FOLDER)
	@echo "---- Building $$@"
	@$$(strip $(CC) $$($(1)FLAGS) $$(DEBUG_FLAGS) $$(B_FLAGS) -c -o $$@ $$<)

## Linking bechmark
$(BDIR)/$(1)/$(EXE).d.a: $$($(1)D_OBJS) $$($(1)D_MAIN) | $$(RDIR)/. $$(FOLDER)
	@echo "++ Linking $$@ "
	@$$(strip $(CC) $$($(1)FLAGS) $$(DEBUG_FLAGS) $$(FLAGS) -o $$@ $$^ $$(LIBS))

$$($(1)DB_R_FILES): $$($(1)D_OBJS) $$($(1)BO_DIR)/$$(BASENAME).o | $(RDIR)/. $$(FOLDER)
	@echo "++ Linking $$@"
	@$$(strip $$(CC) $$($(1)FLAGS) $$(DEBUG_FLAGS) $$(B_FLAGS) -o $$@ $$^ $$(B_LIBS))


########### PERF BUILD #####################
## Building Objects
$(OODIR)/$(1)/%.p.o: $$(SDIR)/%.c $$(DEPS) | $$(FOLDER)
	@echo "---- Building $$@" 
	@$$(strip $(CC) $$($(1)FLAGS) $$(PROF_FLAGS) $$(FLAGS) -c -o $$@ $$<)

$(OODIR)/$(1)/%.p.o: $$(SDIR)/%.cpp $$(DEPS) $$(BENCH_DEPS)  | $$(FOLDER)
	@echo "---- Building $$@"
	@$$(strip $(CC) $$($(1)FLAGS) $$(PROF_FLAGS) $$(B_FLAGS) -c -o $$@ $$<)

## Linking bechmark
$(BDIR)/$(1)/$(EXE).p.a: $$($(1)P_OBJS) $$($(1)P_MAIN) | $$(RDIR)/. $$(FOLDER).
	@echo "++ Linking $$@ "
	@$$(strip $(CC) $$($(1)FLAGS) $$(PROF_FLAGS) $$(FLAGS)-o $$@ $$^ $$(LIBS))

$$($(1)PB_R_FILES): $$($(1)P_OBJS) $$($(1)BO_DIR)/$$(BASENAME).o | $(RDIR)/. $$(FOLDER)
	@echo "++ Linking $$@"
	@$$(strip $$(CC) $$($(1)FLAGS) $$(DEBUG_FLAGS) $$(B_FLAGS) -o $$@ $$^ $$(B_LIBS))

#################################################################################
############## Testing Rules ####################################################
#################################################################################
$$(TO_DIR)/$(1)/%.o: $$(TS_DIR)/%.c $$(DEPS) $$(TEST_DEPS) | $$(FOLDER)
	@echo "---- Building $$@"
	@$(strip $(CC) $$($(1)FLAGS) $(T_FLAGS) -c -o $$@ $$<)

$$($(1)T_R_FILES): $(TO_DIR)/$(1)/$$(BASENAME).o $$($(1)OBJS) | $$(FOLDER)
	@echo "++ Linking $$@"
	@$(strip $(CC) $$($(1)FLAGS) $(DEBUG_FLAGS) $(T_FLAGS) -o $$@ $$^ $(T_LIBS))

## Debuging test
$$(TO_DIR)/$(1)/%.d.o: $$(TS_DIR)/%.c $$(DEPS) $$(TEST_DEPS) | $$(FOLDER)
	@echo "---- Building $$@"
	@$(strip $(CC) $$($(1)FLAGS) $(DEBUG_FLAGS) $(T_FLAGS) -c -o $$@ $$<)

$$($(1)DT_R_FILES): $$(TO_DIR)/$$(BASENAME).o $$($(1)D_OBJS) | $$(FOLDER)
	@echo "++ Linking $$@"
	@$(strip $(CC) $$($(1)FLAGS) $(DEBUG_FLAGS) $(T_FLAGS) -o $$@ $$^ $(T_LIBS))

## Profiling test
$$(TO_DIR)/$(1)/%.p.o: $$(TS_DIR)/%.c $$(DEPS) $$(TEST_DEPS) | $$(FOLDER)
	@echo "---- Building $$@"
	@$(strip $$(CC) $$($(1)FLAGS) $$(PROF_FLAGS) $$(T_FLAGS) -c -o $$@ $$<)

$$($(1)PT_R_FILES): $(TO_DIR)/$$(BASENAME).o $$($(1)P_OBJS) | $$(FOLDER)
	@echo "++ Linking $$@"
	@$(strip $(CC) $$($(1)FLAGS) $(PROF_FLAGS) $(T_FLAGS) -o $$@ $$^ $(T_LIBS))



.PHONY: $(1) D$(1) P$(1) _$(1)asm PT$(1) DT$(1)
$(1): _$(1)START $(1)asm $$(BDIR)/$(1)/$$(EXE).a T$(1) $$($(1)B_R_FILES) _$(1)END
D$(1): _D$(1)START $(1)asm $$(BDIR)/$(1)/$$(EXE).d.a $$($(1)DB_R_FILES) DT$(1) _D$(1)END
P$(1): _P$(1)START  $(1)asm $$(BDIR)/$(1)/$$(EXE).p.a $$($(1)PB_R_FILES) PT$(1) _P$(1)END
T$(1): $$($(1)T_R_FILES)
DT$(1): $$($(1)DT_R_FILES)
PT$(1): $$($(1)PT_R_FILES)

$(1)asm: $$($(1)ASM)



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


.PHONY: base base_test project all
base: _basic
project: o0 o2 o2n
all: _basic oall

.PHONY: debug-base debug-project debug-all
debug-base: _d_basic
debug-project: Do0 Do2 Do2n
debug-all: _d_basic debug-oall 

.PHONY: prof-base prof-project prof-all
prof-base: _p_basic
prof-project: Po0 Po2 Po2n
prof-all: _p_basic prof-oall

.PHONY: oall oNum oNum-n
oall: o0 o1 o2 o3 o0n o1n o2n o3n og ogn os osn
oNum: o0 o1 o2 o3
oNum-n: o0n o1n o2n o3n

.PHONY: debug-oall debug-oNum debug-oNum-n
debug-oall: Do0 Do1 Do2 Do3 Do0n Do1n Do2n Do3n Dog Dogn Dos Dosn
debug-oNum: Do0 Do1 Do2 Do3
debug-oNum-n: Do0n Do1n Do2n Do3n

.PHONY: prof-oall prof-oNum prof-oNum-n
prof-oall: Po0 Po1 Po2 Po3 Po0n Po1n Po2n Po3n Pog Pogn Pos Posn
prof-oNum: Po0 Po1 Po2 Po3
prof-oNum-n: Po0n Po1n Po2n Po3n

everything: all debug-all prof-all
###############################################
######## General clean-up Rules ###############
###############################################

.PHONY: clean clean_obj clean_asm clean_bin clean-results clean-all
clean: clean_obj clean_asm clean_bin

clean_obj:
	@echo "---- Cleaning Objects"
	@rm -f $(ODIR)/*.o $(ODIR)/*/*.o
	@echo "---- Cleaning Optimization Objects"
	@rm -f $(OODIR)/*.o $(OODIR)/*/*.o 
	@echo "---- Cleaning Test Objects"
	@rm -f $(TO_DIR)/*.o $(TO_DIR)/*/*.o 

clean_asm:
	@echo "---- Cleaning Assembler code"
	@rm -f $(ADIR)/*.s $(ADIR)/*/*.s
	@echo "---- Cleaning Optimization Assembler code"
	@rm -f $(OADIR)/*.s $(OADIR)/*/*.s

clean_bin:
	@echo "---- Cleaning Binaries"
	@rm -f $(BDIR)/*.a $(BDIR)/*/*.a 
	@echo "---- Cleaning Test Binaries"
	@rm -f $(TB_DIR)/*.a $(TB_DIR)/*/*.a 

clean-results:
	@echo "---- Cleaning Results"
	@rm -fr $(RDIR)/*
	

clean-folders:
	@echo "---- Removing Folders"
	@rm -fr $(ADIR) $(OADIR) $(BDIR) $(ODIR) $(OODIR) $(RDIR) $(TB_DIR) $(TO_DIR)

clean-all: clean clean-results clean-folders clean-celero clean-unity
