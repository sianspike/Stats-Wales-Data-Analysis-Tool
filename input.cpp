


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 950574

  This file contains the code responsible for opening and closing file
  streams. The actual handling of the data from that stream is handled
  by the functions in data.cpp. See the header file for additional comments.
  
  Each function you must implement has a TODO in its comment block. You may
  have to implement additional functions. You may implement additional
  functions not specified.
 */

#include "input.h"

#include <utility>

/*
  Constructor for an InputSource.

  @param source
    A unique identifier for a source (i.e. the location).
*/
InputSource::InputSource(std::string source): source(std::move(source)) {
}

/*
  This function should be callable from a constant context.

  @return
    A non-modifable value for the source passed into the constructor.
*/
std::string InputSource::getSource() const {

    return source;
}

/*
  Constructor for a file-based source.

  @param path
    The complete path for a file to import.

  @example
    InputFile input("data/areas.csv");
*/
InputFile::InputFile(const std::string& path) : InputSource(path), file_path(path) {
}

/*

 REFERENCES:
  https://stackoverflow.com/questions/36057028/throw-exception-if-the-file-does-not-exist-in
 -constructor-and-try-catch-it-when
 https://www.cplusplus.com/doc/oldtutorial/files/
 http://www.cplusplus.com/reference/istream/istream/istream/
 https://stackoverflow.com/questions/2399375/returning-ifstream-in-a-function
 https://stackoverflow.com/questions/843083/open-file-by-its-full-path-in-c
 https://stackoverflow.com/questions/11991657/returning-a-reference-to-istream-in-c
 https://www.cplusplus.com/doc/tutorial/files/
 https://www.educba.com/c-plus-plus-read-file/
 https://www3.ntu.edu.sg/home/ehchua/programming/cpp/cp10_IO.html


  Open a file stream to the file path retrievable from getSource()
  and return a reference to the stream.

  @return
    A standard input stream reference

  @throws
    std::runtime_error if there is an issue opening the file, with the message:
    InputFile::open: Failed to open file <file name>

  @example
    InputFile input("data/areas.csv");
    input.open();
*/
std::istream &InputFile::open() {

    file_stream.open(getSource());

    if (!file_stream.is_open()) {

        throw std::runtime_error("InputFile::open: Failed to open file " + getSource());
    }

    return file_stream;
}

std::string InputFile::getSource() const {

    return file_path;
}
