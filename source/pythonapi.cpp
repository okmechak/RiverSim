#include <boost/python.hpp>

#include "app.hpp"

using namespace boost::python;

char const* greet()
{
    return "Some text is going here.. blablabla";
}

BOOST_PYTHON_MODULE(riversimPY)
{
    def("greet", greet);


    class_<River::Point>("Point")
        .def(init<double, double>())
    ;
}