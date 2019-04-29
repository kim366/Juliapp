const include_regex = Regex(raw"""^#include [<"](([A-Z][a-z0-9]+)+\.(hpp|inl))[">]$""", "m")

const header = """
/*

Juliapp
=======
Seamlessly embed Julia code in C++ with an API using modern language features.
https://github.com/kim366/Juliapp

=======

$(read("LICENSE", String))
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
