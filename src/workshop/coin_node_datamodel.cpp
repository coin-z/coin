#include "coin_node_datamodel.hpp"
#include <QtDebug>
#include <QCoreApplication>
#include <QMessageBox>
#include <QtNodes/NodeData>
#include <filesystem>
#include <iostream>

#include <coin-node/system.hpp>

#include "coin_embedded_widget.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;

namespace coin::workshop
{

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class TextData : public NodeData
{
public:
    TextData() {}

    TextData(QString const &text)
        : _text(text)
    {}

    NodeDataType type() const override { return NodeDataType{"text", "Text"}; }

    QString text() const { return _text; }

private:
    QString _text;
};


CoinNodeDataModel::CoinNodeDataModel(const coin::node::PackageBaseInfo &info)
  : _widget(nullptr), _info(info), node_state_(NodeWorkState::Normal)
  , _configs("config")
{

    _configs.add<std::string>("run_params", "");

    auto removeWhitespace = [](const std::string_view& str)
    {
        std::string s(str);
        s.erase(
            std::remove_if(s.begin(), s.end(), [](char c) {
                return std::isspace(c);
            }),
            s.end()
        );

        return QString::fromStdString(s);
    };

    _name = QString::fromStdString( info.name() );
    for(auto const& protocol : *info.connectorInfo())
    {
        auto obj = protocol.second->parse<coin::code::GrammarCommunicator>();

        for(auto itor = obj.begin(); itor != obj.end(); itor++)
        {
            if((*itor)->comm_type() == "reader")
            {
                _inputDataTypes.push_back(NodeDataType({
                    removeWhitespace((*itor)->data_type()),
                    removeWhitespace((*itor)->path()) + removeWhitespace((*itor)->name())}));
            }
            else if((*itor)->comm_type() == "writer")
            {
                _outputDataTypes.push_back(NodeDataType({
                    removeWhitespace((*itor)->data_type()),
                    removeWhitespace((*itor)->path()) + removeWhitespace((*itor)->name())}));
            }
            else if((*itor)->comm_type() == "subscriber")
            {
                _inputDataTypes.push_back(NodeDataType({
                    removeWhitespace((*itor)->data_type()),
                    removeWhitespace((*itor)->path()) + removeWhitespace((*itor)->name())}));
            }
            else if((*itor)->comm_type() == "publisher")
            {
                _outputDataTypes.push_back(NodeDataType({
                    removeWhitespace((*itor)->data_type()),
                    removeWhitespace((*itor)->path()) + removeWhitespace((*itor)->name())}));
            }
            else
            {
                std::cout << "Unknown comm type: " << (*itor)->comm_type() << std::endl;
            }
        }
    }

    executor_.set_receive_notification([this]{
        if(_widget)
        {
            do
            {
                auto text = executor_.read_front();
                if(text.empty()) break;
                size_t pos = text.find_last_not_of("\r\n");
                if (pos != std::string::npos) {
                    text.erase(pos + 1);
                }
                QCoreApplication::postEvent(_widget, new OnNodeDataReadyEvent(QString::fromStdString(text)));
            } while(true);
        }
    });

    executor_.set_exit_notification([this](int st){
        if(_widget)
        {
            QCoreApplication::postEvent(_widget, new OnNodeExitEvent(st));
        }
    });
}

CoinNodeDataModel::~CoinNodeDataModel()
{
    if(_widget)
    {
        _widget->deleteLater();
        _widget = nullptr;
    }
    executor_.force_shutdown();
}

void CoinNodeDataModel::setName(const QString & name)
{
    _nodeName = name;
    emit nodeNameChanged(name);
}

unsigned int CoinNodeDataModel::nPorts(PortType portType) const
{
    unsigned int result = 0;

    switch (portType) {
    case PortType::In:
        result = _inputDataTypes.size();
        break;

    case PortType::Out:
        result = _outputDataTypes.size();
        break;

    default:
        break;
    }

    return result;
}

NodeDataType CoinNodeDataModel::dataType(PortType portType, PortIndex portIndex) const
{
    if(portType == PortType::In)
    {
        return _inputDataTypes[portIndex];
    }
    else if(portType == PortType::Out)
    {
        return _outputDataTypes[portIndex];
    }
    else
    {
        return NodeDataType({"null", "invalid port type"});
    }
}

std::shared_ptr<NodeData> CoinNodeDataModel::outData(PortIndex const port)
{
    Q_UNUSED(port);
    return std::make_shared<TextData>();
}

void CoinNodeDataModel::setInData(std::shared_ptr<NodeData> data, PortIndex const portIndex)
{
}

QWidget *CoinNodeDataModel::embeddedWidget()
{
    if(not _widget)
    {
        _widget = new NodeEmbeddedWidget(_info);
        static_cast<NodeEmbeddedWidget*>(_widget)->load_env(executor_.env()->env());
        connect(static_cast<NodeEmbeddedWidget*>(_widget), &NodeEmbeddedWidget::node_start, this, &CoinNodeDataModel::on_node_start);
        connect(static_cast<NodeEmbeddedWidget*>(_widget), &NodeEmbeddedWidget::node_stop, this, &CoinNodeDataModel::on_node_stop);
        connect(static_cast<NodeEmbeddedWidget*>(_widget), &NodeEmbeddedWidget::node_exit, this, &CoinNodeDataModel::on_node_exit);
    }
    return _widget;
}

void CoinNodeDataModel::on_node_start()
{
    auto workdir = coin::SystemInfo::info().getParam<std::string>("workspace.path") + "/"
        + coin::node::Package::cate2Path(_info.category()) + "/" + _info.name() + "/build/";
    auto run_cmd = workdir + _info.name();
    executor_.env()->add_env("COIN_NODE_NAME", static_cast<NodeEmbeddedWidget*>(_widget)->name().toStdString());

    executor_.env()->add_env("COIN_NODE_COMMUNICATIONS", static_cast<NodeEmbeddedWidget*>(_widget)->node_comm_list());
    executor_.change_directory(workdir);
    executor_.execute(run_cmd, static_cast<NodeEmbeddedWidget*>(_widget)->run_params());

    node_state_ = NodeWorkState::Working;
    work_state_changed(NodeWorkState::Working);

    _configs.set<std::string>("run_params", static_cast<NodeEmbeddedWidget*>(_widget)->run_params());
}
void CoinNodeDataModel::on_node_stop()
{
    executor_.shutdown();
}
void CoinNodeDataModel::on_node_exit(const int st)
{
    if((st >> 8) == 0)
    {
        node_state_ = NodeWorkState::ExitNormal;
        emit work_state_changed(NodeWorkState::ExitNormal);
    }
    else
    {
        node_state_ = NodeWorkState::Crash;
        emit work_state_changed(NodeWorkState::Crash);
    }
}
QJsonObject CoinNodeDataModel::save() const
{
    auto w = static_cast<NodeEmbeddedWidget*>(_widget);
    ::toml::table table;
    _configs.encode(table);
    std::stringstream text;
    text << ::toml::toml_formatter{table};
    QJsonObject modelJson {
        {"model-name", name()},
        {"node-name", nodeName()},
        {"info", QJsonObject{
            {"name", QString::fromStdString(_info.name())},
            {"category", QString::fromStdString(_info.category())}
        }},
        {"node-config", QString::fromStdString( text.str() )}
    };

    return modelJson;
}

void CoinNodeDataModel::load(QJsonObject const &data)
{
    auto node_name = data["node-name"].toString();
    setName(node_name);
    auto node_config = data["node-config"].toString();
    auto table = ::toml::parse(node_config.toStdString());
    _configs.decode(table);
    static_cast<NodeEmbeddedWidget*>(_widget)->set_run_params(_configs.get<std::string>("run_params", ""));
}

void CoinNodeDataModel::inputConnectionCreated(QtNodes::ConnectionId const &id)
{
    auto model = CoinDataFlowGraphModel::graphModel().delegateModel<CoinNodeDataModel>(id.outNodeId);
    std::string target_name = 
        model->nodeName().toStdString() + "/" + model->_outputDataTypes[id.outPortIndex].name.toStdString();
    updatePorts(_nodeName.toStdString(),
        _inputDataTypes[id.inPortIndex].name.toStdString(),
        target_name
    );

    // 记录连接关系
    _portConfigInfo.insert(_inputDataTypes[id.inPortIndex].name, QString::fromStdString(target_name));
    emit portInfoChanged(_portConfigInfo);
}
void CoinNodeDataModel::inputConnectionDeleted(QtNodes::ConnectionId const &id)
{
    updatePorts(_nodeName.toStdString(),
        _inputDataTypes[id.inPortIndex].name.toStdString(),
        ""
    );

    // 删除连接关系
    _portConfigInfo.insert(_inputDataTypes[id.inPortIndex].name, QString::fromStdString(""));
    emit portInfoChanged(_portConfigInfo);
}
void CoinNodeDataModel::outputConnectionCreated(QtNodes::ConnectionId const &id)
{
    std::string target_name = 
        _nodeName.toStdString() + "/" + _outputDataTypes[id.outPortIndex].name.toStdString();
    updatePorts(_nodeName.toStdString(),
        _outputDataTypes[id.outPortIndex].name.toStdString(),
        target_name
    );

    // 记录连接关系
    _portConfigInfo.insert(_outputDataTypes[id.outPortIndex].name, QString::fromStdString(target_name));
    emit portInfoChanged(_portConfigInfo);
}
void CoinNodeDataModel::outputConnectionDeleted(QtNodes::ConnectionId const &id)
{
    qDebug() << _nodeName << "-" << __FUNCTION__ << " - " << __LINE__ << ": " << _outputDataTypes[id.outPortIndex].id << " - " << _outputDataTypes[id.outPortIndex].name;
}

void CoinNodeDataModel::updatePorts(const std::string& ser, const std::string& id, const std::string& target)
{
    // 短链接请求
    auto mut_client = coin::data::Communicator::client<
        coin::data::Communicator::MutableServerReq, 
        coin::data::Communicator::MutableServerAck>(
        "$$/" + ser + "/service/mutable_communicator"
    );

    if(not mut_client->isOnline())
    {
        return;
    }

    auto req = coin::data::makeShmShared<coin::data::Communicator::MutableServerReq>();
    auto ack = coin::data::makeShmShared<coin::data::Communicator::MutableServerAck>();
    req->comm_id = coin::data::fromStdString(id);
    req->target_name = coin::data::fromStdString(target);
    auto ret = mut_client->call(req, ack);
    if(not ack->is_ok)
    {
        QMessageBox::critical(nullptr, "error", QString::fromStdString(
            coin::data::toStdString(ack->msg)));
    }
}

const QJsonObject CoinNodeDataModel::portConfigInfo() const
{
    return _portConfigInfo;
}

} // namespace coin::workshop
