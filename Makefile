# for building on linux run (without quotes): "make linux"
# for building on windows run (without quotes): "mingw32-make windows"

app_name = gem_puzzle


main_cpp_linux = ./app/main.cpp
main_o_linux = ./app/main.o
output_path_linux = ./Build/Linux/

main_cpp_windows = .\app\main.cpp
main_o_windows = .\app\main.o
output_path_windows = ./Build/Windows/



SFML_include_path = ".\src\include"
SFML_library_path = ".\src\lib"
SFML_libraries = -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio
SFML_static_libraries = -lsfml-graphics-s -lsfml-audio-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows


linux:
	g++ -c $(main_cpp_linux) -o $(main_o_linux) -I$(SFML_include_path) -DSFML_STATIC
	g++ $(main_cpp_linux) -o $(output_path_linux)$(app_name) -L$(SFML_library_path) $(SFML_libraries)
	rm -f $(main_o_linux)
	$(output_path_linux)$(app_name)


windows: compile_windows link_windows clean_windows run_windows


compile_windows:
	g++ -c $(main_cpp_windows) -o $(main_o_windows) -I$(SFML_include_path) -DSFML_STATIC

link_windows:
	g++ $(main_o_windows) -o $(output_path_windows)$(app_name).exe -L$(SFML_library_path) $(SFML_static_libraries)

clean_windows: 
	del $(main_o_windows)

run_windows:
	$(output_path_windows)$(app_name).exe




#sfml dynamicly linked libraries

windows_dynamic_link:

	g++ -c $(main_cpp) -o $(main_o) -I$(SFML_include_path)

	g++ $(main_o) -o $(output_path_windows)"Dynamic Link/"$(app_name).exe -L$(SFML_library_path) $(SFML_libraries)

	del $(main_o)

	$(output_path_windows)"Dynamic Link/"$(app_name).exe

