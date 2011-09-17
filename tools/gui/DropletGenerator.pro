TEMPLATE = lib

CONFIG += dll debug

MOC_DIR = moc_obj
OBJECTS_DIR = obj
DESTDIR=./libs

# ScenarioGenerator related headers
HEADERS  += src/Objects/Object.h
HEADERS  += src/Generators/Generator.h
HEADERS  += src/Parameters/Parameter.h
HEADERS  += src/Parameters/ParameterWithValue.h
HEADERS  += src/Parameters/ParameterWithIntValue.h
HEADERS  += src/Parameters/ParameterWithDoubleValue.h
HEADERS  += src/Parameters/ParameterWithChoice.h
HEADERS  += src/Parameters/ParameterCollection.h
HEADERS  += src/IO/WriteOutput.h
HEADERS  += src/Tokenize.h

# MD Generator headers
HEADERS  += generators/MDGenerator.h
HEADERS  += generators/DropletGenerator.h
SOURCES  += generators/MDGenerator.cpp
SOURCES  += generators/DropletGenerator.cpp
HEADERS  += generators/common/PMFileReader.h
HEADERS  += generators/common/ComponentParameters.h
HEADERS  += generators/common/MardynConfiguration.h
HEADERS  += generators/common/MardynConfigurationParameters.h
HEADERS  += generators/common/MardynConfigLegacyWriter.h
HEADERS  += generators/common/DrawableMolecule.h
HEADERS  += generators/common/eig3.h
HEADERS  += generators/common/PrincipalAxisTransform.h

SOURCES  += generators/common/PMFileReader.cpp
SOURCES  += generators/common/ComponentParameters.cpp
SOURCES  += generators/common/MardynConfiguration.cpp
SOURCES  += generators/common/MardynConfigurationParameters.cpp
SOURCES  += generators/common/DrawableMolecule.cpp
SOURCES  += generators/common/MardynConfigLegacyWriter.cpp
SOURCES  += generators/common/eig3.cpp
SOURCES  += generators/common/PrincipalAxisTransform.cpp

LIBS += -L. -lMardyn

INCLUDEPATH += ./src/
INCLUDEPATH += ../../src/
INCLUDEPATH += $(VTKINCLUDEPATH)