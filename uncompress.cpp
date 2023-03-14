#include <iostream>
#include <string>
#include "Helper.hpp"
#include "HCTree.hpp"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

using namespace std;


/* 
Correct command line argument format:
./uncompress <compressed_file> <uncompressed_file>

Note, path needs to be specified:
./uncompress example_files/dna.txt example_files/empty.txt

- argv[0] element is the program name
- argv[1] element is <compressed_file> name, i.e. input file name 
- argv[2] element is <uncompressed_file> name, i.e. output file name

argc should equal 3

The outputted uncompressed_file should be the same as the original_file
compressed by ./compress
*/


int main(int argc, char* argv[]) {

    /* 
     * Parse command line arguments and throw error message if user runs program incorrectly
     */

    // Check for correct number of input arguments
    if (argc != 3) { 
        cerr << "Wrong number of arguments. Correct format is:\n./uncompress <compressed_file> <uncompressed_file>" << endl;
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

    //cout << "Input file size: " << i_stream.filesize() << endl;

    // empty file case
    if (i_stream.filesize() == 0) {
        return EXIT_SUCCESS;
    }

    

    vector<int> freq_list(256, 0); // value at index 0 will be the number of times ASCII symbol 0 appears in file


    int symbol_count = 0; // total number of symbols encoded in the file


    /* 
     * NAIVE APPROACH:
     * Read header (1024 bytes) into freq_list
     */
    for (int i = 0; i < 256; i++) {
        freq_list[i] = i_stream.read_int();

        if (freq_list[i] != 0) {
            symbol_count += freq_list[i];
            // cout << "(" << (unsigned char)i << "," << freq_list[i] << ")" << endl; // check freq_list is correct
        }
    }

    // build tree
    HCTree hc_tree;
    hc_tree.build(freq_list);
    // hc_tree.print_tree();



    // cout << "Symbol count: " << symbol_count << endl;

    // Using Huffman tree, decode the bits from input file into the appropriate sequence of bytes (char symbols), writing them to the output file
    // Remember: one byte is not the encoding for one symbol. The encoding for one symbol changes depending on path to leaf so don't read_byte in loop condition
    // Call decode the same number of times as the number of symbols in the file
    for (int i = 0; i < symbol_count; i++) {

        /*
        THIS PART MAY BE WRONG 
        */ 
        unsigned char symbol = hc_tree.decode(i_stream);
        o_stream.write_byte(symbol);
    }


   
    // Close input and output files; Handled by Destructors of FancyInputStream and FancyOutputStream


    return EXIT_SUCCESS;
}

