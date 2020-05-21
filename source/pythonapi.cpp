#include "pythonapi.hpp"

char const* greet()
{
    return "hello, world";
}

BOOST_PYTHON_MODULE(riversimPY)
{
    def("greet", greet);
}