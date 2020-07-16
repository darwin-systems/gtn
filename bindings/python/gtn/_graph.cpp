#include <cstdint>
#include <sstream>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "gtn/gtn.h"

using namespace gtn;

namespace py = pybind11;
using namespace py::literals;

PYBIND11_MODULE(_graph, m) {
  py::class_<Graph>(m, "Graph")
      .def(py::init<bool>(), "calc_grad"_a = true)
      .def(
          "add_node",
          py::overload_cast<bool, bool>(&Graph::addNode),
          "start"_a = false,
          "accept"_a = false)
      .def("add_arc", py::overload_cast<int, int, int>(&Graph::addArc))
      .def(
          "add_arc",
          py::overload_cast<int, int, int, int, float>(&Graph::addArc),
          "upNode"_a,
          "downNode"_a,
          "ilabel"_a,
          "olabel"_a,
          "weight"_a = 0.0)
      .def("grad", (Graph & (Graph::*)()) & Graph::grad)
      .def("num_arcs", &Graph::numArcs)
      .def("num_nodes", &Graph::numNodes)
      .def("num_start", &Graph::numStart)
      .def("num_accept", &Graph::numAccept)
      .def("item", &Graph::item)
      .def("calc_grad", &Graph::calcGrad)
      .def("zero_grad", &Graph::zeroGrad)
      .def(
          "weights",
          [](Graph& g) {
            return reinterpret_cast<std::uintptr_t>(g.weights());
          })
      .def(
          "weights_to_list",
          [](Graph& g) {
            return std::vector<float>(g.weights(), g.weights() + g.numArcs());
          })
      .def(
          "weights_to_numpy",
          [](Graph& g) {
            std::vector<size_t> strides = {sizeof(float)};
            std::vector<size_t> shape = {static_cast<size_t>(g.numArcs())};
            size_t ndim = 1;

            return py::array(py::buffer_info(
                g.weights(),
                sizeof(float),
                py::format_descriptor<float>::value,
                ndim,
                shape,
                strides));
          })
      .def(
          "set_weights",
          [](Graph& g, const std::uintptr_t b) {
            g.setWeights(reinterpret_cast<float*>(b));
          })
      .def(
          "set_weights",
          [](Graph& g, const std::vector<float>& weights) {
            g.setWeights(weights.data());
          })
      .def(
          "set_weights",
          [](Graph& g, const py::array_t<float> weights) {
            g.setWeights(reinterpret_cast<float*>(weights.request().ptr));
          })
      .def("labels_to_list", &Graph::labelsToVector, "inLabel"_a = true)
      .def("__repr__", [](const Graph& a) {
        std::ostringstream ss;
        print(a, ss);
        return ss.str();
      });
  m.attr("epsilon") = Graph::epsilon;
}
