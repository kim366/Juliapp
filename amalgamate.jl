const include_regex = Regex(raw"""^#include [<"](([A-Z][a-z0-9]+)+\.(hpp|inl))[">]$""", "m")

const header = """
/*

Juliapp
=======
Seamlessly embed Julia code in C++ with an API using modern language features.
https://github.com/kim366/Juliapp

=======

BSD 2-Clause License

Copyright (c) 2018-2019, Kim Schmider
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef JULIAPP_SINGLE_INCLUDE_HPP
#define JULIAPP_SINGLE_INCLUDE_HPP"""

const footer = """

#endif // JULIAPP_SINGLE_INCLUDE_HPP
"""

readheader(filename) = replace(read("src/$filename", String), "#pragma once" => "")
stripnewlines(content) = replace(content, Regex(raw"^\n\n+$", "m") => "\n")

let sourcecode = readheader("Julia.hpp")
  included_headers = []

  while true
    current_match = match(include_regex, sourcecode)
    current_match == nothing && break
    filename_to_include = current_match.captures[1]

    if filename_to_include ∉ included_headers
      content_to_substitute = readheader(filename_to_include)
      push!(included_headers, filename_to_include)
    else
      content_to_substitute = ""
    end

    sourcecode = sourcecode[1:current_match.offset - 1] *
      content_to_substitute *
      sourcecode[current_match.offset + length(current_match.match):end]
  end

  write("single_include/juliapp/julia.hpp", header * stripnewlines(sourcecode) * footer)
end
