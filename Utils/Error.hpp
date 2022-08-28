/****
 **
 ** @copyright copyright (c) 2022
 **
 **
 ** http://www.boost.org/LICENSE_1_0.txt
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

#ifndef __ERROR_HPP_
#define __ERROR_HPP_


#pragma once
#include <iostream>

#include <exception>
#include <string>


///////////////////////////////////////////////////////////////////////////
/// This file contains implementation of the helper logic for error handling
/// used internally.
///////////////////////////////////////////////////////////////////////////



class Error : public std::exception {
    public:
        Error(const char* message): message(message){}
        Error(){}


        std::string getErrorMessage() const{
            return message;
        }

        /// Method to be used for checking required conditions.
        void CHECK(bool check, const char* message) {
            if (!check) {
                throw Error(message);
            }
        }
   
    private:
        std::string message;
};

#endif


