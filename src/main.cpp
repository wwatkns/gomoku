#include "gomoku.hpp"
#include "Game.hpp"
// #include "GraphicalInterface.hpp"

#include <bitset>
// #include "BitBoard.hpp"

namespace options {
    t_options      g_options = { 1, 5 };
}

static int      get_depth(int depth) {
    if (depth < 1) {
        return (10);
    }
    else if (depth > 11) {
        std::cout << "Are you sure?!" << std::endl;
        return (depth);
    }
    else
        return (depth);
}

static int      get_algo_type(int algo_type) {
    std::cout << "You have selected: ";
    switch (algo_type) {
        case 1:
            std::cout << "(1) default" << std::endl;
            return (1);
        case 2:
            std::cout << "(2) MinMax" << std::endl;
            return (2);
        case 3:
            std::cout << "(3) AlphaBeta" << std::endl;
            return (3);
        case 4:
            std::cout << "(4) MTDf" << std::endl;
            return (4);
        case 5:
            std::cout << "(5) MCTS" << std::endl;
            return (5);
        default:
            std::cout << "a wrong input, set to (1) default" << std::endl;
            return (1);
    }
}

static void     print_options(void) {
    std::cout << "Welcome in:" << std::endl;
    std::cout << "\t ██████╗  ██████╗ ███╗   ███╗ ██████╗ ██╗  ██╗██╗   ██╗™" << std::endl;
    std::cout << "\t██╔════╝ ██╔═══██╗████╗ ████║██╔═══██╗██║ ██╔╝██║   ██║" << std::endl;
    std::cout << "\t██║  ███╗██║   ██║██╔████╔██║██║   ██║█████╔╝ ██║   ██║" << std::endl;
    std::cout << "\t██║   ██║██║   ██║██║╚██╔╝██║██║   ██║██╔═██╗ ██║   ██║" << std::endl;
    std::cout << "\t╚██████╔╝╚██████╔╝██║ ╚═╝ ██║╚██████╔╝██║  ██╗╚██████╔╝" << std::endl;
    std::cout << "\t ╚═════╝  ╚═════╝ ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ " << std::endl;
    std::cout << "\tMade with ♥︎ by wwatkins and ademenet. Please enjoy." << std::endl;
    return ;
}

int             main(int argc, char **argv) {
    // uint64_t    mask = 1;
    //
    // for (int i = 0; i < 32; i++) {
    //     mask |= mask << 1;
    //     std::cout << std::bitset<64>(mask) << " : " << std::hex << mask << std::endl;
    // }
    //
    // exit(1);

    // TODO: Add argparser in order to select algorithm
    try {
        boost::program_options::options_description     desc("Options");
        desc.add_options()
            ("help,h", "Print help options")
            ("ai,a", boost::program_options::value<int>(), "Choose AI algorithm:\n(1) default,\n(2) MinMax,\n(3) AlphaBeta,\n(4) MTDf,\n(5) MCTS")
            ("depth,d", boost::program_options::value<int>(), "Select the maximum depth");
        boost::program_options::variables_map           vm;
        try {
            boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
            boost::program_options::notify(vm);
            if (vm.count("help")) {
                print_options();
                std::cout << desc << std::endl;
                return (0);
            }
            if (vm.count("ai")) {
                options::g_options.algo_type = get_algo_type(vm["ai"].as<int>());
            }
            if (vm.count("depth")) {
                options::g_options.depth = get_depth(vm["depth"].as<int>());
                std::cout << "Maximum depth: " << options::g_options.depth << std::endl;
            }
        }
        catch(boost::program_options::error& e) {
            std::cerr << "Error: " << e.what() << std::endl << desc << std::endl;
            return (1);
        }
        Game    *game = new Game();        
        game->loop(); // Start the game
    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
        return (2);
    }
    return (0);
}
