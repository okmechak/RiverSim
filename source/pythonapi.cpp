#include "pythonapi.hpp"

#include "app.hpp"

char const* greet()
{
    return "Some text is going here.. blablabla";
}

BOOST_PYTHON_MODULE(reiversimPY2)
{
    //def("greet", greet);

    class_<ForwardRiverSimulation>("ForwardRiverSimulation")
        .def("Run", &App::Run);
}