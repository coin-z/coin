#include <coin-data.hpp>
#include <communicator.hpp>

int main(int argc, char *argv[]) {
    std::cout << "redirect: " << argv[0] << std::endl;

    std::string name = "coin";
    if(argc == 2)
    {
        name = argv[1];
    }

    coin::data::init(argc, argv);

    auto redirect = coin::data::Communicator::client<coin::data::Communicator::MutableServerReq, coin::data::Communicator::MutableServerAck>(
        "$$/" + name + "/service/mutable_communicator"
    );

    std::cout << "redirect url: " << redirect->name() << std::endl;

    auto req = coin::data::makeShmShared<coin::data::Communicator::MutableServerReq>();
    auto ack = coin::data::makeShmShared<coin::data::Communicator::MutableServerAck>();
    req->comm_id = "sub";
    req->target_name = "name";

    redirect->call(req, ack);

    std::cout << "ack:" << ack->msg << std::endl;

    size_t cnt = 0;

    auto work = std::thread([]{
        while(coin::data::ok())
        {
            coin::data::spin_once();
            usleep(1);
        }
    });

    work.join();

    return 0;
}