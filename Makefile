# for building on linux run (without quotes): "make linux"
# for building on windows run (without quotes): "mingw32-make windows"


main_cpp = ./app/main.cpp
main_o = ./app/main.o
app_name = gem_puzzle
output_path_windows = /Build/Windows/
output_path_linux = /Build/Linux/



SFML_include_path = ".\src\include"
SFML_library_path = ".\src\lib"
SFML_libraries = -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio
SFML_static_libraries = -lsfml-system-s -lsfml-window-s -lsfml-graphics-s -lsfml-audio-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows


linux: compile link_linux clean run_linux

windows: compile link_windows clean run_windows


compile:
	g++ -c $(main_cpp) -o $(main_o) -I$(SFML_include_path)


link_linux:
	g++ $(main_o) -o .$(output_path_linux)$(app_name) -L$(SFML_library_path) $(SFML_libraries)

link_windows:
	g++ $(main_o) -o .$(output_path_windows)$(app_name).exe -L$(SFML_library_path) $(SFML_libraries)


clean: 
	rm -f $(main_o)


run_linux:
	.$(output_path_linux)$(app_name)

run_windows:
	.$(output_path_windows)$(app_name).exe






# compile_static_link

compile_static_link:
	g++ -c $(main_cpp) -o $(main_o) -I$(SFML_include_path) -DSFML_STATIC

	g++ $(main_o) -o .$(output_path_linux)$(app_name) -L$(SFML_library_path) $(SFML_static_libraries)

	rm -f $(main_o)

	.$(output_path_linux)$(app_name)