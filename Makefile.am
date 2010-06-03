bin_PROGRAMS = simMc2Mesh

noinst_LIBRARIES = libIris.a

OBJDIR = $(TOPDIR)/build
BUILDDIR = $(TOPDIR)/build
CXX_FLAGS = -D_STLP_DEBUG -I source -pthread -ansi -Wall -g -gstabs+ -O3 -funroll-loops -fkeep-inline-functions -fpermissive
#DEBUG_FLAGS = -D_DEBUG -D_DEBUG_ROUTER 
DEBUG_FLAGS = -D_DEBUG -DDEBUG -D_DEBUG_ROUTER -D_DEBUG_INTERFACE #-fprofile-arcs -ftest-coverage
IRIS_SOURCES = \
		  source/kernel/link.cc \
		  source/kernel/simulator.cc \
		  source/kernel/component.cc \
		  source/data_types/impl/irisEvent.cc \
		  source/data_types/impl/flit.cc \
		  source/data_types/impl/util.cc \
		  source/data_types/impl/highLevelPacket.cc \
		  source/data_types/impl/lowLevelPacket.cc \
		  source/components/interfaces/router.cc \
		  source/components/interfaces/processor.cc \
		  source/components/interfaces/interface.cc \
		  source/components/interfaces/irisLink.cc \
		  source/components/interfaces/networkComponent.cc \
		  source/components/impl/genericBuffer.cc \
		  source/components/impl/genericRC.cc \
		  source/components/impl/genericCrossbar.cc \
		  source/components/impl/genericInterface.cc \
		  source/components/impl/genericData.cc \
		  source/components/impl/genericLink.cc \
		  source/components/impl/ptop_swa.cc \
		  source/components/impl/genericRouterNoVcs.cc \
		  source/components/impl/genericRouterVct.cc \
		  source/components/impl/genericFlatMc.cc \
		  source/components/impl/genericTPG.cc \
		  source/memctrl/mshr.cc \
		  source/memctrl/request.cc \
		  source/frontend/impl/mesh.cc 


## simMc2Mesh
simMc2Mesh_CXXFLAGS = $(CXX_FLAGS)

simMc2Mesh_SOURCES = \
	       source/frontend/impl/simMc2Mesh.cc
simMc2Mesh_LDADD = libIris.a

##libIris
libIris_a_CXXFLAGS = $(CXX_FLAGS)
libIris_a_SOURCES = $(IRIS_SOURCES)

##Headers
noinst_HEADERS = \
		 source/kernel/simulator.h \
		 source/kernel/link.h \
		 source/kernel/component.h \
		 source/data_types/impl/irisEvent.h \
		 source/data_types/impl/flit.h \
		 source/data_types/impl/util.h \
		 source/data_types/impl/highLevelPacket.h \
		 source/data_types/impl/lowLevelPacket.h \
		 source/components/interfaces/crossbar.h \
		 source/components/interfaces/addressDecoder.h \
		 source/components/interfaces/interface.h \
		 source/components/interfaces/buffer.h \
		 source/components/interfaces/genericComponentHeader.h \
		 source/components/interfaces/virtualChannelArbiter.h \
		 source/components/interfaces/router.h \
		 source/components/interfaces/portArbiter.h \
		 source/components/interfaces/processor.h \
		 source/components/interfaces/irisLink.h \
		 source/components/interfaces/networkComponent.h \
		 source/components/impl/genericRC.h \
		 source/components/impl/genericCrossbar.h \
		 source/components/impl/genericBuffer.h \
		 source/components/impl/genericInterface.h \
		 source/components/impl/genericLink.h \
		 source/components/impl/ptop_swa.h \
		 source/components/impl/genericRouterNoVcs.h \
		 source/components/impl/genericRouterVct.h \
		 source/components/impl/genericTPG.h \
		 source/components/impl/genericFlatMc.h \
		 source/frontend/impl/mesh.h \
		 source/tests/clock.h \
		 source/memctrl/mshr.h \
		 source/memctrl/request.h 
