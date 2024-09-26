#pragma once

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QLabel>

#include <coin-node/package.hpp>
#include <coin-data/communicator.hpp>
#include <coin-commons/executor/executor.hpp>
#include <coin-commons/parmeter/anydata.hpp>
#include <workshop/dataflow_graphmodel.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

using QtNodes::NodeDataType;


namespace coin::workshop
{

/**
 * @brief The CoinNodeDataModel class, base node has 3 properties:
 *          - input port(s)
 *          - output port(s)
 *          - parameter(s)
*/

class CoinNodeDataModel : public NodeDelegateModel
{

    Q_OBJECT

public:
    enum class NodeWorkState
    {
        Normal,
        ExitNormal,
        Working,
        Crash
    };

    CoinNodeDataModel(const coin::node::PackageBaseInfo &info);

    virtual ~CoinNodeDataModel();

public:
    QString caption() const override { return QString("Base Node"); }
    bool captionVisible() const override { return false; }
    QString name() const override { return _name; }

    inline QString nodeName() const { return _nodeName; }
    void setName(const QString& name);

public:

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const port) override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex const portIndex) override;

    QWidget *embeddedWidget() override;

    QJsonObject save() const override;

    void load(QJsonObject const &data) override;

    const QJsonObject portConfigInfo() const;

signals:
    void nodeNameChanged(const QString& name);
    void portInfoChanged(const QJsonObject& info);

    void work_state_changed(const NodeWorkState& state);

private:
    QWidget *_widget;

    QString _name;
    QString _nodeName;
    coin::node::PackageBaseInfo _info;

    coin::Executor executor_;

    std::vector<NodeDataType> _inputDataTypes;
    std::vector<NodeDataType> _outputDataTypes;

    QJsonObject _portConfigInfo;

    TomlAnyDataTable _configs;

    void updatePorts(const std::string& ser, const std::string& id, const std::string& target);

public slots:
    virtual void inputConnectionCreated(QtNodes::ConnectionId const &id) override;

    virtual void inputConnectionDeleted(QtNodes::ConnectionId const &id) override;

    virtual void outputConnectionCreated(QtNodes::ConnectionId const &id) override;

    virtual void outputConnectionDeleted(QtNodes::ConnectionId const &id) override;

private slots:
    void on_node_start();
    void on_node_stop();
    void on_node_exit(const int st);
private:
    NodeWorkState node_state_;
public:
    NodeWorkState work_state() const
    {
        return node_state_;
    }
};

} // namespace coin::workshop
