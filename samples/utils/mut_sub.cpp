#include <coin-data.hpp>
#include <communicator.hpp>

int main(int argc, char *argv[]) {
    std::cout << "mut_sub: " << argv[0] << std::endl;

    coin::data::init(argc, argv);

    auto sub = coin::data::MutableSubscriber<int>::create("sub", [](coin::data::Subscriber<int>::ConstDataPtr data){
        std::cout << "data: " << *data << std::endl;
    });

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