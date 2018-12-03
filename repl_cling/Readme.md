# Using fplus in a functional REPL via cling


## About cling

Cling is an interactive C++ interpreter, built on top of Clang and LLVM compiler infrastructure. Cling realizes the read-eval-print loop (REPL) concept, in order to leverage rapid application development. Implemented as a small extension to LLVM and Clang, the interpreter reuses their strengths such as the praised concise and expressive compiler diagnostics.

See https://github.com/root-project/cling and https://cdn.rawgit.com/root-project/cling/master/www/index.html


## Installation

### Mac

````bash
brew install cling
````

### Linux

Nightly build are available here: https://root.cern.ch/download/cling/


### Usage

cling is a mix of `clang` + REPL. Thus, it accepts the standard clang argument (-L, --std, -I, etc)



## Example usage with fplus and opencv


### preparation
````bash
ln -s /path/to/your/opencv/install opencv-install
export DYLD_LIBRARY_PATH=$(pwd)/opencv-install/lib  # this is MacOS specific
````

### An example interactive session:

````bash
cling --std=c++14 -Lstdc++
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
cv::imshow("lena", lena); cv::waitKey(10);

// let's try some image manipulation
cv::Mat blur;
cv::blur(lena, blur, cv::Size(15, 15));
cv::imshow("blur", blur); cv::waitKey(10);


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

// oops, it is not happy:
// input_line_8:2:40: error: no template named 'basic_string'; did you mean 'std::basic_string'?
//  cling::printValue(*(const std::vector<basic_string<char>, allocator<basic_string<char> > >**)0x7ffee7d0de18);
//                                        ^~~~~~~~~~~~
//                                        std::basic_string

// let's help it
using namespace std;
lines
// Still not happy !
// IncrementalExecutor::executeFunction: symbol '_ZNSt3__1plIcNS_11char_traitsIcEENS_9allocatorIcEEEENS_12basic_stringIT_T0_T1_EERKS9_PKS6_' unresolved while linking [cling interface function]!
// You are probably missing the definition of std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > std::__1::operator+<char, std::__1::char_traits<char>, std::__1::allocator<char> >(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&, char const*)
// Maybe you need to load the corresponding shared library?
// ERROR in cling's callPrintValue(): cannot pass value!
// (const std::vector<basic_string<char>, allocator<basic_string<char> > > &) ERROR in cling's callPrintValue(): missing value string.

// Ok, fplus::show to the rescue !
template <typename T> void print(const T &t)
{
  std::cout << fplus::show(t).c_str() << std::endl;
}


// Does it work?
print(lines)
// -> [Lorem ipsum , dolor sit amet,, consectetur,, adipisci velit]
// Yes !


// Let's continue
typedef std::set<std::string::value_type> character_set;
const auto sets = fplus::transform(
    fplus::convert_container<character_set, std::string>,
    lines);

print(sets)
// -> [[ , L, e, i, m, o, p, r, s, u], [ , ,, a, d, e, i, l, m, o, r, s, t], [,, c, e, n, o, r, s, t, u], [ , a, c, d, e, i, l, p, s, t, v]]

const auto gem_elements = fplus::fold_left_1(
    fplus::set_intersection<character_set>, sets);
print(gem_elements)
// -> [e, s]

````