##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=FreewayAC
ConfigurationName      :=Release
WorkspacePath          := "/home/carlos/Documentos/Maestria/Sistemas_Dinamicos/FreewayAC/Codelite_Project"
ProjectPath            := "/home/carlos/Documentos/Maestria/Sistemas_Dinamicos/FreewayAC/Codelite_Project"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Carlos
Date                   :=05/17/15
CodeLitePath           :="/home/carlos/.codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="FreewayAC.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -O2 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(ObjectSuffix) $(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(ObjectSuffix) $(IntermediateDirectory)/FreewayAC_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/FreewayAC_mtrand.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Release || $(MakeDirCommand) ./Release

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(ObjectSuffix): ../FreewayAC/BmpWriter.cpp $(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/carlos/Documentos/Maestria/Sistemas_Dinamicos/FreewayAC/FreewayAC/BmpWriter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(DependSuffix): ../FreewayAC/BmpWriter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(DependSuffix) -MM "../FreewayAC/BmpWriter.cpp"

$(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(PreprocessSuffix): ../FreewayAC/BmpWriter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FreewayAC_BmpWriter.cpp$(PreprocessSuffix) "../FreewayAC/BmpWriter.cpp"

$(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(ObjectSuffix): ../FreewayAC/CellularAutomata.cpp $(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/carlos/Documentos/Maestria/Sistemas_Dinamicos/FreewayAC/FreewayAC/CellularAutomata.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(DependSuffix): ../FreewayAC/CellularAutomata.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(DependSuffix) -MM "../FreewayAC/CellularAutomata.cpp"

$(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(PreprocessSuffix): ../FreewayAC/CellularAutomata.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FreewayAC_CellularAutomata.cpp$(PreprocessSuffix) "../FreewayAC/CellularAutomata.cpp"

$(IntermediateDirectory)/FreewayAC_main.cpp$(ObjectSuffix): ../FreewayAC/main.cpp $(IntermediateDirectory)/FreewayAC_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/carlos/Documentos/Maestria/Sistemas_Dinamicos/FreewayAC/FreewayAC/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FreewayAC_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FreewayAC_main.cpp$(DependSuffix): ../FreewayAC/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FreewayAC_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FreewayAC_main.cpp$(DependSuffix) -MM "../FreewayAC/main.cpp"

$(IntermediateDirectory)/FreewayAC_main.cpp$(PreprocessSuffix): ../FreewayAC/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FreewayAC_main.cpp$(PreprocessSuffix) "../FreewayAC/main.cpp"

$(IntermediateDirectory)/FreewayAC_mtrand.cpp$(ObjectSuffix): ../FreewayAC/mtrand.cpp $(IntermediateDirectory)/FreewayAC_mtrand.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/carlos/Documentos/Maestria/Sistemas_Dinamicos/FreewayAC/FreewayAC/mtrand.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FreewayAC_mtrand.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FreewayAC_mtrand.cpp$(DependSuffix): ../FreewayAC/mtrand.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FreewayAC_mtrand.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FreewayAC_mtrand.cpp$(DependSuffix) -MM "../FreewayAC/mtrand.cpp"

$(IntermediateDirectory)/FreewayAC_mtrand.cpp$(PreprocessSuffix): ../FreewayAC/mtrand.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FreewayAC_mtrand.cpp$(PreprocessSuffix) "../FreewayAC/mtrand.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


