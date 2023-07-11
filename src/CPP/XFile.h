/* ******************************************************************************** */ 
/* MIT License                                                                      */
/*                                                                                  */
/* Copyright (c) 2023 Koray Üstündağ                                                */
/*                                                                                  */
/* Permission is hereby granted, free of charge, to any person obtaining a copy     */
/* of this software and associated documentation files (the "Software"), to deal    */
/* in the Software without restriction, including without limitation the rights     */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/* copies of the Software, and to permit persons to whom the Software is            */
/* furnished to do so, subject to the following conditions:                         */
/*                                                                                  */
/* The above copyright notice and this permission notice shall be included in all   */
/* copies or substantial portions of the Software.                                  */
/*                                                                                  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    */
/* SOFTWARE.                                                                        */
/* ******************************************************************************** */

#ifndef XFILE_H
#define XFILE_H
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <array>

class SourceFileIsNotXFileException : public std::exception
{
public:
    explicit SourceFileIsNotXFileException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

class XFile
{
public:
    static const std::array<uint8_t, 8> HEADER;
    static const std::array<uint8_t, 8> EOF_XFILE;

    static bool CheckIsXFile(const std::string& path);
    static std::vector<uint8_t> ReadAllBytes(const std::string& path);
    static void WriteAllBytes(const std::string& path, const std::vector<uint8_t>& bytes);
    static void Append(const std::string& path, const std::vector<uint8_t>& bytes);

private:
    template <size_t N>
    static bool CompareByteArrays(const std::vector<uint8_t>& array1, const std::array<uint8_t, N>& array2);
};

#endif
