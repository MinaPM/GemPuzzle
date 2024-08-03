# for building on linux run (without quotes): "make linux"
# for building on windows run (without quotes): "mingw32-make windows" or "mingw32-make windows-d"

app_name = gem_puzzle

source_file = ./app/main.cpp
output_path_windows = ./Build/Windows/
output_path_linux = ./Build/Linux/

SFML_include_path = ./src/include
SFML_library_path = ./src/lib
SFML_libraries =  -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system

SFML_static_libraries = \
-lsfml-window-s \
-lsfml-graphics-s \
-lsfml-window-s \
-lsfml-audio-s \
-lsfml-system-s \
-lopengl32 \
-lwinmm \
-lgdi32 \
-lopengl32 \
-lfreetype \
-lopenal32 \
-lflac \
-lvorbisenc \
-lvorbisfile \
-lvorbis \
-logg \
-lwinmm \
-mwindows



#Windows------------------------
windows: build-windows run-windows
windows-d: build-windows-d run-windows-d

#static link
build-windows:
	g++ $(source_file) \
	-o $(output_path_windows)$(app_name).exe \
	-I$(SFML_include_path) \
	-L$(SFML_library_path) \
	-DSFML_STATIC \
	$(SFML_static_libraries)

run-windows:
	$(output_path_windows)$(app_name).exe



#dynamic link
build-windows-d:
	g++ $(source_file) \
	-o $(output_path_windows)"Dynamic Link/"$(app_name).exe \
	-I$(SFML_include_path) \
	-L$(SFML_library_path) \
	$(SFML_libraries)

run-windows-d:
	$(output_path_windows)"Dynamic Link/"$(app_name).exe



#Linux------------------------
linux:
	g++ -c $(source_file) -o $(main_o_linux) -I$(SFML_include_path) -DSFML_STATIC
	g++ $(source_file) -o $(output_path_linux)$(app_name) -L$(SFML_library_path) $(SFML_libraries)
	rm -f $(main_o_linux)
	$(output_path_linux)$(app_name)

