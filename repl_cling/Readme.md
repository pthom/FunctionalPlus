# Using fplus in a functional REPL via cling


## About cling

> Cling is an interactive C++ interpreter, built on top of Clang and LLVM compiler infrastructure. Cling realizes the read-eval-print loop (REPL) concept, in order to leverage rapid application development. Implemented as a small extension to LLVM and Clang, the interpreter reuses their strengths such as the praised concise and expressive compiler diagnostics.

See https://github.com/root-project/cling and https://cdn.rawgit.com/root-project/cling/master/www/index.html


## Installation

### Linux

Nightly build are available here: https://root.cern.ch/download/cling/

Download them and add to your path.

### Mac

````bash
brew install cling
````

### Usage

cling is a mix of `clang` + REPL. Thus, it accepts the standard clang argument (`-L`, `--std=c++14`, `-I`, etc)


## Example usage with fplus and opencv

### preparation
As of now, a copy of fplus include dir is required here:
````bash
cp -a ../include/fplus .
````

### An example interactive session:

Launch `cling` with C++14 support (required by fplus).

````bash
cling --std=c++14
````

Then, enter these command one by in the cling interpreter:

````cpp

// this will load the file init.cpp :
// - set some include and library paths
// - load opencv libraries
// - include opencv main files
// - include fplus
.L init.cpp

// this will load an image, and show it immediately
cv::Mat lena = cv::imread("lena.jpg");
cv::imshow("lena", lena); cv::waitKey(100);

// let's try some image manipulation
cv::Mat blur;
cv::blur(lena, blur, cv::Size(15, 15));
cv::imshow("blur", blur); cv::waitKey(100);


//
// Now, onto some functional REPL
//
std::string input = "Lorem ipsum \ndolor sit amet,\nconsectetur,\nadipisci velit";

// Let's try to split some lines
const auto lines = fplus::split_lines(input, false);
// input_line_5:2:21: error: no matching function for call to 'split_lines'
//  const auto lines = fplus::split_lines(input, false);
//                     ^~~~~~~~~~~~~~~~~~
// fplus_include/fplus/string_tools.hpp:74:14: note: candidate function not viable: no known conversion from 'std::string' (aka 'basic_string<char, char_traits<char>, allocator<char> >') to 'bool' for 1st argument
// ContainerOut split_lines(bool allowEmpty, const String& str)

// ooops, I was wrong in the argument order; but I got an immediate feedback
// (no neded to wait for compilation)
// Let's correct this:
const auto lines = fplus::split_lines(false, input);

// Let's see those lines:
lines
// -> (std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > &) { "Lorem ipsum ", "dolor sit amet,", "consectetur,", "adipisci velit" }

// Let's continue
typedef std::set<std::string::value_type> character_set;
const auto sets = fplus::transform(
    fplus::convert_container<character_set, std::string>,
    lines);

sets
// -> (std::vector<std::set<char, std::less<char>, std::allocator<char> >, std::allocator<std::set<char, std::less<char>, std::allocator<char> > > > &) { { ' ', 'L', 'e', 'i', 'm', 'o', 'p', 'r', 's', 'u' }, { ' ', ',', 'a', 'd', 'e', 'i', 'l', 'm', 'o', 'r', 's', 't' }, { ',', 'c', 'e', 'n', 'o', 'r', 's', 't', 'u' }, { ' ', 'a', 'c', 'd', 'e', 'i', 'l', 'p', 's', 't', 'v' } }

const auto gem_elements = fplus::fold_left_1(
    fplus::set_intersection<character_set>, sets);
gem_elements
// -> (std::set<char, std::less<char>, std::allocator<char> > &) { 'e', 's' }

````


## Advices

### You can test this in a docker container

If you do not want to install cling on your machine, or if you are not running under linux,
you can use the docker image provided inside `repl_cling/Docker/ubuntu`.

Refer to [Docker/ubuntu/Readme.md](Docker/ubuntu/Readme.md)

### Mac OS

On Mac OS, if opencv is not installed in a standard path, you can fill `DYLD_LIBRARY_PATH`

````bash
ln -s /path/to/your/opencv/install opencv-install
export DYLD_LIBRARY_PATH=$(pwd)/opencv-install/lib  # this is MacOS specific
````

Also, use `init.mac.cpp` instead of `init.cpp`
