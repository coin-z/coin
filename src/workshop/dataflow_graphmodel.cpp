#include <workshop/dataflow_graphmodel.hpp>

namespace coin::workshop
{
CoinDataFlowGraphModel& CoinDataFlowGraphModel::instance()
{
    static CoinDataFlowGraphModel model;
    return model;
}
DataFlowGraphModel& CoinDataFlowGraphModel::graphModel()
{
    static DataFlowGraphModel model = DataFlowGraphModel(std::make_shared<NodeDelegateModelRegistry>());
    return model;
}
} // namespace coin::workshop
