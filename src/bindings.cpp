#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "board.hpp"

namespace py = pybind11;

PYBIND11_MODULE(BoardModule, m) {
    py::class_<Board>(m, "Board")
        .def("get_legal_moves", &Board::get_legal_moves);
        // Add bindings for other Board methods here

}