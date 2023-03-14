#include <iostream>
#include <string>
#include "Helper.hpp"
#include "HCTree.hpp"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

using namespace std;


/*
References:
https://stackoverflow.com/questions/6121094/how-do-i-run-a-program-with-commandline-arguments-using-gdb-within-a-bash-script
https://stackoverflow.com/questions/29776831/valgrind-address-is-8-bytes-before-a-block-of-size-16-allocd
https://stackoverflow.com/questions/2902064/how-to-track-down-a-double-free-or-corruption-error
https://linuxhint.com/double-free-corruption-error/#:~:text=The%20error%20of%20double%20free,directly%20using%20the%20raw%20pointer.
https://groups.google.com/g/julia-users/c/FV9AmKsZNuk#:~:text=A%20double%20free%20or%20corruption,library%20that%20your%20code%20calls.
https://cplusplus.com/articles/DEN36Up4/
https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f
https://www.baeldung.com/cs/binary-tree-serialize-deserialize
https://stackoverflow.com/questions/3919850/conversion-from-myitem-to-non-scalar-type-myitem-requested
https://stackoverflow.com/questions/8931547/using-class-pointers-vs-instance


valgrind --leak-check=yes ./compress example_files/alphaext.txt oo1.txt
valgrind --leak-check=yes ./uncompress oo1.txt o1.txt

gdb --args ./compress rq.txt b.txt


./compress example_files/dna.txt coded.txt
./uncompress coded.txt decoded.txt
diff example_files/dna.txt decoded.txt


1.)   g++ -Wall -pedantic -pg -g -O0 -std=c++11 -o compress compress.cpp
 
can edit make file for above ^^
and then do
make compress

2.)  ./compress <input_file> <output_file>
3.)   gprof compress | less -S     OR       gprof compress > result.txt
         (show in terminal)                     (save to file)




https://stackoverflow.com/questions/16365155/removing-a-newline-character-at-the-end-of-a-file 

*/




/* 
Correct command line argument format:
./compress <original_file> <compressed_file>

Note, path needs to be specified:
./compress example_files/dna.txt example_files/empty.txt

- argv[0] element is the program name
- argv[1] element is <original_file> name, i.e. input file name 
- argv[2] element is <compressed_file> name, i.e. output file name

argc should equal 3
*/


int main(int argc, char* argv[]) {

    /* 
     * Parse command line arguments and throw error message if user runs program incorrectly
     */

    // Check for correct number of input arguments
    if (argc != 3) { 
        cerr << "Wrong number of arguments. Correct format is:\n./compress <original_file> <compressed_file>" << endl;
        return EXIT_FAILURE;
    }
    // check if input file can be opened
    ifstream argument_file(argv[1], ios::in);
    if (!argument_file.is_open()) {
        cerr << "Could not open file: " << argv[1] << "\nFile name or path may be incorrect." << endl;
        return EXIT_FAILURE;
    }

    // open output file
    FancyOutputStream o_stream(argv[2]);


    FancyInputStream i_stream(argv[1]);
    i_stream.reset();

    /*
    Should store filesize in a variable since every time you call filesize() it reads
    from the disk, which takes a long time / is very expensive
    */
    int input_file_size = i_stream.filesize();
    //cout << "Input file size: " << input_file_size << endl;


    // empty file case
    if (input_file_size == 0) {
        return EXIT_SUCCESS; 
    }



    // Read bytes from the file. Count the number of occurrences of each byte value
    vector<int> freq_list(256, 0); // value at index 0 will be the number of times ASCII symbol 0 appears in file
    
    for (int i = 0; i < input_file_size; i++) {
        int symbol = i_stream.read_byte();
        freq_list[symbol] = freq_list[symbol] + 1;
    }



    HCTree h_tree;
    h_tree.build(freq_list);
    h_tree.print_tree();

    

    /*
    File Header: NAIVE APPROACH  
    (1024 bytes, 4 bytes for each of 256 characters)
    */
    for (int i = 0; i < 256; i++) {
        o_stream.write_int(freq_list[i]);
    }


    // Move back to the beginning of the input file to be able to read it, again
    i_stream.reset(); 

    // Using Huffman tree, encode each byte from the input file, and append these codes as a sequence of bits to the output file, after the header
    for (int i = 0; i < input_file_size; i++) {
        unsigned char symbol = i_stream.read_byte();
        h_tree.encode(symbol, o_stream);
    }

    

    // Close input and output files; Handled by Destructors of FancyInputStream and FancyOutputStream



    return EXIT_SUCCESS;


}

