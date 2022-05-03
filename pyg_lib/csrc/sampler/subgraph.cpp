#include "subgraph.h"

#include <ATen/core/dispatch/Dispatcher.h>
#include <torch/library.h>

namespace pyg {
namespace sampler {

std::tuple<at::Tensor, at::Tensor, c10::optional<at::Tensor>> subgraph(
    const at::Tensor& rowptr,
    const at::Tensor& col,
    const at::Tensor& nodes,
    const bool return_edge_id) {
  at::TensorArg rowptr_t{rowptr, "rowtpr", 1};
  at::TensorArg col_t{col, "col", 1};
  at::TensorArg nodes_t{nodes, "nodes", 1};

  at::CheckedFrom c = "subgraph";
  at::checkAllDefined(c, {rowptr_t, col_t, nodes_t});
  at::checkAllSameType(c, {rowptr_t, col_t, nodes_t});

  static auto op = c10::Dispatcher::singleton()
                       .findSchemaOrThrow("pyg::subgraph", "")
                       .typed<decltype(subgraph)>();
  return op.call(rowptr, col, nodes, return_edge_id);
}

c10::Dict<utils::RELATION_TYPE,
          std::tuple<at::Tensor, at::Tensor, c10::optional<at::Tensor>>>
hetero_subgraph(const utils::HETERO_TENSOR_TYPE& rowptr,
                const utils::HETERO_TENSOR_TYPE& col,
                const utils::HETERO_TENSOR_TYPE& nodes,
                const c10::Dict<utils::RELATION_TYPE, bool> return_edge_id) {
  c10::Dict<utils::RELATION_TYPE,
            std::tuple<at::Tensor, at::Tensor, c10::optional<at::Tensor>>>
      out_dict;
  return out_dict;
}

TORCH_LIBRARY_FRAGMENT(pyg, m) {
  m.def(TORCH_SELECTIVE_SCHEMA(
      "pyg::subgraph(Tensor rowptr, Tensor col, Tensor "
      "nodes, bool return_edge_id) -> (Tensor, Tensor, Tensor?)"));
  m.def(TORCH_SELECTIVE_SCHEMA(
      "pyg::hetero_subgraph(Dict(str, Tensor) rowptr, Dict(str, "
      "Tensor) col, Dict(str, Tensor) nodes, Dict(str, bool) "
      "return_edge_id) -> (Dict(str, Tensor), Dict(str, Tensor), "
      "Dict(str, Tensor?))"));
}

}  // namespace sampler
}  // namespace pyg
