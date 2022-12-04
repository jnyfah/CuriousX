/****
 **
 ** @copyright copyright (c) 2022
 **
 **
 ** Distributed under the Boost Software License, Version 1.0.
 ** (See accompanying file LICENSE_1_0.txt or copy at
 ** http://www.boost.org/LICENSE_1_0.txt)
 **
 **
 ** @author Jennifer Chukwu
 ** @email: <jnyfaah@gmail.com>
 **
 ** see https://github.com/jnyfah/CuriousX for most recent version including documentation.
 ** Project CuriousX...2022
 **
 */


#ifndef __FILE_HANDLER_HPP_
#define __FILE_HANDLER_HPP_


///////////////////////////////////////////////////////////////////////////
/// This file contains implementation of the helper logic for argument parser
/// and file handling used internally.
///////////////////////////////////////////////////////////////////////////


#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>


#include "../LexicalAnalysis/include/LexerToken.hpp"
#include "../SyntaxAnalysis/include/Node.hpp"

#include "Error.hpp"


#ifdef __APPLE__
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

class FileHandler
{
public:
  FileHandler() {}

  Error error;

  bool ParseArguments(int argc, const char *argv[])
  {
    int start = 0;

    error.CHECK((argc >= 2), "no input file specified \n");
    filename = argv[++start];

    error.CHECK((fs::is_regular_file(fs::path(filename)) && fs::path(filename).extension() == ".txt"),
      "invalid input file path \n");


    inputFile.open(filename);

    error.CHECK(inputFile.is_open(), "cannot open input file \n");


    return true;
  }

  std::string getFileContents()
  {
    std::ostringstream sstr;
    sstr << inputFile.rdbuf();
    return sstr.str();
  }

  void LexerFile(std::vector<LexerToken> m_tokens)
  {
    std::fstream outputFile;

    outputFile.open("Lexical-analysis.txt", std::ios_base::out);

    error.CHECK(outputFile.is_open(), "failed to create lexical analysis file \n");

    for (auto x : m_tokens) {
      auto vv = "[" + std::string(x.value) + "]";
      outputFile << std::left << std::setw(6) << vv << " ->   " << x.location.toString() << ";\t " << toString(x.type)
                 << std::endl;
    }
  }

  void SyntaxFile(std::unique_ptr<Node> &root)
  {
    std::fstream outputFile;

    outputFile.open("Syntax-analysis.txt", std::ios_base::out);

    error.CHECK(outputFile.is_open(), "failed to create syntax analysis file \n");

    printTree(std::move(root), 0, outputFile);
  }


  void printTree(std::unique_ptr<Node> root, int space, std::ostream &out)
  {
    if (root == NULL) return;
    space += 2;
    printTree(std::move(root->right), space, out);
    for (int i = 1; i < space; i++) out << "\t";
    out << root->type.value << "|----"
        << "\n\n";
    printTree(std::move(root->left), space, out);
  }


  std::string getFilename() { return filename; }


private:
  std::string filename;
  std::fstream inputFile;
};

#endif
