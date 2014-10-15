#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>


#include "../inout/include/inout.hpp"
#include "../guess_img/include/bfs_guess.hpp"
#include "../guess_img/include/blocked_guess.hpp"
#include "../guess_img/include/correlation.hpp"
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


void appMain()
{
    utils::write("PPM file path ----- ");
    auto pId = readFrom<std::string>(std::cin);
    auto p_opt = utils::Problem::get(pId);

    PROCON_ENFORCE(p_opt, "Fail: download ppm file");

    const utils::Problem& pb = *p_opt;

    auto pred = guess::Correlator(pb);


    const auto select_cost = pb.select_cost();
    const auto change_cost = pb.change_cost();
    const auto max_select_times = pb.max_select_times();

    auto idxs = blocked_guess::guess(pb, pred);
    auto after = modify::modify_guess_image(idxs, pb,
        [=](std::vector<std::vector<utils::ImageID>> const & imgMap)
        {
            utils::writeln("cmp: ", utils::opCmp(imgMap, imgMap));

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
            }catch(std::exception& ex){
                utils::writeln(ex);
            }
        });
}


int main()
{
    while(1){
        try{
            appMain();
        }
        catch (std::exception& ex){
            utils::writeln("Error: ", ex);
        }
    }

    return 0;
}
