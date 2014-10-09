#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>


#include "../inout/include/inout.hpp"
#include "../guess_img/include/bfs_guess.hpp"
#include "../guess_img/include/blocked_guess.hpp"
#include "../modify_guess_image/modify_guess_image.hpp"
#include "../calc_exchange/include/greedy_calc_exchange.hpp"
#include "../utils/include/image.hpp"
#include "../utils/include/dwrite.hpp"
#include "../utils/include/exception.hpp"

using namespace procon;


template <typename T, typename S>
T readFrom(S& stream)
{
    T t;
    stream >> t;
    return t;
}


void appMain(std::string const & svrAddr, std::string const & tmToken)
{
    utils::write("Problem ID(ex. `03`) ----- ");
    auto pId = readFrom<std::string>(std::cin);
    auto p_opt = inout::get_problem(svrAddr, pId);

    PROCON_ENFORCE(p_opt, "Fail: download ppm file");

    const utils::Problem& pb = *p_opt;


    auto pred = [&](utils::Image const & img1,
                    utils::Image const & img2,
                    utils::Direction dir)
    {
        return bfs_guess::diff_connection(img1, img2, dir);
    };


    const auto select_cost = pb.select_cost();
    const auto change_cost = pb.change_cost();
    const auto max_select_times = pb.max_select_times();

    auto idxs = blocked_guess::guess(pb, pred);
    auto after = modify::modify_guess_image(idxs, pb,
        [=](std::vector<std::vector<utils::ImageID>> const & imgMap)
        {
            try{
                std::cout << std::dec;
                utils::writeln("[");
                for (auto& ee : imgMap){
                    utils::write("[");
                    for (auto& e: ee)
                        utils::writef("[%, %], ", e.get_index()[0], e.get_index()[1]);
                    utils::writeln("],");
                }
                utils::writeln("]");

                auto ss = greedy_calc_exchange::greedy_calc_exchange(imgMap, select_cost, change_cost, max_select_times);
                for(auto& e: ss)
                    utils::writeln(e);

                utils::writeln("Sending");
                PROCON_ENFORCE(inout::SendStatus::success == inout::send_result(svrAddr, tmToken, pId, ss), "Fail: sending an answer");
            }catch(std::exception& ex){
                utils::writeln(ex);
            }
        });
}


int main()
{
    const auto svrAddr = "localhost";       // http://{svrAddr}/SubmitAnswer
    const auto tmToken = "1";               // team token

    while(1){
        try{
            appMain(svrAddr, tmToken);
        }
        catch (std::exception& ex){
            utils::writeln("Error: ", ex);
        }
    }

    return 0;
}
