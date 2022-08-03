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


enum ErrorCode {
    OK = 0,
    FAIL = -1,
    UNKNOWN_ERROR = -2
};


class Error : public std::exception {
    public:
        Error(const char* message, ErrorCode errorCode): message(message), errorCode(errorCode) {}

        ErrorCode getErrorCode() const {
            return errorCode;
        }

        /// Method to be used for checking required conditions.
        void CHECK(bool check, ErrorCode errorCode, const char* message) {
            if (!check) {
                throw Error(message, errorCode);
            }
        }
   
    private:
        std::string message;
        ErrorCode errorCode;
};

#endif


