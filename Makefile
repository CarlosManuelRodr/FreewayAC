WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -std=c++11
RESINC = 
LIBDIR = 
LIB =  
LDFLAGS = -pthread

INC_RELEASE =  $(INC)
CFLAGS_RELEASE =  $(CFLAGS) -O2 
RESINC_RELEASE =  $(RESINC)
RCFLAGS_RELEASE =  $(RCFLAGS)
LIBDIR_RELEASE =  $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS)
OBJDIR_RELEASE = obj
DEP_RELEASE = 
OUT_RELEASE = bin/FreewayAC


OBJ_RELEASE := 
OBJ_RELEASE += \
$(OBJDIR_RELEASE)/Auxiliar.o \
$(OBJDIR_RELEASE)/BmpWriter.o \
$(OBJDIR_RELEASE)/CellularAutomata.o \
$(OBJDIR_RELEASE)/Experiments.o \
$(OBJDIR_RELEASE)/Export.o \
$(OBJDIR_RELEASE)/main.o \


all: release

clean: clean_release


before_release: 
	@echo ""
	@echo "################################"
	@echo "#                              #"
	@echo "#     Building FreewayAC       #"
	@echo "#                              #"
	@echo "################################"
	@echo ""
	@test -d bin/ || mkdir -p bin/
	@test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 
	@echo ""
	@echo "#############################"
	@echo "#                           #"
	@echo "#            Done.          #"
	@echo "#                           #"
	@echo "#############################"
	@echo ""

release: before_release out_release after_release

out_release: $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(OBJ_RELEASE) $(LDFLAGS_RELEASE) $(LIBDIR_RELEASE) $(LIB_RELEASE) -o $(OUT_RELEASE)


$(OBJDIR_RELEASE)/Auxiliar.o: FreewayAC/Auxiliar.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FreewayAC/Auxiliar.cpp -o $(OBJDIR_RELEASE)/Auxiliar.o

$(OBJDIR_RELEASE)/BmpWriter.o: FreewayAC/BmpWriter.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FreewayAC/BmpWriter.cpp -o $(OBJDIR_RELEASE)/BmpWriter.o

$(OBJDIR_RELEASE)/CellularAutomata.o: FreewayAC/CellularAutomata.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FreewayAC/CellularAutomata.cpp -o $(OBJDIR_RELEASE)/CellularAutomata.o

$(OBJDIR_RELEASE)/Experiments.o: FreewayAC/Experiments.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FreewayAC/Experiments.cpp -o $(OBJDIR_RELEASE)/Experiments.o

$(OBJDIR_RELEASE)/Export.o: FreewayAC/Export.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FreewayAC/Export.cpp -o $(OBJDIR_RELEASE)/Export.o

$(OBJDIR_RELEASE)/main.o: FreewayAC/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FreewayAC/main.cpp -o $(OBJDIR_RELEASE)/main.o




clean_release: 
	@echo "########################################################"
	@echo "#                                                      #"
	@echo "#  Cleaning up                                         #"
	@echo "#                                                      #"
	@echo "########################################################"
	@echo ""
	@rm -f $(OBJ_RELEASE) $(OUT_RELEASE)


.PHONY: before_release after_release clean_release