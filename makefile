#------- Makefile para la compilación de un programa en C -------
# Comando de compilacion
CC			:= g++

# Nombre del programa
TARGET		:= anomaly_detection

# Directorios de includes, codigo fuente, objetos y destino
SRCDIR		:= src
INCDIR		:= include
OBJDIR		:= bin
TARGETDIR	:= build

# Flags, librerias e includes
LIBPATH		:= /usr/local/lib
LIBS		:= -L$(LIBPATH) -lpthread -llivox_sdk_static -lgtest -lgmock

INCPATH		:= /usr/local/include
INC			:= -I$(INCDIR) -I$(INCPATH)

DEBUG_FLAG	:= -DDEBUG_LBAD
FLAGS		:= -Wall -g3

#----------------------------------------------------------------
# REGLAS - NO EDITAR
#----------------------------------------------------------------
# Funcion recursiva de inclusión de archivos
rwildcard	= $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SOURCES  	:= $(call rwildcard,$(SRCDIR)/main/,*.cc)
TEST		:= $(filter-out $(SRCDIR)/main/main.cc,$(call rwildcard,$(SRCDIR)/test/,*.cc)) $(SRCDIR)/test/test.cc
INCLUDES	:= $(call rwildcard,$(INCDIR)/,*.hh)

OBJS_APP		:= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.cc=.o))
NODIR_OBJS_APP	:= $(patsubst %,$(OBJDIR)/%,$(notdir $(OBJS_APP)))
OBJS_TEST		:= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(TEST:.cc=.o))
NODIR_OBJS_TEST	:= $(patsubst %,$(OBJDIR)/%,$(notdir $(OBJS_TEST)))

#
# APP RULES
#
# Creación de la app (Opcion por defecto)
app: directories app$(TARGET)
	@echo "\e[1;32m[$@] Programa compilado y enlazado\e[0m"

# Opción de DEBUG
debug: FLAGS += $(DEBUG_FLAG)
debug: TARGET := $(TARGET)_debug
debug: app

# Limpieza + opcion por defecto
remakeApp: clean app
	@echo "\e[1;34m[$@] Remake del programa completado\e[0m"

# Enlazado del programa
app$(TARGET): $(NODIR_OBJS_APP)
	$(CC) $(FLAGS) -o $(TARGETDIR)/$(TARGET).out $^ $(INC) $(LIBS)
	@echo "\e[1;34m[$@] Codigo del programa enlazado\e[0m"

# Compilación
$(NODIR_OBJS_APP): $(OBJS_APP)
$(OBJS_APP): $(OBJDIR)/%.o: $(SRCDIR)/%.cc

# 
# TEST RULES
#
# Creación de los tests
test: TARGET := $(TARGET)_test
test: directories test$(TARGET)
	@echo "\e[1;32m[$@] Tests compilados y enlazados\e[0m"

# Limpieza + opcion por defecto
remakeTest: clean test
	@echo "\e[1;34m[$@] Remake de los tests completado\e[0m"

# Enlazado del programa
test$(TARGET): $(NODIR_OBJS_TEST)
	$(CC) $(FLAGS) -o $(TARGETDIR)/$(TARGET).out $^ $(INC) $(LIBS)
	@echo "\e[1;34m[$@] Tests del programa enlazados\e[0m"

# Compilación
$(NODIR_OBJS_TEST): $(OBJS_TEST)
$(OBJS_TEST): $(OBJDIR)/%.o: $(SRCDIR)/%.cc

#
# COMPILATION RULE
#
# Compilación de código
$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CC) $(FLAGS) -o $(OBJDIR)/$(notdir $@) -c $< $(INC)
	@echo "\e[1;34m[$@] Archivo $< compilado \e[0m"

#
# DIRECTORY RULES
#
# Creacion de directorios
directories:
	@mkdir -p $(OBJDIR) $(TARGETDIR) $(LIBDIR)
	@echo "\e[1;34m[$@] Directorios creados\e[0m"

#
# LIBRARY RULES
#
# Crea las librerías
libraries: sdk_core googletest
	@echo "\e[1;34m[$@] Librerías creadas\e[0m"

# Crea la librería sdk_core
skd_core:
	cd sdk_core/ && mkdir -p build
	cd build && cmake ..
	make &&	make install
	cd ../../
	@echo "\e[1;34m[$@] Librería $@ creada\e[0m"

# Crea la librería googletest
googletest:
	cd googletest/ && mkdir -p build
	cd build && cmake ..
	make &&	make install
	cd ../../
	@echo "\e[1;34m[$@] Librería $@ creada\e[0m"

#
# CLEAN RULES
#
# Limpiado de archivos ejecutables y objeto
clean: clean_obj
	$(RM) -r $(TARGETDIR) skd_core/build googletest/build
	@echo "\e[1;34m[$@] Borrado total completado\e[0m"
	
# Limpiado de archivos objeto
clean_obj:
	$(RM) -r $(OBJDIR)
	@echo "\e[1;34m[$@] Borrado de codigo objeto completado\e[0m"