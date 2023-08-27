#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "board.hpp"

namespace py = pybind11;

PYBIND11_MODULE(BoardModule, m) {
    py::class_<Board>(m, "Board")
        .def("get_legal_moves", &Board::get_legal_moves)
        // Add bindings for other Board methods here
        .def("get_pseudolegal_moves", &Board::get_pseudolegal_moves)
        .def("get_legal_moves", &Board::get_legal_moves)
        .def("get_pseudolegal_moves_for_piece", &Board::get_pseudolegal_moves_for_piece)
        .def("in_check", &Board::in_check)
        .def("copy", &Board::copy)
        .def("do_move", &Board::do_move)
        .def("undo_last_move", &Board::undo_last_move);
}
