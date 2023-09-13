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

    // bool ParseArguments(int argc, const char *argv[])
    // {
    //     int start = 0;

    //     error.CHECK((argc >= 2), "no input file specified \n");
    //     filename = argv[++start];

    //     error.CHECK((fs::is_regular_file(fs::path(filename)) && fs::path(filename).extension() == ".txt"),
    //       "invalid input file path \n");


    //     inputFile.open(filename);

    //     error.CHECK(inputFile.is_open(), "cannot open input file \n");


    //     return true;
    // }

    std::string getFileContents()
    {
        std::ostringstream sstr;
        sstr << inputFile.rdbuf();
        return sstr.str();
    }
    // ===================================================================================================//
    //                            DRAW SYMBOL TREE                                                //
    // ===================================================================================================//

    void SymbolTable()
    {
        std::fstream outputFile;

        outputFile.open("Lexical-analysis.txt", std::ios_base::out);
    }

    // ===================================================================================================//
    //                            WRITING TO LEXER FILE                                                   //
    // ===================================================================================================//
    // void LexerFile(std::vector<LexerToken> m_tokens)
    // {
    //     std::fstream outputFile;

    //     outputFile.open("Lexical-analysis.txt", std::ios_base::out);

    //     error.CHECK(outputFile.is_open(), "failed to create lexical analysis file \n");

    //     for (auto x : m_tokens)
    //     {
    //         auto vv = "[" + std::string(x.value) + "]";
    //         outputFile << std::left << std::setw(6) << vv << " ->   " << x.location.toString() << ";\t "
    //                    << toString(x.type) << std::endl;
    //     }
    // }


    // ===================================================================================================//
    //                            WRITING TO SYNTAX FILE                                                  //
    // ===================================================================================================//

    std::string SyntaxFileString(std::vector<std::shared_ptr<Node>> compound)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < compound.size(); i++)
        {
            oss << printTree(compound[i]);
            oss << "\n\n\n\n";
        }
        return oss.str();
    }



    // void CodeGenFile(const std::string &output)
    // {
    //     std::fstream outputFile;
    //     outputFile.open("assembly.txt", std::ios_base::out);

    //     error.CHECK(outputFile.is_open(), "failed to create syntax analysis file \n");

    //     outputFile << output;
    // }

    // void SemanticFile(const std::string &output)
    // {
    //     std::fstream outputFile;
    //     outputFile.open("symbol-tree.txt", std::ios_base::out);

    //     error.CHECK(outputFile.is_open(), "failed to create syntax analysis file \n");

    //     outputFile << output;
    // }


    std::string getFilename() { return filename; }


  private:
    std::string filename;
    std::fstream inputFile;



    // ===================================================================================================//
    //                            2D TREE VISUALIZER                                                      //
    // ===================================================================================================//
    void drawsNode(std::vector<std::string> &output,
      std::vector<std::string> &linkAbove,
      std::shared_ptr<Node> node,
      size_t level,
      int p,
      char linkChar)
    {
        if (!node) return;

        size_t height = output.size();
        std::string SP = " ";

        if (p < 0)
        {
            std::string extra(-p, ' ');
            for (std::string &s : output)
                if (!s.empty()) s = extra + s;
            for (std::string &s : linkAbove)
                if (!s.empty()) s = extra + s;
        }
        if (level < height - 1) p = std::max(p, (int)output[level + 1].size());
        if (level > 0) p = std::max(p, (int)output[level - 1].size());
        p = std::max(p, (int)output[level].size());

        // Fill in to left
        if (node->left)
        {
            std::string leftData = SP + node->left->type.value + SP;
            drawsNode(output, linkAbove, node->left, level + 1, int(p - leftData.size()), 'L');
            p = std::max(p, (int)output[level + 1].size());
        }

        // Enter this data
        size_t space = p - output[level].size();
        if (space > 0) output[level] += std::string(space, ' ');
        std::string nodeData = SP + node->type.value + SP;
        output[level] += nodeData;

        // Add vertical link above
        space = p + SP.size() - linkAbove[level].size();
        if (space > 0) linkAbove[level] += std::string(space, ' ');
        linkAbove[level] += linkChar;

        // Fill in to right
        if (node->right) drawsNode(output, linkAbove, node->right, level + 1, int(output[level].size()), 'R');
    }




    // ===================================================================================================//
    //                            2D TREE PRINT                                                           //
    // ===================================================================================================//
    std::string printTree(std::shared_ptr<Node> &root)
    {
        std::stringstream out;
        int height = TreeHeight(root);
        std::vector<std::string> output(height), linkAbove(height);
        drawsNode(output, linkAbove, root, 0, 5, ' ');

        // Create link lines
        for (size_t i = 1; i < height; i++)
        {
            for (size_t j = 0; j < linkAbove[i].size(); j++)
            {
                if (linkAbove[i][j] != ' ')
                {
                    size_t size = output[i - 1].size();
                    if (size < j + 1) output[i - 1] += std::string(j + 1 - size, ' ');
                    size_t jj = j;
                    if (linkAbove[i][j] == 'L')
                    {
                        while (output[i - 1][jj] == ' ') jj++;
                        for (size_t k = j + 1; k < jj - 1; k++) output[i - 1][k] = '_';
                    } else if (linkAbove[i][j] == 'R')
                    {
                        while (output[i - 1][jj] == ' ') jj--;
                        for (size_t k = j - 1; k > jj + 1; k--) output[i - 1][k] = '_';
                    }
                    linkAbove[i][j] = '|';
                }
            }
        }

        // Output
        for (size_t i = 0; i < height; i++)
        {
            if (i) { out << linkAbove[i] << '\n'; }
            out << output[i] << '\n';
        }
        return out.str();
    }
};


#endif
