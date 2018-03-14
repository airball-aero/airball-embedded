/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018, Ihab A.B. Awad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <iostream>
#include <string>
#include "data_source.h"
#include "airdata.h"

/**
 * Reads lines of probe data from stdin, and writes out the lines as read and
 * adds columns for the computed values using the current Airdata
 * implementation. The added columns are:
 *
 *     IAS (m/s)
 *     TAS (m/s)
 *     alpha (radians)
 *     beta (radians)
 *     free stream Q (Pa)
 */
int main(int argc, char** argv) {
  airball::Airdata airdata;
  std::string s;
  while (true) {
    std::getline(std::cin, s);
    if (s == "") { break; }
    airdata.update_from_sentence(s);
    std::cout
        << s << ","
        << airdata.ias() << ","
        << airdata.tas() << ","
        << airdata.alpha() << ","
        << airdata.beta() << ","
        << airdata.free_stream_q()
        << std::endl;
  }
}
