To install programmes required for compilation as well as the SDK + headers refer to:

https://github.com/WRFan/jailbreak10.3.3/blob/main/install

To actually compile, "cd" to the source directory, then:

	mkdir build

	cd build

	cmake .. -DCMAKE_C_COMPILER="/usr/lib/llvm-10/bin/clang-10" -DCMAKE_CXX_COMPILER="/usr/lib/llvm-10/bin/clang-10" -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_SYSROOT="/usr/include/iPhoneOS10.3.sdk" -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF -DCMAKE_C_FLAGS:STRING="-Ofast -DNDEBUG -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-command-line-argument -Wno-unknown-pragmas -Wno-format-security -Wno-unused-function -flto=thin -fwhole-program-vtables -funroll-loops -lto_library /usr/lib/llvm-10/lib/libLTO.dylib" -DCMAKE_CXX_FLAGS:STRING="-Ofast -DNDEBUG -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-command-line-argument -Wno-unknown-pragmas -Wno-format-security -Wno-unused-function -flto=thin -fwhole-program-vtables -funroll-loops -lto_library /usr/lib/llvm-10/lib/libLTO.dylib"

	cmake --build . --config Release
