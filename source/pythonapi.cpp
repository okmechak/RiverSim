#include <boost/python.hpp>

#include "app.hpp"

using namespace boost::python;
using namespace River;

char const* greet()
{
    return "Some text is going here.. blablabla";
}

BOOST_PYTHON_MODULE(riversim)
{
    def("greet", greet);

    class_<River::Point>("Point")
        .def(init<double, double>(args( "x", "y" )))
        .def(init<const River::Point&>())
        .def(init<const River::Polar&>())
        .def("norm", static_cast< double (River::Point::*)() const>( &River::Point::norm ) )
        //.def("norm", static_cast< double (River::Point::*)(const double, const double) >( &River::Point::norm ), args("x", "y") )
        //.staticmethod("norm")
        .def("rotate", &River::Point::rotate, args("phi"), return_value_policy<reference_existing_object>())
        .def("normalize", &River::Point::normalize, return_value_policy<reference_existing_object>())//return value policy
        .def("angle", static_cast< double (River::Point::*)() const>( &River::Point::angle ))
        //.def("angle", static_cast<double (River::Point::*)(double, double) >( &River::Point::angle ), args("x", "y") )
        //.staticmethod( "angle" )
        .def("angle", static_cast< double (River::Point::*)(const River::Point &) const>( &River::Point::angle ), args("p") )
        .def(self + self) 
        .def(self - self) 
        .def(self += self)
        .def(self -= self)
        .def(self == self) 
        .def(self * self)
        .def(self * double())
        .def(self *= double())
        .def(self / double())
        .def(self /= double())
        .def("__getitem__", &River::Point::operator[])
        //def("__setitem__", &vector_setitem)
    ;
}