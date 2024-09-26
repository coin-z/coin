#include "communicator.hpp"
#include <sstream>
#include <stdlib.h>
#include <uuid/uuid.h>

namespace coin::data
{

MutableCommunicator::MutableCommunicator(const std::string& id) : id_(id)
{

}

std::shared_ptr<
std::map<std::string, MutableCommunicator::Ptr> > Communicator::mutable_communicator()
{
    static auto mc = std::make_shared<std::map<std::string, MutableCommunicator::Ptr>>();
    return mc;
}
Service<Communicator::MutableServerReq, Communicator::MutableServerAck>::Ptr Communicator::mutable_communicator_service_;

const std::string& Communicator::nodeName()
{
    return coin::data::local::LocalChannal::instance().selfName();
}

void Communicator::init(int argc, char *argv[])
{
    coin::data::local::LocalChannal::init(argc, argv);
    mutable_communicator_service_ = 
        coin::data::Communicator::service<MutableServerReq, MutableServerAck>("$$/" + nodeName() + "/service/mutable_communicator",
            std::bind(&Communicator::mutableCommunicatorServiceCallback_, std::placeholders::_1, std::placeholders::_2));

    spin_once();
}
void Communicator::spin_once()
{
    coin::data::local::LocalChannal::instance().spin_once();
}

void Communicator::registerMutableCommunicator_(const std::string& id, const MutableCommunicator::Ptr& mc)
{
    auto itor = coin::data::local::LocalChannal::communicators().find(id);
    if(itor != coin::data::local::LocalChannal::communicators().end())
    {
        std::cout << "redirect:[" << id << "] to" << std::endl;
        std::cout << itor->second << std::endl;
        mc->redirect( itor->second );
    }

    mutable_communicator()->insert_or_assign(id, mc);
}

bool Communicator::mutableCommunicatorServiceCallback_(
        Service<MutableServerReq, MutableServerAck>::ConstReqPtr& req,
        Service<MutableServerReq, MutableServerAck>::AckPtr& ack
    )
{
    ack->is_ok = true;
    auto id = toStdString(req->comm_id);
    auto itor = mutable_communicator()->find( id );
    if(itor == mutable_communicator()->end())
    {
        ack->is_ok = false;
        ack->msg = fromStdString("communicator <" + id + "> is not exist.");
        return true;
    }

    if(req->target_name.empty())
    {
        ack->msg = fromStdString("disconnect.");
        ack->is_ok = true;
        itor->second->disconnect();
    }
    else
    {
        ack->msg = fromStdString("redirect to <" + toStdString(req->target_name) + ">.");
        ack->is_ok = true;
        itor->second->redirect( toStdString( req->target_name ) );
    }

    return true;
}
} // namespace coin::data
