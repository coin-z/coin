#pragma once

#include <QtCore/QObject>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModelRegistry>

using QtNodes::DataFlowGraphModel;
using QtNodes::NodeDelegateModelRegistry;

namespace coin::workshop
{
class CoinDataFlowGraphModel
{
public:
    CoinDataFlowGraphModel(const CoinDataFlowGraphModel&) = delete;
    CoinDataFlowGraphModel& operator=(const CoinDataFlowGraphModel&) = delete;

    ~CoinDataFlowGraphModel() = default;

    static CoinDataFlowGraphModel& instance();
    static DataFlowGraphModel& graphModel();

private:
    CoinDataFlowGraphModel() = default;

};
}// namespace coin::workshop